/*
 * Hardware_Init.c
 *
 *  Created on: 2019��10��16��
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
	Get_SensorType();						//���ݲ����ȡ����������
	Sensor_Init();
	RS485_Init();
	Can_Init();
	LoraNode_Init_Mode(MODE_CMD);
	Led_Init();

    HAL_Delay(50);
}

//Ӳ������ -> ���������� ��ӳ���
void Get_SensorType(void)
{
	const uint8_t Type[] =
	{
		0,
		Temperature_Humidity_Type,			// 1  ������ʪ��
		Temperature_Humidity_Type,			// 2  ������ʪ��
		Temperature_Humidity_Type,			// 3  ������ʪ��
		Temperature_Humidity_Type,			// 4  ������ʪ��
		Outside_Temperature_Humidity_Type,	// 5  ������ʪ��
		Water_Temperature_Type,				// 6  ˮ��
		Negative_Pressure_Type,				// 7  ��ѹ
		Gas_CO2_Type,						// 8  ������̼
		Gas_NH3_Type,						// 9  ����
		Illumination_Type,					//10  ����
		WindowPosition_Type,				//11  С���Ƹ�λ��
		WindowPosition_Type,				//12  С���Ƹ�λ��
		Gas_O2_Type,						//13  ����
		WaterMeter_Type,                    //14  ˮ��
		Alarm_Type,                         //15  ������
		Temperature_Humidity_Type,			//16  ������ʪ��
		Temperature_Humidity_Type,			//17  ������ʪ��
		Temperature_Humidity_Type,			//18  ������ʪ��
		Temperature_Humidity_Type,			//19  ������ʪ��
		Temperature_Humidity_Type,			//20  ������ʪ��
		Temperature_Humidity_Type,			//21  ������ʪ��
		Temperature_Humidity_Type,			//22  ������ʪ��
		Temperature_Humidity_Type,			//23  ������ʪ��
		Temperature_Humidity_Type,			//24  ������ʪ��
		Temperature_Humidity_Type,			//25  ������ʪ��
		Temperature_Humidity_Type,			//26  ������ʪ��
		Water_Temperature_Type,				//27  ˮ��
		Negative_Pressure_Type,				//28  ��ѹ
		Gas_CO2_Type,						//29  ������̼
		Gas_NH3_Type,						//30  ����
		DeflectorPosition_Type,             //31 ������λ��
		DeflectorPosition_Type,             //32 ������λ��
		DeflectorPosition_Type,             //33 ������λ��
		DeflectorPosition_Type,             //34 ������λ��
		DeflectorPosition_Type,             //35 ������λ��
		DeflectorPosition_Type,             //36 ������λ��
		DeflectorPosition_Type,             //37 ������λ��
		DeflectorPosition_Type,             //38 ������λ��
		DeflectorPosition_Type,             //39 ������λ��
		DeflectorPosition_Type,             //40 ������λ��
		DeflectorPosition_Type,             //41 ������λ��
		DeflectorPosition_Type,             //42 ������λ��

	};

	Sensor_Type = Type[Sensor_ID];
}

//��ȡ��������Ӳ������
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
	RS485Uart_RX_TX_Switch(RECEIVE);                                 //485���մ򿪣����ͽ�ֹ
}


void Can_Init(void)
{
	Get_CanSensor_FrameID(Sensor_ID);
	CAN_Config_Init();
}




