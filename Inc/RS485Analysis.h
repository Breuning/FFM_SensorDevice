/*
 * RS485Analysis.h
 *
 *  Created on: 2019Äê10ÔÂ21ÈÕ
 *      Author: Breuning
 */

#ifndef RS485ANALYSIS_H_
#define RS485ANALYSIS_H_

#define SersorTimeout 1000*10

typedef enum
{
	RECEIVE = 0,
    TRANSMIT = 1

}RS485_STATE;

typedef enum
{
	TEMHUM_MODBUS_SLAVE_ADDR2 = 2,
	TEMHUM_MODBUS_SLAVE_ADDR3 = 3,
	TEMHUM_MODBUS_SLAVE_ADDR4 = 4,
	TEMHUM_MODBUS_SLAVE_ADDR5 = 5,
	CO2_MODBUS_SLAVE_ADDR     = 8,
	NH3_MODBUS_SLAVE_ADDR     = 9,

	FUNC_VALUE_READ            = 3,

}MODBUS_SLAVE_ADDR;

typedef enum
{
	SLAVE_DEV_ADDR_NUM = 0,
	FUNC_NUM           = 1,
	REG_ADDR_HIGH_NUM  = 2,
	REG_ADDR_LOW_NUM   = 3,
	REG_QUA_HIGH_NUM   = 4,
	REG_QUA_LOW_NUM    = 5,
	CRC_VAL_HIGH_NUM   = 6,
	CRC_VAL_LOW_NUM    = 7,
	PACKET_SIZE_NUM    = 8,

}MODBUS_RECIEVE_PACKET;

typedef enum
{
	SLAVE_DEV_ADDR_NUM2= 0,
	FUNC_NUM2          = 1,
	BYTES_NUM          = 2,
	REG1_VAL_HIGH_NUM  = 3,
	REG1_VAL_LOW_NUM   = 4,
	REG2_VAL_HIGH_NUM  = 5,
	REG2_VAL_LOW_NUM   = 6,
	CRC_VAL_HIGH_NUM2  = 7,
	CRC_VAL_LOW_NUM2   = 8,

}MODBUS_ACK_PACKET;



void RS485Analysis(void);

void ModbusData_Analysis(void);
void ModBus_SersorData_Ack(uint8_t DEV_ADDR);

void RS485Uart_RX_TX_Switch(RS485_STATE RS485state);
uint16_t CRC16(uint8_t *puchMsg, uint8_t usDataLen);









#endif /* RS485ANALYSIS_H_ */
