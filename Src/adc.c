/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ADC1 GPIO Configuration    
    PB1     ------> ADC1_IN9 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    /* ADC1 Init */
    hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_NORMAL;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();
  
    /**ADC1 GPIO Configuration    
    PB1     ------> ADC1_IN9 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

//--------------------------------------------------------------------------------------------
//	 @function:获取ADC采集数据
//--------------------------------------------------------------------------------------------
uint16_t ADC_ReadData(void)
{
    static uint16_t ad_data;

    HAL_ADC_Start(&hadc1);						 //软件开始ADC转换
    HAL_ADC_PollForConversion(&hadc1,100);	     //等待ADC转换完成，超时时间为100ms
    ad_data=(uint16_t)HAL_ADC_GetValue(&hadc1);  //调用HAL_ADC_GetValue获取采样值赋给ad_data
    HAL_ADC_Stop(&hadc1);						 //软件停止ADC转换
    HAL_Delay(5);
    return ad_data;
}

//--------------------------------------------------------------------------------------------
//	 @function:获取ADC采集数据平均值
//--------------------------------------------------------------------------------------------
uint16_t ADC_Average(uint8_t times)
{
	uint32_t sum=0,i,adc_average;
    for(i=0;i<times;i++)
    {
    	sum+=ADC_ReadData();
    }
    adc_average=sum/times;
    return adc_average;
}

//--------------------------------------------------------------------------------------------
//	 @function:获取负压探头数值
//--------------------------------------------------------------------------------------------
uint16_t Get_Pressure(void)
{
    double temp_data;
    double temp_data1;
    temp_data = (double) ADC_Average(10);
    temp_data1 = temp_data*(double)3.3;
    temp_data1 = temp_data1/4095;
    if (temp_data1 >0.64)
    {
        temp_data1 -= 0.64;
        temp_data1 = temp_data1 / 0.0256;
        //temp_data = ((3.3*temp_data)/4095 -0.64);
        //(3.3/4096*X - 0.64)*0.00256
		//temp_data1 = filter_1(temp_data1,Sensor_Data_Buff.Negative_Pressure,50);
        return temp_data1;
    }
    else
    {
        return 0;
    }
}

//--------------------------------------------------------------------------------------------
//	 @function:获取小窗位移探头数值
//--------------------------------------------------------------------------------------------
uint16_t Get_WindowPos(void)
{
	float l = 0;
	float v = 0;
	v =(float) (ADC_Average(10));      		 //10次数据的平均值
	v = v * 3.3 / 4096;                      //采集到的实际电压值
	l = (((v / 160) * 1000 )- 4) * 62.5;     //电压值转换为相应的位移值
	return  (uint16_t)(l);
}




/*
static float calPostionPercent(uint32_t origin,ref_stype_t type)
{
	hcfg_t *hcfg = CFG_get();

	float ret = 0;
	double r = 0;

	if(type == ref_SENSOR_POSITION1)
	{
		//计算电压
		ret =  ((float)origin / 4096.0 * 3.3);
		//计算电阻
		r = ntc_PUSH_UP_RESISTOR_KR  / (3.3/ret - 1);
		//计算电阻增量
		r -= hcfg->sw.align.Base1;
		//计算电阻变化的百分比
		ret = r / (hcfg->sw.align.full1 - hcfg->sw.align.Base1) * 100.0;
	}
	else if (type == ref_SENSOR_POSITION2)
	{
		ret =  ((float)origin / 4096.0 * 3.3);
		r = ntc_PUSH_UP_RESISTOR_KR  / (3.3/ret - 1);
		r -= hcfg->sw.align.Base2;
		ret = r / (hcfg->sw.align.full2 - hcfg->sw.align.Base2) * 100.0;
	}

	if(ret < 0)
	{
		ret =0;
	}
	if(ret > 100)
	{
		ret = 100;
	}

	if(hcfg->sw.align.revDir)
	{
		return ret;
	}
	else
	{
		return  100.0 - ret;;
	}
}

#define ntc_PUSH_UP_RESISTOR_KR (float)10.0
*/


/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
