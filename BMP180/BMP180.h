/**
  ******************************************************************************
  BMP180 LIBRARY for STM32 using I2C
  Author:   Cuong Nguyen
  Updated:  1/11/2025
  ******************************************************************************
*/

#ifndef _BMP180_H_
#define _BMP180_H_


#include "stm32f1xx_hal.h"

void BMP180_Start (void);

float BMP180_GetTemp (void);

float BMP180_GetPress (int oss);

float BMP180_GetAlt (int oss);

#endif /* INC_BMP180_H_ */
