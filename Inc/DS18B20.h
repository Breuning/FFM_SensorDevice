/*
 * DS18B20.h
 *
 *  Created on: 2019Äê8ÔÂ8ÈÕ
 *      Author: Breuning
 */

#ifndef DS18B20_H_
#define DS18B20_H_


#include "main.h"

#define   SIZE   5
#define   T_MUTATION_MAXIMUN   0X32

#define   DS18B20_DQ_HIGH()   HAL_GPIO_WritePin(DS18B20_DQ_GPIO_Port, DS18B20_DQ_Pin, GPIO_PIN_SET)
#define   DS18B20_DQ_LOW()    HAL_GPIO_WritePin(DS18B20_DQ_GPIO_Port, DS18B20_DQ_Pin, GPIO_PIN_RESET)

#define   DS18B20_DQ_IN_Read  HAL_GPIO_ReadPin(DS18B20_DQ_GPIO_Port, DS18B20_DQ_Pin)

uint8_t GetTemFrom18B20(void);

#endif /* DS18B20_H_ */
