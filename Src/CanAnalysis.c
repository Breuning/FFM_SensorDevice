/*
 * CanAnalysis.c
 *
 *  Created on: 2019��11��20��
 *      Author: Breuning
 */
#include <string.h>
#include "main.h"
#include "can.h"
#include "CanAnalysis.h"
#include "HardwareInit.h"
#include "SensorAnalysis.h"

static void Can_TxSensorData(FloatSensorDataType type);

uint32_t CanSensor_FrameID;
uint32_t CanSensor_FrameID_Hum;

CanSensor_t  CanSensor_Type;
CanSensor_t  CanSensor_Type_Hum = 0;

BOOL CanDataSendTimerFlag = FALSE;
BOOL CanDataSendTimerFlag_Intime = FALSE;

void CanAnalysis(void)
{
	uint32_t Tick_CanTx;
	uint32_t SensorTimeout = 1000*10;

	Tick_CanTx = HAL_GetTick();

	if(CanDataSendTimerFlag_Intime == TRUE)
	{
		if(Tick_CanTx - Tick_GetSensorData < SensorTimeout)
		{
			switch(CanSensor_Type)
			{
				case CanSensor_TYPE_PRESSURE:
					Can_TxSensorData(NegativePressure);
					break;
				case CanSensor_TYPE_WINDOW_POSITION:
					Can_TxSensorData(WindowPosition);
					break;
				default:
					break;
			}
		}
		CanDataSendTimerFlag_Intime = FALSE;
	}


	if(CanDataSendTimerFlag == TRUE)
	{
		if(Tick_CanTx - Tick_GetSensorData < SensorTimeout)
		{
			switch(CanSensor_Type)
			{
				case CanSensor_TYPE_TEMPERATURE:
					Can_TxSensorData(Temperature);
					break;
				case CanSensor_TYPE_WATER_TEMPERATURE:
					Can_TxSensorData(WaterTemperature);
					break;
				case CanSensor_TYPE_GAS_CO2:
					Can_TxSensorData(CO2_Data);
					break;
				case CanSensor_TYPE_GAS_NH3:
					Can_TxSensorData(NH3_Data);
					break;
				case CanSensor_TYPE_ILLUMINATION:
					Can_TxSensorData(Illumination);
					break;
				case CanSensor_TYPE_WATER_METER:
					Can_TxSensorData(WaterMeter);
					break;
				default:
					break;
			}

			HAL_Delay(100);

			if(CanSensor_Type_Hum == CanSensor_TYPE_HUMIDITY)
			{
				Can_TxSensorData(Humidity);
			}

		}

		CanDataSendTimerFlag = FALSE;
	}
}

static void Can_TxSensorData(FloatSensorDataType type)
{
	FloatDataToArr_t CanSensorData;

	CanSensorData.FloatData = GetFloatSensorData(type);
	if(type == Humidity)
	{
		Can_TxMessage(ExtendedFrame, CanSensor_FrameID_Hum, 4, CanSensorData.TransformedDataBuf);
	}
	else
	{
		Can_TxMessage(ExtendedFrame, CanSensor_FrameID, 4, CanSensorData.TransformedDataBuf);
	}

	HAL_GPIO_TogglePin(LED2_MSGTX_GPIO_Port, LED2_MSGTX_Pin);

	memset(CanSensorData.TransformedDataBuf, 0 , sizeof(CanSensorData.TransformedDataBuf));
}

uint16_t Get_CanSensor_FrameID(uint8_t sensorid)
{
	CanIdResolve_t CanIdResolve;
	CanIdConstruction_t CanIdConstruction;

	CanIdConstruction.Aid     = sensorid;
	CanIdConstruction.Tid     = Get_CanSensor_Type(sensorid);
	CanIdConstruction.Bid     = CanBoard_TYPE_SENSOR;
	CanIdConstruction.FreeBit = 0xFFF;

	CanIdResolve.ResolveId = CanIdConstruction;
	CanSensor_FrameID = CanIdResolve.OriginId;


	if(CanSensor_Type_Hum == CanSensor_TYPE_HUMIDITY)
	{
		CanIdConstruction.Tid = CanSensor_TYPE_HUMIDITY;

		CanIdResolve.ResolveId = CanIdConstruction;
		CanSensor_FrameID_Hum = CanIdResolve.OriginId;
	}

	return CanSensor_FrameID;
}

CanSensor_t Get_CanSensor_Type(uint8_t sensorid)
{
	const CanSensor_t CanSensorType[] =
	{
		0,
		CanSensor_TYPE_TEMPERATURE_HUMIDITY,
		CanSensor_TYPE_TEMPERATURE_HUMIDITY,
		CanSensor_TYPE_TEMPERATURE_HUMIDITY,
		CanSensor_TYPE_TEMPERATURE_HUMIDITY,
		CanSensor_TYPE_TEMPERATURE_HUMIDITY,
		CanSensor_TYPE_WATER_TEMPERATURE,
		CanSensor_TYPE_PRESSURE,
		CanSensor_TYPE_GAS_CO2,
		CanSensor_TYPE_GAS_NH3,
		CanSensor_TYPE_ILLUMINATION,
		CanSensor_TYPE_WINDOW_POSITION,
		CanSensor_TYPE_WINDOW_POSITION,
	};

	CanSensor_Type = CanSensorType[sensorid];

	if(CanSensor_Type == CanSensor_TYPE_TEMPERATURE_HUMIDITY)
	{
		CanSensor_Type = CanSensor_TYPE_TEMPERATURE;
		CanSensor_Type_Hum = CanSensor_TYPE_HUMIDITY;
	}

	return CanSensor_Type;
}

