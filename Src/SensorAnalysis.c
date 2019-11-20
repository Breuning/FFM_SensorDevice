/*
 * SensorAnalysis.c
 *
 *  Created on: 2019��7��25��
 *      Author: Breuning
 */

#include "string.h"
#include "HardwareInit.h"
#include "SensorAnalysis.h"
#include "adc.h"
#include "usart.h"
#include "SHT30.h"
#include "BH1750.h"
#include "Gas.h"
#include "FilterAlgorithm.h"
#include "tim.h"

BOOL SensorReadTimerFlag = FALSE;
Sensor_Data_Struct Sensor_Data;

uint32_t Tick_GetSensorData = 0;                  //�����ж�̽ͷ��ȡ�������Ƿ�ܾ�δ����

char SensorData_Buff[30];

float SensorData_Float[] =
{
	((float)Sensor_Data.Temperature)/10.0,
	((float)Sensor_Data.Humidity)/10.0,
	(float)Sensor_Data.WaterTemperature,
	(float)Sensor_Data.NegativePressure,
	(float)Sensor_Data.CO2_Data,
	(float)Sensor_Data.NH3_Data,
	(float)Sensor_Data.Illumination,
	(float)Sensor_Data.WindDerection,
	(float)Sensor_Data.WaterMeter

}


void SensorAnalysis(void)
{

	if(SensorReadTimerFlag == TRUE)              //ͨ��TIM5����Ϊÿ5���ȡһ�δ�������ֵ
	{
		Get_SensorData();

/**********������, ����ȡ�Ĵ�������ֵ�����SensorData_Buff������,���������ڵ�����ͨ��USART2������PC**********/

		memset(SensorData_Buff, 0 , sizeof(SensorData_Buff));

//		sprintf(SensorData_Buff, "Tem:%d Hum:%d \n", Sensor_Data.Temperature, Sensor_Data.Humidity );
//		sprintf(SensorData_Buff, "Illu: %d \n", Sensor_Data.Illumination);
		sprintf(SensorData_Buff, "NH3: %d \n", Sensor_Data.NH3_Data);
//		sprintf(SensorData_Buff, "CO2: %d \r", Sensor_Data.CO2_Data);

		//�������ڵ�����ͨ��USART2������PC
		HAL_UART_Transmit(&huart2, (uint8_t *)SensorData_Buff, strlen(SensorData_Buff), 100);

/********************************************************************************************************/
		SensorReadTimerFlag = FALSE;
	}

}

void Get_SensorData(void)
{
	switch (Sensor_Type)
	{

		case Temperature_Humidity_Type:

		case Outside_Temperature_Humidity_Type:
			GetValidDateFromSHT30();
			FiltetAlgorithmforSensors(Sensor_Data.Temperature_u, &filter[0]);
			TemperatureData_NegativeValueJudge();
			FiltetAlgorithmforSensors(Sensor_Data.Humidity,    &filter[1]);
			break;
//		case Water_Temperature_Type:
//			Sensor_Data.Water_Temperature = DS18B20_Get_Temp();
//			FiltetAlgorithmforSensors(Sensor_Data.Water_Temperature, &filter[2]);
			break;
		case Pressure_Type:
			Sensor_Data.NegativePressure = Get_Pressure();           //��ѹ���Ҳ����˲�
			FiltetAlgorithmforSensors(Sensor_Data.NegativePressure, &filter[3]);
			break;
		case Gas_CO2_Type:
			GetValidDateFromMHZ14CO2();
			FiltetAlgorithmforSensors(Sensor_Data.CO2_Data, &filter[4]);
			break;
		case Gas_NH3_Type:
			GetValidDateFromZE03NH3();
			FiltetAlgorithmforSensors(Sensor_Data.NH3_Data, &filter[5]);
			break;
		case Illumination_Type:
			GetValidDateFromBH1750();
			FiltetAlgorithmforSensors(Sensor_Data.Illumination, &filter[6]);
			break;
		case WindowPosition_Type:
			Sensor_Data.WindowPosition = Get_WindowPos();
			FiltetAlgorithmforSensors(Sensor_Data.WindowPosition, &filter[7]);
			break;
		default:
			break;
	}
}








