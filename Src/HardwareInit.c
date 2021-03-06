/*
 * Hardware_Init.c
 *
 *  Created on: 2019年10月16日
 *      Author: Breuning
 */

#include "HardwareInit.h"
#include "LoraNode.h"
#include "SHT30.h"
#include "BH1750.h"
#include "Gas.h"
#include "RS485Analysis.h"
#include "CanAnalysis.h"
#include "Led.h"
#include "can.h"

uint8_t Sensor_ID;
uint8_t Sensor_Type;

void Hardware_Init(void)
{
	Get_SensorID();
	Get_SensorType();						//根据拨码获取传感器类型
	Sensor_Init();
	RS485_Init();
	Can_Init();
	LoraNode_Init_Mode(MODE_CMD);
	Led_Init();

    HAL_Delay(50);
}

//硬件拨码 -> 传感器类型 的映射表
void Get_SensorType(void)
{
	const uint8_t Type[] =
	{
		0,
		Temperature_Humidity_Type,			// 1  室内温湿度
		Temperature_Humidity_Type,			// 2  室内温湿度
		Temperature_Humidity_Type,			// 3  室内温湿度
		Temperature_Humidity_Type,			// 4  室内温湿度
		Outside_Temperature_Humidity_Type,	// 5  室外温湿度
		Water_Temperature_Type,				// 6  水温
		Negative_Pressure_Type,				// 7  负压
		Gas_CO2_Type,						// 8  二氧化碳
		Gas_NH3_Type,						// 9  氨气
		Illumination_Type,					//10  光照
		WindowPosition_Type,				//11  小窗推杆位移
		WindowPosition_Type,				//12  小窗推杆位移
		Gas_O2_Type,						//13  氧气
		WaterMeter_Type,                    //14  水表
		Alarm_Type,                         //15  报警器
		Temperature_Humidity_Type,			//16  室内温湿度
		Temperature_Humidity_Type,			//17  室内温湿度
		Temperature_Humidity_Type,			//18  室内温湿度
		Temperature_Humidity_Type,			//19  室内温湿度
		Temperature_Humidity_Type,			//20  室内温湿度
		Temperature_Humidity_Type,			//21  室内温湿度
		Temperature_Humidity_Type,			//22  室内温湿度
		Temperature_Humidity_Type,			//23  室内温湿度
		Temperature_Humidity_Type,			//24  室内温湿度
		Temperature_Humidity_Type,			//25  室内温湿度
		Temperature_Humidity_Type,			//26  室内温湿度
		Water_Temperature_Type,				//27  水温
		Negative_Pressure_Type,				//28  负压
		Gas_CO2_Type,						//29  二氧化碳
		Gas_NH3_Type,						//30  氨气
		DeflectorPosition_Type,             //31 导流板位移
		DeflectorPosition_Type,             //32 导流板位移
		DeflectorPosition_Type,             //33 导流板位移
		DeflectorPosition_Type,             //34 导流板位移
		DeflectorPosition_Type,             //35 导流板位移
		DeflectorPosition_Type,             //36 导流板位移
		DeflectorPosition_Type,             //37 导流板位移
		DeflectorPosition_Type,             //38 导流板位移
		DeflectorPosition_Type,             //39 导流板位移
		DeflectorPosition_Type,             //40 导流板位移
		DeflectorPosition_Type,             //41 导流板位移
		DeflectorPosition_Type,             //42 导流板位移

	};

	Sensor_Type = Type[Sensor_ID];
}

//获取传感器的硬件拨码
void Get_SensorID(void)
{
	Sensor_ID = 1 + (SW1 * 1  + SW2 * 2  + SW3 * 4  + SW4 * 8 + SW5 * 16);
}

void Sensor_Init(void)
{
	switch (Sensor_Type)
	{
		case Outside_Temperature_Humidity_Type:
			SHT30_Init();
			break;
		case Temperature_Humidity_Type:
			SHT30_Init();
			break;
		case Gas_CO2_Type:
			MHZ14CO2_Init();
			break;
		case Gas_NH3_Type:
			ZE03GAS_Init();
			break;
		case Gas_O2_Type:
			ZE03GAS_Init();
			break;
		case Illumination_Type:
			BH1750_Init();
			break;
		default:
			break;
	}
}

void RS485_Init(void)
{
	RS485Uart_RX_TX_Switch(RECEIVE);                                 //485接收打开，发送截止
}


void Can_Init(void)
{
	Get_CanSensor_FrameID(Sensor_ID);
	CAN_Config_Init();
}




