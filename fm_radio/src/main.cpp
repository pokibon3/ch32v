//
//	CW Decoder Ver 1.0
//
//  2024.08.12 New Create
//  2024.08.21 integer version
#define OLED_SPI
#define SERIAL_OUT

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
//#include <math.h>
#include "ch32v003fun.h"

// what type of OLED - uncomment just one
//#define SSD1306_64X32
//#define SSD1306_128X32
#define SSD1306_128X64
#ifdef OLED_SPI
#include "ssd1306_spi.h"
#else
#include "ssd1306_i2c.h"
#endif
#include "i2c_master.h"
#include "ssd1306.h"
#include "RDA5807M.h"
#define GPIO_ADC_MUX_DELAY 100
#define GPIO_ADC_sampletime GPIO_ADC_sampletime_43cy
#include "ch32v003_GPIO_branchless.h"

#define micros() (SysTick->CNT / DELAY_US_TIME)
#define millis() (SysTick->CNT / DELAY_MS_TIME)

//#define LED_PIN GPIOv_from_PORT_PIN(GPIO_port_D, 5)
#define SW1_PIN GPIOv_from_PORT_PIN(GPIO_port_D, 0)
#define SW2_PIN GPIOv_from_PORT_PIN(GPIO_port_D, 4)
#define SW3_PIN GPIOv_from_PORT_PIN(GPIO_port_C, 0)
#define ADC_PIN GPIOv_from_PORT_PIN(GPIO_port_D, 3)
#define SPK_PIN GPIOv_from_PORT_PIN(GPIO_port_D, 2)
#define LED_PIN GPIOv_from_PORT_PIN(GPIO_port_D, 5)
#define UART_PIN GPIOv_from_PORT_PIN(GPIO_port_D, 6)

#define I2C_ADDR_SEQ 0x10
#define I2C_ADDR_RND 0x11

uint16_t freq  = 8470  ;///Frequency! 80 82.5 84.7 
//	uint16_t freq  = 8000  ;///Frequency! 80 82.5 84.7 
//	uint16_t freq  = 8250  ;///Frequency! 80 82.5 84.7 
uint16_t vol  = 15        ;///Frequency! 80 82.5 84.7 

RDA5807M radio;


// function prototype (declaration), definition in "ch32v003fun.c"
extern "C" int mini_snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...);

//char title1[] = "FrequencyCounter";
static char title1[]   = "  FM Radio Ver 0.1";

void check_input() 
{
	if (GPIO_digitalRead(SW2_PIN)) {				// Dwon
//		freq -= 10;
//		if (freq < 7600) freq = 7600;
//		radio.setFrequency(freq);
		radio.seekDown(false);
		Delay_Ms(100);

	} else if (GPIO_digitalRead(SW3_PIN)) {			// UP
//		freq += 10;
//		if (freq > 10800) freq = 10800;
//		radio.setFrequency(freq);
		radio.seekUp(false);
		Delay_Ms(100);
	} else if (GPIO_digitalRead(SW1_PIN)) {			// Mode
		Delay_Ms(100);
	}
}


void setup()
{
	uint8_t buf[16];
	uint16_t len = 4;

    // 各GPIOの有効化
    GPIO_port_enable(GPIO_port_D);
    GPIO_port_enable(GPIO_port_C);
    // 各ピンの設定
    GPIO_pinMode(SW1_PIN, GPIO_pinMode_I_pullDown, GPIO_Speed_10MHz); /// GPIO_Speed_In? @see https://github.com/cnlohr/ch32v003fun/blob/master/examples/GPIO/GPIO.c#L55
    GPIO_pinMode(SW2_PIN, GPIO_pinMode_I_pullDown, GPIO_Speed_10MHz);
    GPIO_pinMode(SW3_PIN, GPIO_pinMode_I_pullDown, GPIO_Speed_10MHz);
    GPIO_pinMode(ADC_PIN, GPIO_pinMode_I_analog, GPIO_Speed_10MHz);
    
	GPIO_pinMode(LED_PIN, GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
	GPIO_digitalWrite(LED_PIN, low);

	GPIO_pinMode(UART_PIN, GPIO_pinMode_O_pushPullMux, GPIO_Speed_10MHz);
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO;

	// remap UART1  RXD : PD5, TXD : PD6  
	AFIO->PCFR1 &= ~GPIO_PartialRemap1_USART1;
	AFIO->PCFR1 &= ~GPIO_PartialRemap2_USART1;
	AFIO->PCFR1 |=  GPIO_PartialRemap2_USART1;
#ifdef OLED_SPI
	ssd1306_spi_init();		// i2c Setup
#else
	ssd1306_i2c_init();		// i2c Setup
#endif
	init_i2c_master();
//	i2c_init();
	ssd1306_init();			// SSD1306 Setup
	ssd1306_setbuf(0);		// Clear Screen
	ssd1306_drawstr_sz(0, 32, title1, 1, fontsize_8x8);
//	ssd1306_drawstr_sz(0, 40, title2, 1, fontsize_8x8);
	ssd1306_refresh();
	GPIO_ADCinit();
	radio.begin(RDA5807M_BAND_WORLD);
	Delay_Ms(500);
	// Setup RDA5807 
	//   reset 02H and 03H 
	//   TUNE set to 0
	//buf[0] = 0x00;  // 02h(HighByte02) DMUTE0
	//buf[1] = 0x00;  // 02l(LowByte02)
	//buf[2] = 0x00;  // 03h
	//buf[3] = 0x00;  // 03l TUNE0 ENABLE0
	//len = 4;

 	//i2c_send(I2C_ADDR_SEQ, buf, len);

	// write data from 02H to 05H. 
	//   FREQ -> CHAN -> 03
/*
	uint16_t chan = freq / 10 - 760;
	uint8_t  h3 = chan >> 2; //Highbyte03
	uint8_t  l3 = chan & 0b00000011;
	l3 = l3 <<6 ;
	l3 = l3 | 0b00011000; //Lowbyte03 TUNE1 BAND10 SPACE00

	buf[0] = 0xc2; 		//02h DMUTE1 SEEK0 水晶利用
	buf[1] = 0x85; 		//02l ENABLE1 CLK000(32768hz)
	buf[2] = h3; 		//03h freq 
	buf[3] = l3; 		//03l freq  TUNE0->1 BAND10
	buf[4] = 0b00001010;//04h DE50
	buf[5] = 0x00; 		//04l 
	buf[6] = 0x88; 		//05h 
	buf[7] = 0x81; 		//05l VOLUME0001
	len = 8;

	i2c_send(I2C_ADDR_SEQ, buf, len);
*/
}


int main()
{
	static uint32_t time = 0;
	char buf[32];
	SystemInit();			// ch32v003 sETUP
	setup();				// gpio Setup;
	Delay_Ms( 2000 );
//	printf("Start CW Decoder Version 1.0\n");
	ssd1306_setbuf(0);	// Clear Screen
	ssd1306_refresh();
	radio.setFrequency(freq);
	uint8_t  rs;
	uint16_t fr;
	radio.dumpRegister();
	while(1) {

		check_input();
		ssd1306_setbuf(0);
		mini_snprintf(buf, sizeof(buf), "FREQ:%3d.%1d", freq / 100, freq % 100 / 10);
		ssd1306_drawstr_sz(0, 0, buf, 1, fontsize_16);
		mini_snprintf(buf, sizeof(buf), "VOL :%3d", vol);
		ssd1306_drawstr_sz(0, 20, buf, 1, fontsize_16);
		uint32_t  t = millis();
		if ((t- time) > 100) {
			time = t;
			rs   = radio.getRSSI();
			freq = radio.getFrequency();
//    	    printf("RSSI = %d\n", rs);
//        	printf("FREQ = %d\n", freq);
		}
		mini_snprintf(buf, sizeof(buf), "RSSI:%3d", rs);
		ssd1306_drawstr_sz(0, 40, buf, 1, fontsize_16);
		ssd1306_refresh();

		Delay_Ms(1);
	}

}
