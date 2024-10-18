//
//	CW Decoder Ver 1.0
//
//  2024.08.12 New Create
//
#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include "goertzel.h"
#include "ch32v003fun.h"

// what type of OLED - uncomment just one
//#define SSD1306_64X32
//#define SSD1306_128X32
#define SSD1306_128X64
#include "ssd1306_spi.h"
#include "ssd1306.h"

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

#define SAMPLES 48 
#define SAMPLING_FREQUENCY 8000

// function prototype (declaration), definition in "ch32v003fun.c"
extern "C" int mini_snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...);

static uint16_t sampling_period_us;
static int16_t 	morseData[SAMPLES];

//char title1[] = "FrequencyCounter";
static char title1[]   = "CW Decoder V1.1";
//static char title2[]   = "  Version 1.1";
static uint16_t magnitudelimit = 100;
static uint16_t magnitudelimit_low = 100;
//static uint16_t magnitude;
static uint16_t realstate = low;
static uint16_t realstatebefore = low;
static uint16_t filteredstate = low;
static uint16_t filteredstatebefore = low;
static uint32_t starttimehigh;
static uint32_t highduration;
static uint32_t lasthighduration;
static uint32_t hightimesavg;
//static uint32_t lowtimesavg;
static uint32_t startttimelow;
static uint32_t lowduration;
static uint32_t laststarttime = 0;
//static uint16_t nbtime = 6;  /// ms noise blanker         
#define  nbtime 	6  /// ms noise blanker         

static char code[20];
static uint16_t stop = low;
static uint16_t wpm;

/////////////////////////////////////
// here we update the upper line   //
// with the speed.                 //
/////////////////////////////////////
void updateinfolinelcd()
{
	char buf[17];
	uint16_t w;

	if (wpm < 10) {
		w = 0;
	} else {
		w = wpm;
	}
	mini_snprintf(buf, sizeof(buf), "%2d WPM [US Mode]", w);
	ssd1306_drawstr_sz(0 ,2 , buf, 1, fontsize_8x8);
	ssd1306_drawFastHLine(0, 13, 128, 1);
	ssd1306_refresh();
}

const int colums = 8; /// have to be 16 or 20
const int rows = 4;  /// have to be 2 or 4

int lcdindex = 0;
uint8_t line1[colums];
uint8_t line2[colums];
uint8_t lastChar = 0;
/////////////////////////////////////
// print the ascii code to the lcd //
// one a time so we can generate   //
// special letters                 //
/////////////////////////////////////
void printascii(int asciinumber)
{
	int fail = 0;

	if (lastChar == 32 && asciinumber == 32) return;
	if (rows == 4 and colums == 16)fail = -4; /// to fix the library problem with 4*16 display http://forum.arduino.cc/index.php/topic,14604.0.html
	if (lcdindex > colums-1){
		lcdindex = 0;
		if (rows == 4){
			for (int i = 0; i <= colums - 1 ; i++){
				ssd1306_drawchar_sz(i * 16 ,(rows - 3) * 16 , line2[i], 1, fontsize_16x16);
				line2[i]=line1[i];
			}
		}
		for (int i = 0; i <= colums-1 ; i++){
			ssd1306_drawchar_sz((i + fail)* 16 ,(rows - 2) * 16 , line1[i], 1, fontsize_16x16);
			ssd1306_drawchar_sz((i + fail)* 16 ,(rows - 1) * 16 , 32, 1, fontsize_16x16);
		}
 	}
	line1[lcdindex]=asciinumber;
	ssd1306_drawchar_sz((lcdindex + fail)* 16 ,(rows - 1) * 16 , asciinumber, 1, fontsize_16x16);
	lcdindex += 1;
	ssd1306_refresh();
	lastChar = asciinumber;
}

////////////////////////////////
// translate cw code to ascii //
////////////////////////////////
void docode(){
    if (strcmp(code,".-") 	== 0) printascii(65);
	if (strcmp(code,"-...") == 0) printascii(66);
	if (strcmp(code,"-.-.") == 0) printascii(67);
	if (strcmp(code,"-..") 	== 0) printascii(68);
	if (strcmp(code,".") 	== 0) printascii(69);
	if (strcmp(code,"..-.") == 0) printascii(70);
	if (strcmp(code,"--.") 	== 0) printascii(71);
	if (strcmp(code,"....") == 0) printascii(72);
	if (strcmp(code,"..") 	== 0) printascii(73);
	if (strcmp(code,".---") == 0) printascii(74);
	if (strcmp(code,"-.-") 	== 0) printascii(75);
	if (strcmp(code,".-..") == 0) printascii(76);
	if (strcmp(code,"--") 	== 0) printascii(77);
	if (strcmp(code,"-.") 	== 0) printascii(78);
	if (strcmp(code,"---") 	== 0) printascii(79);
	if (strcmp(code,".--.") == 0) printascii(80);
	if (strcmp(code,"--.-") == 0) printascii(81);
	if (strcmp(code,".-.") 	== 0) printascii(82);
	if (strcmp(code,"...") 	== 0) printascii(83);
	if (strcmp(code,"-") 	== 0) printascii(84);
	if (strcmp(code,"..-") 	== 0) printascii(85);
	if (strcmp(code,"...-") == 0) printascii(86);
	if (strcmp(code,".--") 	== 0) printascii(87);
	if (strcmp(code,"-..-") == 0) printascii(88);
	if (strcmp(code,"-.--") == 0) printascii(89);
	if (strcmp(code,"--..") == 0) printascii(90);

	if (strcmp(code,".----") == 0) printascii(49);
	if (strcmp(code,"..---") == 0) printascii(50);
	if (strcmp(code,"...--") == 0) printascii(51);
	if (strcmp(code,"....-") == 0) printascii(52);
	if (strcmp(code,".....") == 0) printascii(53);
	if (strcmp(code,"-....") == 0) printascii(54);
	if (strcmp(code,"--...") == 0) printascii(55);
	if (strcmp(code,"---..") == 0) printascii(56);
	if (strcmp(code,"----.") == 0) printascii(57);
	if (strcmp(code,"-----") == 0) printascii(48);

	if (strcmp(code,"..--..") == 0) printascii(63);
	if (strcmp(code,".-.-.-") == 0) printascii(46);
	if (strcmp(code,"--..--") == 0) printascii(44);
	if (strcmp(code,"-.-.--") == 0) printascii(33);
	if (strcmp(code,".--.-.") == 0) printascii(64);
	if (strcmp(code,"---...") == 0) printascii(58);
	if (strcmp(code,"-....-") == 0) printascii(45);
	if (strcmp(code,"-..-.")  == 0) printascii(47);

	if (strcmp(code,"-.--.") 	== 0) printascii(40);
	if (strcmp(code,"-.--.-") 	== 0) printascii(41);
	if (strcmp(code,".-...") 	== 0) printascii(95);
	if (strcmp(code,"...-..-") 	== 0) printascii(36);
	if (strcmp(code,"...-.-") 	== 0) printascii(62);
	if (strcmp(code,".-.-.") 	== 0) printascii(60);
	if (strcmp(code,"...-.") 	== 0) printascii(126);
	//////////////////
	// The specials //
	//////////////////
//	if (strcmp(code,".-.-") 	== 0) printascii(3);
//	if (strcmp(code,"---.") 	== 0) printascii(4);
//	if (strcmp(code,".--.-") 	== 0) printascii(6);
}

void setup()
{
    // 各GPIOの有効化
    GPIO_port_enable(GPIO_port_D);
    GPIO_port_enable(GPIO_port_C);
    // 各ピンの設定
//    GPIO_pinMode(SW1_PIN, GPIO_pinMode_I_pullDown, GPIO_Speed_10MHz); /// GPIO_Speed_In? @see https://github.com/cnlohr/ch32v003fun/blob/master/examples/GPIO/GPIO.c#L55
//    GPIO_pinMode(SW2_PIN, GPIO_pinMode_I_pullDown, GPIO_Speed_10MHz);
//    GPIO_pinMode(SW3_PIN, GPIO_pinMode_I_pullDown, GPIO_Speed_10MHz);
    GPIO_pinMode(ADC_PIN, GPIO_pinMode_I_analog, GPIO_Speed_10MHz);
    
	GPIO_pinMode(LED_PIN, GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
	GPIO_digitalWrite(LED_PIN, low);

	GPIO_pinMode(UART_PIN, GPIO_pinMode_O_pushPullMux, GPIO_Speed_10MHz);
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO;

	// remap UART1  RXD : PD5, TXD : PD6  
	AFIO->PCFR1 &= ~GPIO_PartialRemap1_USART1;
	AFIO->PCFR1 &= ~GPIO_PartialRemap2_USART1;
	AFIO->PCFR1 |=  GPIO_PartialRemap2_USART1;

	ssd1306_spi_init();		// i2c Setup
	ssd1306_init();			// SSD1306 Setup
	ssd1306_setbuf(0);		// Clear Screen
	ssd1306_drawstr_sz(0, 32, title1, 1, fontsize_8x8);
//	ssd1306_drawstr_sz(0, 40, title2, 1, fontsize_8x8);
	ssd1306_refresh();
	GPIO_ADCinit();
	initGoertzel();
	sampling_period_us = 1000000 / SAMPLING_FREQUENCY;
	for (int i = 0; i < colums; i++){
    	line1[i] = 32;
		line2[i] = 32;
	}           
}

int main()
{
	float magnitude;

	SystemInit();			// ch32v003 sETUP
	setup();				// gpio Setup;
	Delay_Ms( 2000 );
//	printf("Start CW Decoder Version 1.0\n");
	ssd1306_setbuf(0);	// Clear Screen
	ssd1306_refresh();
	while(1) {
 	  	int16_t ave = 0;

		for (int i = 0; i < SAMPLES; i++) {
			uint32_t t = micros();
			morseData[i] = GPIO_analogRead(GPIO_Ain4_D3);
			ave += morseData[i];
			while ((micros() - t) < sampling_period_us);
		}
		ave = ave / SAMPLES;
		for (int i = 0; i < SAMPLES; i++) {
			morseData[i] -= ave;
		}
		// calc goertzel
		magnitude = goertzel(morseData, SAMPLES);
//		printf("%d\n", (int16_t)magnitude);
		/////////////////////////////////////////////////////////// 
		// here we will try to set the magnitude limit automatic //
		///////////////////////////////////////////////////////////
  		if (magnitude > magnitudelimit_low){
    		magnitudelimit = (magnitudelimit +((magnitude - magnitudelimit)/6));  /// moving average filter
  		}
  		if (magnitudelimit < magnitudelimit_low) {
			magnitudelimit = magnitudelimit_low;
		}

		////////////////////////////////////
		// now we check for the magnitude //
		////////////////////////////////////
		if(magnitude > magnitudelimit*0.6) {  // just to have some space up 
     		realstate = high; 
		} else {
    		realstate = low; 
		}

		///////////////////////////////////////////////////// 
		// here we clean up the state with a noise blanker //
		/////////////////////////////////////////////////////
		if (realstate != realstatebefore){
			laststarttime = millis();
		}
		if ((millis()-laststarttime)> nbtime) {
			if (realstate != filteredstate) {
				filteredstate = realstate;
			}
		}

		////////////////////////////////////////////////////////////
		// Then we do want to have some durations on high and low //
		////////////////////////////////////////////////////////////
		if (filteredstate != filteredstatebefore) {
			if (filteredstate == high) {
				starttimehigh = millis();
				lowduration = (millis() - startttimelow);
			}
			if (filteredstate == low) {
				startttimelow = millis();
				highduration = (millis() - starttimehigh);
				if (highduration < (2*hightimesavg) || hightimesavg == 0) {
					hightimesavg = (highduration+hightimesavg+hightimesavg) / 3;     // now we know avg dit time ( rolling 3 avg)
				}
				if (highduration > (5*hightimesavg) ) {
					hightimesavg = highduration+hightimesavg;     // if speed decrease fast ..
				}
			}
		}

		///////////////////////////////////////////////////////////////
		// now we will check which kind of baud we have - dit or dah //
		// and what kind of pause we do have 1 - 3 or 7 pause        //
		// we think that hightimeavg = 1 bit                         //
		///////////////////////////////////////////////////////////////
		if (filteredstate != filteredstatebefore){
			stop = low;
			if (filteredstate == low){  //// we did end a HIGH
				if (highduration < (hightimesavg*2) && highduration > (hightimesavg*0.6)){ /// 0.6 filter out false dits
					strcat(code,".");
					printf(".");
				}
				if (highduration > (hightimesavg*2) && highduration < (hightimesavg*6)){ 
					strcat(code,"-");
					printf("-");
					wpm = (wpm + (1200/((highduration)/3)))/2;  //// the most precise we can do ;o)
				}
			}
		}

		if (filteredstate == high){  //// we did end a LOW
	
			float lacktime = 1;
			if(wpm > 25)lacktime=1.0; ///  when high speeds we have to have a little more pause before new letter or new word 
			if(wpm > 30)lacktime=1.2;
			if(wpm > 35)lacktime=1.5;
		
			if (lowduration > (hightimesavg*(2*lacktime)) && lowduration < hightimesavg*(5*lacktime)){ // letter space
				docode();
				code[0] = '\0';
 				printf("/");
			}
			if (lowduration >= hightimesavg*(5*lacktime)){ // word space
				docode();
				code[0] = '\0';
				printascii(32);
				printf(" ");
				printf("\n");
			}
		}

		//////////////////////////////
		// write if no more letters //
		//////////////////////////////
		if ((millis() - startttimelow) > (highduration * 6) && stop == low){
			docode();
			code[0] = '\0';
			stop = high;
		}

		/////////////////////////////////////
		// we will turn on and off the LED //
		// and the speaker                 //
		/////////////////////////////////////
		if(filteredstate == high){ 
			GPIO_digitalWrite(LED_PIN, high);
		} else {
			GPIO_digitalWrite(LED_PIN, low);
		}
 
		//////////////////////////////////
		// the end of main loop clean up//
		/////////////////////////////////
		updateinfolinelcd();
		realstatebefore = realstate;
		lasthighduration = highduration;
		filteredstatebefore = filteredstate;
	}
}