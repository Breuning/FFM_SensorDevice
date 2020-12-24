/*
 * WaterMeter.h
 *
 *  Created on: 2020��9��16��
 *      Author: Breuning
 */

#ifndef WATERMETER_H_
#define WATERMETER_H_

#include "stm32f1xx_hal.h"


void GetValidDataFromWaterMet(void);
unsigned BCDtoDecimal (unsigned bVal);
uint16_t CRC16(uint8_t *puchMsg, uint8_t usDataLen);




#endif /* WATERMETER_H_ */
