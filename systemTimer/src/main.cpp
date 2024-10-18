//
//	micFFT.cpp 
//
//  2024.07.31 New Create
//
#include "ch32v003fun.h"
volatile uint32_t millis_count = 0;
volatile uint32_t micros_count = 0;

void SysTick_Init(void)
{
    SysTick->LOAD = (SystemCoreClock / 1000) - 1; // 1msごとのリロード値を設定
    SysTick->VAL = 0; // 現在値をクリア
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; // SysTickを有効化、割り込みを有効化、クロックソースを設定
}

void SysTick_Handler(void)
{
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
    systick_val = SysTick->VAL;

    // 割り込みが発生する直前の状態を考慮してカウンタ値を調整
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0)
    {
        micros_tmp += 1000;
        systick_val = SysTick->VAL;
    }

    // 割り込みを有効にしてクリティカルセクションを終了
    __enable_irq();

    return micros_tmp + (SystemCoreClock / 1000000 - systick_val / (SystemCoreClock / 1000000));
}


int main()
{
	char buf[16];

	SystemInit();			// ch32v003 sETUP
    SysTick_Init(); // SysTickの初期化

    while (1)
    {
        uint32_t current_millis = millis();
        uint32_t current_micros = micros();
		printf("%lu, %lu", current_millis, current_micros);

        // ここにmillis()およびmicros()を使用したい処理を記述します
    }
}

