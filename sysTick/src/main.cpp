//
//	micFFT.cpp 
//
//  2024.07.31 New Create
//
#include "ch32v003fun.h"
#include <stdio.h>

volatile uint32_t systick_cnt;

volatile uint32_t millis_count = 0;
volatile uint32_t micros_count = 0;

void systick_init(void)
{
	/* disable default SysTick behavior */
	SysTick->CTLR = 0;
	
	/* enable the SysTick IRQ */
	NVIC_EnableIRQ(SysTicK_IRQn);
	
	/* Set the tick interval to 1ms for normal op */
	SysTick->CMP = (FUNCONF_SYSTEM_CORE_CLOCK/1000)-1;
	
	/* Start at zero */
	SysTick->CNT = 0;
	systick_cnt = 0;
	
	/* Enable SysTick counter, IRQ, HCLK/1 */
	SysTick->CTLR = SYSTICK_CTLR_STE | SYSTICK_CTLR_STIE |
					SYSTICK_CTLR_STCLK;
}

/*
 * SysTick ISR just counts ticks
 * note - the __attribute__((interrupt)) syntax is crucial!
 */
void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
	// move the compare further ahead in time.
	// as a warning, if more than this length of time
	// passes before triggering, you may miss your
	// interrupt.
	SysTick->CMP += (FUNCONF_SYSTEM_CORE_CLOCK/1000);

	/* clear IRQ */
	SysTick->SR = 0;

	/* update counter */
	systick_cnt++;
    millis_count++;
    micros_count += 1000;
}

uint32_t millis(void)
{
    return millis_count;
}

uint32_t micros(void)
{
    uint32_t micros_tmp;
    uint32_t systick_val;

    // 割り込みを無効にしてクリティカルセクションを開始
    __disable_irq();
    
    micros_tmp = micros_count;
    systick_val = SysTick->CNT;

    // 割り込みが発生する直前の状態を考慮してカウンタ値を調整
    if ((SysTick->CTLR & SysTick_CTRL_COUNTFLAG_Msk) != 0)
    {
        micros_tmp += 1000;
        systick_val = SysTick->CNT;
    }

    // 割り込みを有効にしてクリティカルセクションを終了
    __enable_irq();

    return micros_tmp + (SystemCoreClock / 1000000 - systick_val / (SystemCoreClock / 1000000));
}


int main()
{
	char buf[16];
	uint32_t count = 0;

	SystemInit();			// ch32v003 sETUP
    Delay_Ms(100);
	printf("\r\r\n\nsystick_irq example\n\r");
	printf("initializing systick...");
    SysTick_init(); // SysTickの初期化
	printf("done.\n\r");

    while (1)
    {
        uint32_t current_millis = millis();
        uint32_t current_micros = micros();
//		printf("%lu, %lu", current_millis, current_micros);
		printf( "Print #: %lu / Milliseconds: %lu / CNT: %lu\n\r", count++, systick_cnt, SysTick->CNT );
    }
}

