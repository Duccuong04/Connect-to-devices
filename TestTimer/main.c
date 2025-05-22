#include "stm32f10x.h"                  // Device header

// provide clock, khoi SPI se tu dong noi voi GPIO
void RCC_Config()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void TIM_Config()
{
	TIM_TimeBaseInitTypeDef TIM_InitStruct;

	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 72MHzz
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_Period = 0xFFFF;
	TIM_InitStruct.TIM_Prescaler = 7200 - 1;
	
	TIM_TimeBaseInit (TIM2, &TIM_InitStruct);
	TIM_Cmd(TIM2, ENABLE);
}

void GPIO_Config()
{
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &gpio);
}

void delay_us(uint32_t time)
{
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < time)
	{}		
}

int main(void)
{
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	while(1)
	{
		GPIOC->ODR ^= (1 << 13);
		delay_us(10000);
	}
}
