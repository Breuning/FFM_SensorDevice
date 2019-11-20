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

uint16_t DS18B20_Get_Temp(void);
static void DS18B20_Rst(void);
static uint8_t DS18B20_Check(void);
static void DS18B20_Write_Byte(uint8_t dat);
static uint8_t DS18B20_Read_Byte(void);
static uint8_t DS18B20_Read_Bit(void);
static uint8_t B20DataCheckCRC(uint8_t *p, uint8_t len);
static void DS18B20_DQ_IN(void);
static void DS18B20_DQ_OUT(void);
static uint8_t FilterAlgorithm(short* tem);

#endif /* DS18B20_H_ */
