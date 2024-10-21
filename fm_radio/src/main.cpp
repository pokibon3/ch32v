//
//  FM Radio for RDA5307 Ver 1.0
//
//  2024.10.20 New Create
//
#define OLED_SPI
#define SERIAL_OUT

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
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
//#define GPIO_ADC_MUX_DELAY 100
//#define GPIO_ADC_sampletime GPIO_ADC_sampletime_43cy
#include "ch32v003_GPIO_branchless.h"

#define micros() (SysTick->CNT / DELAY_US_TIME)
#define millis() (SysTick->CNT / DELAY_MS_TIME)
// function prototype (declaration), definition in "ch32v003fun.c"
extern "C" int mini_snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...);

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

static struct _bandtable {
	char	 letter;
	uint16_t l_limit;
	uint16_t h_limit;
	uint16_t band;
	uint16_t space;
	uint16_t tune;
} band_table[3] = {
	{'H', 7600, 10800, RDA5807M_BAND_WORLD, RDA5807M_SPACE_100K, 8470},
	{'L', 5000,  7600, RDA5807M_BAND_50M,   RDA5807M_SPACE_50K,  5100}, 
//	{'M', 6500,  7600, RDA5807M_BAND_EAST,  RDA5807M_SPACE_100K, 6500},
//	{'L', 5000,  6500, RDA5807M_BAND_50M,   RDA5807M_SPACE_50K,  5100}, 
};

uint16_t freq  	= 8470;
uint16_t vol  	= 10;
uint16_t band 	= 0;
uint16_t rs 	= 0;
uint16_t func	= 0;

RDA5807M radio;

static char title1[]   = "  FM Radio Ver 0.9";

//===========================================================
//	setBand() : set band and spacing
//===========================================================
static void setBand()
{
	radio.setBand(band_table[band].band);
	radio.setSpacing(band_table[band].space);
	freq = band_table[band].tune;
	radio.setFrequency(freq);
		
	radio.dumpRegister();
}

//===========================================================
//	check_input() : check buttons 
//===========================================================
static void check_input() 
{
	if (GPIO_digitalRead(SW2_PIN)) {				// Down
		switch(func) {
			case 0:
				radio.seekDown(false);
				break;
			case 1:
				if (vol > 1) vol--;
				radio.setVolume(vol);
				break;
			case 2:
				band = 1;
				setBand();
				func = 0;
				break;
			default :
				break;
		}
		Delay_Ms(300);
	} else if (GPIO_digitalRead(SW3_PIN)) {			// UP
		switch(func) {
			case 0:
				radio.seekUp(false);
				break;
			case 1:
				if (vol < 15) vol++;
				radio.setVolume(vol);
				break;
			case 2:
				band = 0;
				setBand();
				func = 0;
				break;
			default :
				break;
		}
		Delay_Ms(300);
	} else if (GPIO_digitalRead(SW1_PIN)) {			// Band
		func++;
		if (func > 2) func = 0;
		Delay_Ms(300);
	}
}

//===========================================================
//	disp_menu()
//===========================================================
void disp_menu()
{
	char buf[32];

	ssd1306_setbuf(0);

	ssd1306_drawstr_sz(0 ,0 , "FM Radio", 1, fontsize_8x8);
	ssd1306_drawstr_sz(60 ,0 , "Band", !(func == 2), fontsize_8x8);
//	ssd1306_drawstr_sz(60 ,0 , "Band", 1, fontsize_8x8);
	mini_snprintf(buf, sizeof(buf), ":%c S:%2d", band_table[band].letter, rs);
	ssd1306_drawstr_sz(84 ,0 , buf, 1, fontsize_8x8);
	ssd1306_drawFastHLine(0, 13, 128, 1);

	mini_snprintf(buf, sizeof(buf), "%3d", freq / 100);
	ssd1306_drawstr_sz(4, 20, buf, 1, fontsize_32x32);
	mini_snprintf(buf, sizeof(buf), ".%02dM", freq % 100);
	ssd1306_drawstr_sz(72, 36, buf, 1, fontsize_16x16);
	int i;
	for (i = 0; i < vol; i++) {
		buf[i] = 0xff;
	}
	buf[i] = 0x00;
	ssd1306_drawstr_sz(0, 56, "Vol", !(func == 1), fontsize_8x8);
	ssd1306_drawstr_sz(18, 56, ":",  1, fontsize_8x8);
	ssd1306_drawstr_sz(24, 56, buf, 1, fontsize_8x8);

	ssd1306_refresh();
}

//===========================================================
//	setup()
//===========================================================
void setup()
{
//	uint8_t buf[16];
//	uint16_t len = 4;

    // 各GPIOの有効化
    GPIO_port_enable(GPIO_port_D);
    GPIO_port_enable(GPIO_port_C);
    // 各ピンの設定
    GPIO_pinMode(SW1_PIN, GPIO_pinMode_I_pullDown, GPIO_Speed_10MHz); /// GPIO_Speed_In? @see https://github.com/cnlohr/ch32v003fun/blob/master/examples/GPIO/GPIO.c#L55
    GPIO_pinMode(SW2_PIN, GPIO_pinMode_I_pullDown, GPIO_Speed_10MHz);
    GPIO_pinMode(SW3_PIN, GPIO_pinMode_I_pullDown, GPIO_Speed_10MHz);
    GPIO_pinMode(ADC_PIN, GPIO_pinMode_I_analog,   GPIO_Speed_10MHz);
    
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
//	GPIO_ADCinit();
	radio.begin(RDA5807M_BAND_WORLD);
	Delay_Ms(100);
}

//===========================================================
//	main()
//===========================================================
int main()
{
	static uint32_t time = 0;
	SystemInit();			// ch32v003 sETUP
	setup();				// gpio Setup;
	Delay_Ms( 1000 );
	ssd1306_setbuf(0);	// Clear Screen
	ssd1306_refresh();

	radio.setBand(band_table[band].band);
	radio.setSpacing(band_table[band].space);
	freq = band_table[band].tune;
	radio.setFrequency(freq);
	radio.setVolume(vol);

//	uint16_t fr;
	radio.dumpRegister();
	while(1) {

		check_input();
		uint32_t  t = millis();
		if ((t- time) > 100) {
			time = t;
			rs   = radio.getRSSI();
			freq = radio.getFrequency();
//  	printf("RSSI = %d\n", rs);
//      printf("FREQ = %d\n", freq);
			disp_menu();
		}

		Delay_Ms(1);
	}

}
