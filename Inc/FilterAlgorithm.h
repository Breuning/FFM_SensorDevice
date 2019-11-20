/*
 * FilterAlgorithm.h
 *
 *  Created on: 2019年8月3日
 *      Author: Breuning
 */

#ifndef FILTERALGORITHM_H_
#define FILTERALGORITHM_H_

#include "main.h"
#define SAMPLE_NUMBER	 10 			        //定义获得的数据个数


typedef struct
{
	uint16_t SampleCount;
	uint16_t SampleBuff[SAMPLE_NUMBER]; 	 	//定义存储数据的数组
}FILTER;

extern FILTER filter[10];
uint16_t FiltetAlgorithmforSensors(uint16_t tem,FILTER *Filter);


#endif /* FILTERALGORITHM_H_ */
