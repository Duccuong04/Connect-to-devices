#ifndef MAIN_H
#define MAIN_H


// include library
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_exti.h"             // Keil::Device:StdPeriph Drivers:EXTI
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

// define Pin SPI1 for NRF24L01
#define NRF24_CE 			GPIO_Pin_3       // Chip Enable Active, RX/TX?
#define NRF24_CSN 		GPIO_Pin_4
#define NRF24_SCK			GPIO_Pin_5
#define NRF24_MISO 		GPIO_Pin_6
#define NRF24_MOSI 		GPIO_Pin_7
#define SPI1_GPIO 		GPIOA            // use SPI1, GPIOA, Pin A4 - A7
#define SPI1_RCC  		RCC_APB2Periph_SPI1

void RCC_Config(void);
void GPIO_Config(void);
void SPI_Config(void);
void TIM_Config(void);
extern volatile uint32_t msTicks;
void SysTick_Handler(void);
void delay_ms (uint32_t ms);
void delay_us(uint32_t time);
#endif