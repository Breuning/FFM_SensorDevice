/*
 * RS485Analysis.c
 *
 *  Created on: 2019年10月18日
 *      Author: Breuning
 */

#include "string.h"
#include "HardwareInit.h"
#include "usart.h"
#include "RS485Analysis.h"
#include "SensorAnalysis.h"
#include "Led.h"

uint32_t Tick_RS485Ack;

uint8_t Master51AckBuf[8]  = {0};
uint8_t Master52AckBuf[15] = {0};


uint8_t ModbusAckBuf[9] = {0};

void RS485Analysis(void)
{
	if(RS485Uart_RX.receive_flag == 1)
	{

		if((RS485Uart_RX.RX_Buf[0] == 0xFF) && (RS485Uart_RX.RX_Buf[1] == 0xEE))             //环控自定义传输协议
		{
			MasterData_Analysis();
		}

		else if((RS485Uart_RX.RX_Buf[0] != 0xFF) && (RS485Uart_RX.RX_Buf[1] == 0x03))        //Modbus标准传输协议，PLC传输过来
		{
			ModbusData_Analysis();
		}

		RS485Uart_RX.receive_flag = 0;
		memset(RS485Uart_RX.RX_Buf, 0 , sizeof(RS485Uart_RX.RX_Buf));

		HAL_UART_Receive_DMA(&huart4, RS485Uart_RX.RX_Buf, RECEIVELEN);
	}
}


/****************************************************养殖大师环控自定义传输协议**************************************************************/

void MasterData_Analysis(void)
{
	uint16_t Master_CrcVal;
    uint16_t Master_CheckCrc;


	if(RS485Uart_RX.rx_len < RS485Uart_RX.RX_Buf[MASTER_DATA_LEN_NUM])
		return;

	Master_CrcVal   = (RS485Uart_RX.RX_Buf[MASTER_CRCVAL_HIGH_NUM]<<8) | RS485Uart_RX.RX_Buf[MASTER_CRCVAL_LOW_NUM];  //数据包中的CRC校验值
	Master_CheckCrc = CRC16(RS485Uart_RX.RX_Buf, RS485Uart_RX.RX_Buf[MASTER_DATA_LEN_NUM]-2);
	if(Master_CrcVal != Master_CheckCrc)
		return;

	if(RS485Uart_RX.RX_Buf[MASTER_SENSOR_ID_NUM] == Sensor_ID)
	{
		Tick_RS485Ack = HAL_GetTick();

		if(Tick_RS485Ack - Tick_GetSensorData > SensorTimeout)
		{
			LED2_MSGTX_Blink();
			HAL_Delay(100);
			LED2_MSGTX_Blink();					//长时间未读到探头数据时信号灯快速闪烁2次
			HAL_Delay(100);
			LED2_MSGTX_Blink();
			return;
		}
		else
		{
			Master_SensorData_Ack(Sensor_ID, Sensor_Type);
			HAL_GPIO_TogglePin(LED2_MSGTX_GPIO_Port, LED2_MSGTX_Pin);      //发送1包数据后LED2反转一次
		}
	}

}

void Master_SensorData_Ack(uint8_t DEV_ADDR, uint8_t DEV_TYPE)
{
	if(RS485Uart_RX.RX_Buf[MASTER_FUNCTION_NUM] == 0x51)
	{
		Master51AckBuf[MASTER_51ACK_HEAD_NUM1] = 0xFF;
		Master51AckBuf[MASTER_51ACK_HEAD_NUM2] = 0xEE;
		Master51AckBuf[MASTER_51ACK_LEN_NUM]   = 0x08;
		Master51AckBuf[MASTER_51ACK_ID_NUM]    = DEV_ADDR;
		Master51AckBuf[MASTER_51ACK_FUNC_NUM]  = 0X51;
		Master51AckBuf[MASTER_51ACK_TYPE_NUM]  = DEV_TYPE;

		Master51AckBuf[MASTER_51ACK_CRCVAL_HIGH_NUM] = (uint8_t)(CRC16(Master51AckBuf,6)>> 8);
		Master51AckBuf[MASTER_51ACK_CRCVAL_LOW_NUM]  = (uint8_t)CRC16(Master51AckBuf,6);

		RS485Uart_RX_TX_Switch(TRANSMIT);   //485发送打开
		HAL_Delay(5);
		HAL_UART_Transmit(&huart4, (uint8_t *)Master51AckBuf, sizeof(Master51AckBuf), 1000);
		RS485Uart_RX_TX_Switch(RECEIVE);    //485接收打开，发送截止

		memset(Master51AckBuf, 0 , sizeof(Master51AckBuf));
	}


	if(RS485Uart_RX.RX_Buf[MASTER_FUNCTION_NUM] == 0x52)
	{
		Master52AckBuf[MASTER_52ACK_HEAD_NUM1] = 0xFF;
		Master52AckBuf[MASTER_52ACK_HEAD_NUM2] = 0xEE;

		Master52AckBuf[MASTER_52ACK_ID_NUM]    = DEV_ADDR;
		Master52AckBuf[MASTER_52ACK_FUNC_NUM]  = 0X52;

		switch(DEV_TYPE)
		{
			case Temperature_Type:
				Master52AckBuf[2]  = 0x0B;
				Master52AckBuf[5]  = Temperature_Type;
				Master52AckBuf[6]  = 0x02;
				Master52AckBuf[7]  = (uint8_t)(Sensor_Data.Temperature_m >> 8);
				Master52AckBuf[8]  = (uint8_t)Sensor_Data.Temperature_m;

				Master52AckBuf[9]  = (uint8_t)(CRC16(Master52AckBuf, 9)>> 8);
				Master52AckBuf[10] = (uint8_t)CRC16(Master52AckBuf, 9);

				break;
			case Temperature_Humidity_Type:
				Master52AckBuf[2]  = 0x0F;
				Master52AckBuf[5]  = Temperature_Type;
				Master52AckBuf[6]  = 0x02;
				Master52AckBuf[7]  = (uint8_t)(Sensor_Data.Temperature_m >> 8);
				Master52AckBuf[8]  = (uint8_t)Sensor_Data.Temperature_m;
				Master52AckBuf[9]  = Humidity_Type;
				Master52AckBuf[10] = 0x02;
				Master52AckBuf[11] = (uint8_t)(Sensor_Data.Humidity >> 8);
				Master52AckBuf[12] = (uint8_t)Sensor_Data.Humidity;

				Master52AckBuf[13] = (uint8_t)(CRC16(Master52AckBuf, 13)>> 8);
				Master52AckBuf[14] = (uint8_t)CRC16(Master52AckBuf, 13);

				break;
			case Outside_Temperature_Humidity_Type:
				Master52AckBuf[2]  = 0x0F;
				Master52AckBuf[5]  = Outside_Temperature_Type;
				Master52AckBuf[6]  = 0x02;
				Master52AckBuf[7]  = (uint8_t)(Sensor_Data.Temperature_m >> 8);
				Master52AckBuf[8]  = (uint8_t)Sensor_Data.Temperature_m;
				Master52AckBuf[9]  = Outside_Humidity_Type;
				Master52AckBuf[10] = 0x02;
				Master52AckBuf[11] = (uint8_t)(Sensor_Data.Humidity >> 8);
				Master52AckBuf[12] = (uint8_t)Sensor_Data.Humidity;

				Master52AckBuf[13] = (uint8_t)(CRC16(Master52AckBuf, 13)>> 8);
				Master52AckBuf[14] = (uint8_t)CRC16(Master52AckBuf, 13);

				break;
			case Water_Temperature_Type:
				Master52AckBuf[2]  = 0x0B;
				Master52AckBuf[5]  = Water_Temperature_Type;
				Master52AckBuf[6]  = 0x02;
				Master52AckBuf[7]  = (uint8_t)(Sensor_Data.WaterTemperature >> 8);
				Master52AckBuf[8]  = (uint8_t)Sensor_Data.WaterTemperature;

				Master52AckBuf[9]  = (uint8_t)(CRC16(Master52AckBuf, 9)>> 8);
				Master52AckBuf[10] = (uint8_t)CRC16(Master52AckBuf, 9);

				break;
			case Negative_Pressure_Type:
				Master52AckBuf[2]  = 0x0B;
				Master52AckBuf[5]  = Negative_Pressure_Type;
				Master52AckBuf[6]  = 0x02;
				Master52AckBuf[7]  = (uint8_t)(Sensor_Data.NegativePressure >> 8);
				Master52AckBuf[8]  = (uint8_t)Sensor_Data.NegativePressure;

				Master52AckBuf[9]  = (uint8_t)(CRC16(Master52AckBuf, 9)>> 8);
				Master52AckBuf[10] = (uint8_t)CRC16(Master52AckBuf, 9);

				break;
			case Gas_CO2_Type:
				Master52AckBuf[2]  = 0x0B;
				Master52AckBuf[5]  = Gas_CO2_Type;
				Master52AckBuf[6]  = 0x02;
				Master52AckBuf[7]  = (uint8_t)(Sensor_Data.CO2_Data >> 8);
				Master52AckBuf[8]  = (uint8_t)Sensor_Data.CO2_Data;

				Master52AckBuf[9]  = (uint8_t)(CRC16(Master52AckBuf, 9)>> 8);
				Master52AckBuf[10] = (uint8_t)CRC16(Master52AckBuf, 9);

				break;
			case Gas_NH3_Type:
				Master52AckBuf[2]  = 0x0B;
				Master52AckBuf[5]  = Gas_NH3_Type;
				Master52AckBuf[6]  = 0x02;
				Master52AckBuf[7]  = (uint8_t)(Sensor_Data.NH3_Data >> 8);
				Master52AckBuf[8]  = (uint8_t)Sensor_Data.NH3_Data;

				Master52AckBuf[9]  = (uint8_t)(CRC16(Master52AckBuf, 9)>> 8);
				Master52AckBuf[10] = (uint8_t)CRC16(Master52AckBuf, 9);

				break;
			case Gas_O2_Type:
				Master52AckBuf[2]  = 0x0B;
				Master52AckBuf[5]  = Gas_O2_Type;
				Master52AckBuf[6]  = 0x02;
				Master52AckBuf[7]  = (uint8_t)(Sensor_Data.O2_Data >> 8);
				Master52AckBuf[8]  = (uint8_t)Sensor_Data.O2_Data;

				Master52AckBuf[9]  = (uint8_t)(CRC16(Master52AckBuf, 9)>> 8);
				Master52AckBuf[10] = (uint8_t)CRC16(Master52AckBuf, 9);

				break;
			case Illumination_Type:
				Master52AckBuf[2]  = 0x0B;
				Master52AckBuf[5]  = Illumination_Type;
				Master52AckBuf[6]  = 0x02;
				Master52AckBuf[7]  = (uint8_t)(Sensor_Data.Illumination >> 8);
				Master52AckBuf[8]  = (uint8_t)Sensor_Data.Illumination;

				Master52AckBuf[9]  = (uint8_t)(CRC16(Master52AckBuf, 9)>> 8);
				Master52AckBuf[10] = (uint8_t)CRC16(Master52AckBuf, 9);

				break;
			case WindowPosition_Type:
				Master52AckBuf[2]  = 0x0B;
				Master52AckBuf[5]  = WindowPosition_Type;
				Master52AckBuf[6]  = 0x02;
				Master52AckBuf[7]  = (uint8_t)(Sensor_Data.WindowPosition >> 8);
				Master52AckBuf[8]  = (uint8_t)Sensor_Data.WindowPosition;

				Master52AckBuf[9]  = (uint8_t)(CRC16(Master52AckBuf, 9)>> 8);
				Master52AckBuf[10] = (uint8_t)CRC16(Master52AckBuf, 9);

				break;
			default :
				break;
		}

		RS485Uart_RX_TX_Switch(TRANSMIT);   //485发送打开
		HAL_Delay(5);
		HAL_UART_Transmit(&huart4, (uint8_t *)Master52AckBuf, sizeof(Master52AckBuf), 1000);
		RS485Uart_RX_TX_Switch(RECEIVE);    //485接收打开，发送截止

		memset(Master52AckBuf, 0 , sizeof(Master52AckBuf));
	}
}


/****************************************************养殖卫士环控Modbus传输协议**************************************************************/
void ModbusData_Analysis(void)
{

	uint16_t Modbus_CRC_VAL;
    uint16_t Modbus_CHECK_CRC;


	if(RS485Uart_RX.rx_len != PACKET_SIZE_NUM)
		return;

	Modbus_CRC_VAL = (RS485Uart_RX.RX_Buf[CRC_VAL_HIGH_NUM]<<8) | RS485Uart_RX.RX_Buf[CRC_VAL_LOW_NUM];  //数据包中的CRC校验值
	Modbus_CHECK_CRC = CRC16(RS485Uart_RX.RX_Buf, PACKET_SIZE_NUM-2);     							     //计算得到的CRC校验值
	if(Modbus_CRC_VAL != Modbus_CHECK_CRC)
		return;


	if(RS485Uart_RX.RX_Buf[SLAVE_DEV_ADDR_NUM] == Sensor_ID)
	{
		Tick_RS485Ack = HAL_GetTick();

		if(Tick_RS485Ack - Tick_GetSensorData > SensorTimeout)
		{
//			ModBus_SensorOutline_Ack(Sensor_ID);

			LED2_MSGTX_Blink();
			HAL_Delay(100);
			LED2_MSGTX_Blink();			//长时间未读到探头数据时信号灯快速闪烁2次
		}
		else
		{
			ModBus_SensorData_Ack(Sensor_ID);
			HAL_GPIO_TogglePin(LED2_MSGTX_GPIO_Port, LED2_MSGTX_Pin);      //发送1包数据后LED2反转一次
		}
	}
}

void ModBus_SensorData_Ack(uint8_t DEV_ADDR)
{
	ModbusAckBuf[SLAVE_DEV_ADDR_NUM2] = DEV_ADDR;
    ModbusAckBuf[FUNC_NUM2]           = FUNC_VALUE_READ;
    ModbusAckBuf[BYTES_NUM]           = 4;


	switch(DEV_ADDR)
	{
		case TEMHUM_MODBUS_SLAVE_ADDR2:
		case TEMHUM_MODBUS_SLAVE_ADDR3:
		case TEMHUM_MODBUS_SLAVE_ADDR4:
		case TEMHUM_MODBUS_SLAVE_ADDR5:
			ModbusAckBuf[REG1_VAL_HIGH_NUM] = (uint8_t)(Sensor_Data.Temperature >> 8);
			ModbusAckBuf[REG1_VAL_LOW_NUM]  = (uint8_t)Sensor_Data.Temperature;
			ModbusAckBuf[REG2_VAL_HIGH_NUM] = (uint8_t)(Sensor_Data.Humidity >> 8);
			ModbusAckBuf[REG2_VAL_LOW_NUM]  = (uint8_t)Sensor_Data.Humidity;
			break;
		case NEGPRE_MODBUS_SLAVE_ADDR:
			ModbusAckBuf[REG1_VAL_HIGH_NUM] = (uint8_t)(Sensor_Data.NegativePressure >> 8);
		    ModbusAckBuf[REG1_VAL_LOW_NUM]  = (uint8_t)Sensor_Data.NegativePressure;
		    break;
		case CO2_MODBUS_SLAVE_ADDR:
			ModbusAckBuf[REG1_VAL_HIGH_NUM] = (uint8_t)(Sensor_Data.CO2_Data >> 8);
			ModbusAckBuf[REG1_VAL_LOW_NUM]  = (uint8_t)Sensor_Data.CO2_Data;
			break;
		case NH3_MODBUS_SLAVE_ADDR:
			ModbusAckBuf[REG1_VAL_HIGH_NUM] = (uint8_t)(Sensor_Data.NH3_Data >> 8);
			ModbusAckBuf[REG1_VAL_LOW_NUM]  = (uint8_t)Sensor_Data.NH3_Data;
			break;
		default:
			break;
	}
	ModbusAckBuf[CRC_VAL_HIGH_NUM2] = (uint8_t)(CRC16(ModbusAckBuf,7)>> 8);
	ModbusAckBuf[CRC_VAL_LOW_NUM2]  = (uint8_t)CRC16(ModbusAckBuf,7);


	RS485Uart_RX_TX_Switch(TRANSMIT);   //485发送打开
	HAL_Delay(1);
	HAL_UART_Transmit(&huart4, (uint8_t *)ModbusAckBuf, sizeof(ModbusAckBuf), 1000);
	RS485Uart_RX_TX_Switch(RECEIVE);    //485接收打开，发送截止

	memset(ModbusAckBuf, 0 , sizeof(ModbusAckBuf));

}

void ModBus_SensorOutline_Ack(uint8_t DEV_ADDR)
{
	ModbusAckBuf[SLAVE_DEV_ADDR_NUM2] = DEV_ADDR;
    ModbusAckBuf[FUNC_NUM2]           = FUNC_VALUE_READ;
    ModbusAckBuf[BYTES_NUM]           = 4;


	ModbusAckBuf[REG1_VAL_HIGH_NUM] = (uint8_t)(10000 >> 8);
	ModbusAckBuf[REG1_VAL_LOW_NUM]  = (uint8_t)10000;
	ModbusAckBuf[REG2_VAL_HIGH_NUM] = (uint8_t)(10000 >> 8);
	ModbusAckBuf[REG2_VAL_LOW_NUM]  = (uint8_t)10000;


	ModbusAckBuf[CRC_VAL_HIGH_NUM2] = (uint8_t)(CRC16(ModbusAckBuf,7)>> 8);
	ModbusAckBuf[CRC_VAL_LOW_NUM2]  = (uint8_t)CRC16(ModbusAckBuf,7);


	RS485Uart_RX_TX_Switch(TRANSMIT);   //485发送打开
	HAL_UART_Transmit(&huart4, (uint8_t *)ModbusAckBuf, sizeof(ModbusAckBuf), 1000);
	RS485Uart_RX_TX_Switch(RECEIVE);    //485接收打开，发送截止

	memset(ModbusAckBuf, 0 , sizeof(ModbusAckBuf));

}

/*******************************************************************************************************************************************/


void RS485Uart_RX_TX_Switch(RS485_STATE RS485state)
{
	if(RS485state == TRANSMIT)
		HAL_GPIO_WritePin(RS485_SEL_GPIO_Port, RS485_SEL_Pin, GPIO_PIN_SET);		//拉高RS485_SEL脚，RS485为发送状态
	if(RS485state == RECEIVE)
		HAL_GPIO_WritePin(RS485_SEL_GPIO_Port, RS485_SEL_Pin, GPIO_PIN_RESET);		//拉低RS485_SEL脚，RS485为接收状态
}


const uint8_t auchCRCHi[] =
 {
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40
};

const uint8_t auchCRCLo[] =
{
    0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,
    0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,0x04,
    0xCC,0x0C,0x0D,0xCD,0x0F,0xCF,0xCE,0x0E,
    0x0A,0xCA,0xCB,0x0B,0xC9,0x09,0x08,0xC8,
    0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,
    0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC,
    0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,
    0xD2,0x12,0x13,0xD3,0x11,0xD1,0xD0,0x10,
    0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,
    0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4,
    0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,
    0xFA,0x3A,0x3B,0xFB,0x39,0xF9,0xF8,0x38,
    0x28,0xE8,0xE9,0x29,0xEB,0x2B,0x2A,0xEA,
    0xEE,0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C,
    0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,
    0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,
    0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,0x62,
    0x66,0xA6,0xA7,0x67,0xA5,0x65,0x64,0xA4,
    0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,0x6E,0xAE,
    0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
    0x78,0xB8,0xB9,0x79,0xBB,0x7B,0x7A,0xBA,
    0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,
    0xB4,0x74,0x75,0xB5,0x77,0xB7,0xB6,0x76,
    0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,
    0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,
    0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,
    0x9C,0x5C,0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,
    0x5A,0x9A,0x9B,0x5B,0x99,0x59,0x58,0x98,
    0x88,0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,
    0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
    0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,
    0x82,0x42,0x43,0x83,0x41,0x81,0x80,0x40
};

uint16_t CRC16(uint8_t *puchMsg, uint8_t usDataLen)
{
	uint8_t uchCRCHi = 0xFF;
	uint8_t uchCRCLo = 0xFF;
	uint8_t uIndex;

	while (usDataLen--)
	{
		uIndex = uchCRCHi ^ *puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	return (uchCRCHi << 8 | uchCRCLo);
}





