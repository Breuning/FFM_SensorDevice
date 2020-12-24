/*
 * WaterMeter.c
 *
 *  Created on: 2020年9月16日
 *      Author: Breuning
 */

#include "WaterMeter.h"
#include "usart.h"
#include "SensorAnalysis.h"
#include "string.h"
#include "RS485Analysis.h"


uint8_t WaterMet_ReadValueCommand[8] = {0x11, 0x03, 0x00, 0x00, 0x00, 0x04, 0x46, 0x99};


void GetValidDataFromWaterMet(void)
{
	uint16_t WaterMet_CrcVal;
    uint16_t WaterMet_CheckCrc;

    uint16_t Data_Integer, Data_Decimal;

    RS485Uart_RX_TX_Switch(TRANSMIT);   //485发送打开
	HAL_Delay(5);
	HAL_UART_Transmit(&huart4, WaterMet_ReadValueCommand, 8, 0xff);       //发送读取水表数值命令
	RS485Uart_RX_TX_Switch(RECEIVE);    //485接收打开，发送截止
	HAL_Delay(100);

	if(RS485Uart_RX.receive_flag == 1)
	{
		if(RS485Uart_RX.rx_len != 13)
			return;

		WaterMet_CrcVal   = (RS485Uart_RX.RX_Buf[11]<<8) | RS485Uart_RX.RX_Buf[12];  //数据包中的CRC校验值
		WaterMet_CheckCrc = CRC16(RS485Uart_RX.RX_Buf, 11);
		if(WaterMet_CrcVal != WaterMet_CheckCrc)
			return;

		Data_Integer = BCDtoDecimal(RS485Uart_RX.RX_Buf[3])*100 + BCDtoDecimal(RS485Uart_RX.RX_Buf[4]);
		Data_Decimal = BCDtoDecimal(RS485Uart_RX.RX_Buf[5])*100 + BCDtoDecimal(RS485Uart_RX.RX_Buf[6]);
		Sensor_Data.WaterMeter = Data_Integer + (float)Data_Decimal/10000.0;

		RS485Uart_RX.receive_flag = 0;
		memset(RS485Uart_RX.RX_Buf, 0 , sizeof(RS485Uart_RX.RX_Buf));

		HAL_UART_Receive_DMA(&huart4, RS485Uart_RX.RX_Buf, RECEIVELEN);
	}

}

unsigned BCDtoDecimal (unsigned bVal)
{
    return (bVal&0x0F) + ((bVal&0xF0) >> 4) * 10;
}

