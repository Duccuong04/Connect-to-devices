#include "stm32f10x.h"                  // Device header
#include "NRF24.h"
#include "main.h"

uint8_t TxAdrress[]={0x00, 0xDD, 0xCC, 0xBB, 0xAA};
uint8_t Tx_data[32] = "Hello World\n";
void RCC_Config(){
	RCC_APB2PeriphClockCmd(SPI1_RCC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = NRF24_CSN | NRF24_MISO | NRF24_MOSI | NRF24_SCK;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // afternation push-pull, connect to SPI block
	
	GPIO_Init(SPI1_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = NRF24_CE;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

// SPI configuration
void SPI_Config(){
	
	SPI_InitTypeDef SPI_InitStructure;
	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7; // check sum CRC bac 7
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;// kiem soat chan NSS/CS bang software
	
	SPI_Init(SPI1, &SPI_InitStructure); // save to register
	SPI_Cmd(SPI1, ENABLE);	
}

void TIM_Config(void)
{
    TIM_TimeBaseInitTypeDef tim;
    tim.TIM_Prescaler = 72 - 1; // 1MHz -> 1 tick = 1us
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = 0xFFFF;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM2, &tim);
    TIM_Cmd(TIM2, ENABLE);
}

void delay_us(uint32_t time){
	TIM_SetCounter(TIM2, 0);
	while (TIM_GetCounter(TIM2) < time); 
}


volatile uint32_t msTicks = 0; // ngat SystemTick

void SysTick_Handler(void)
{
    msTicks++;  // Tang m?i 1ms
}

void delay_ms (uint32_t ms)
{
	uint32_t start = msTicks;
	while((msTicks - start) < ms);
}

uint8_t status;
int main(void)
{
	SystemInit();
	SysTick_Config(SystemCoreClock / 1000);
	RCC_Config();
	GPIO_Config();
	SPI_Config();
	TIM_Config();
	NRF24_Init ();
	NRF24_TxMode(TxAdrress, 10);
	
	while(1)
	{
		status = nrf24_ReadReg(0x07); // STATUS register

		if(NRF24_Transmit(Tx_data) == 1)
		{
			GPIOC->ODR ^= (1 << 13);
		}
		delay_ms(800);

	}

}