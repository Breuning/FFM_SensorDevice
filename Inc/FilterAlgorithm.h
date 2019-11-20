/*
 * FilterAlgorithm.h
 *
 *  Created on: 2019��8��3��
 *      Author: Breuning
 */

#ifndef FILTERALGORITHM_H_
#define FILTERALGORITHM_H_

#include "main.h"
#define SAMPLE_NUMBER	 10 			        //�����õ����ݸ���


typedef struct
{
	uint16_t SampleCount;
	uint16_t SampleBuff[SAMPLE_NUMBER]; 	 	//����洢���ݵ�����
}FILTER;

extern FILTER filter[10];
uint16_t FiltetAlgorithmforSensors(uint16_t tem,FILTER *Filter);


#endif /* FILTERALGORITHM_H_ */
