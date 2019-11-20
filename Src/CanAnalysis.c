/*
 * CanAnalysis.c
 *
 *  Created on: 2019Äê8ÔÂ13ÈÕ
 *      Author: Breuning
 */
#include <string.h>
#include "main.h"
#include "can.h"
#include "CanAnalysis.h"
#include "HardwareInit.h"
#include "SensorAnalysis.h"


uint32_t CanSensor_FrameID;
CanSensor_t  CanSensor_Type;

static uint32_t Get_CanSensor_FrameID(uint8_t sensorid);
static CanSensor_t Get_CanSensor_Type(uint8_t sensorid);

void CanAnalysis(void)
{
	uint32_t Tick_CanTx;
	uint32_t SersorTimeout = 1000*10;
	Tick_CanTx = HAL_GetTick();

	FloatDataToArr_t CanSensorData;


	if(Tick_CanTx - Tick_GetSensorData < SersorTimeout)
	{

		Get_CanSensor_FrameID(Sensor_ID);

		switch(Sensor_ID)
		{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				CanSensorData.FloatData = SensorData_Float[Temperature];
				Can_TxMessage(StandardFrame, CanSensor_FrameID, 4, CanSensorData.TransformedDataBuf);
				memset(CanSensorData.TransformedDataBuf, 0 , sizeof(CanSensorData.TransformedDataBuf));

				CanSensorData.FloatData = SensorData_Float[Humidity];
				Can_TxMessage(StandardFrame, CanSensor_FrameID, 4, CanSensorData.TransformedDataBuf);
				memset(CanSensorData.TransformedDataBuf, 0 , sizeof(CanSensorData.TransformedDataBuf));

				break;
			case 8:
				CanSensorData.FloatData = SensorData_Float[CO2_Data];
				Can_TxMessage(StandardFrame, CanSensor_FrameID, 4, CanSensorData.TransformedDataBuf);
				memset(CanSensorData.TransformedDataBuf, 0 , sizeof(CanSensorData.TransformedDataBuf));
				break;
			case 9:
				CanSensorData.FloatData = SensorData_Float[NH3_Data];
				Can_TxMessage(StandardFrame, CanSensor_FrameID, 4, CanSensorData.TransformedDataBuf);
				memset(CanSensorData.TransformedDataBuf, 0 , sizeof(CanSensorData.TransformedDataBuf));
				break;

		}
	}
}

static uint32_t Get_CanSensor_FrameID(uint8_t sensorid)
{
	StdIdResolve_t StdIdResolve;

	StdIdResolve.Resolve.CanBoardType  = CanBoard_TYPE_SENSOR;
	StdIdResolve.Resolve.CanSensorType = Get_CanSensor_Type(sensorid);
	StdIdResolve.Resolve.CanSensorNum  = sensorid;

	CanSensor_FrameID = StdIdResolve.OriginId;
	return CanSensor_FrameID;
}

static CanSensor_t Get_CanSensor_Type(uint8_t sensorid)
{
	const CanSensor_t CanSensorType[] =
	{
		0,
		CanSensor_TYPE_TEMPERATURE_HUMIDITY,
		CanSensor_TYPE_TEMPERATURE_HUMIDITY,
		CanSensor_TYPE_TEMPERATURE_HUMIDITY,
		CanSensor_TYPE_TEMPERATURE_HUMIDITY,
		CanSensor_TYPE_OUT_TEMPERATURE_HUMIDITY,
		CanSensor_TYPE_WATER_METER,
		CanSensor_TYPE_PRESSURE,
		CanSensor_TYPE_GAS_CO2,
		CanSensor_TYPE_GAS_NH3,
		CanSensor_TYPE_ILLUMINATION,
		CanSensor_TYPE_WINDOW_POSITION,
	};

	CanSensor_Type = CanSensorType[sensorid];
	return CanSensor_Type;
}

