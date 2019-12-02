/*
 * RS485Analysis.h
 *
 *  Created on: 2019年10月21日
 *      Author: Breuning
 */

#ifndef RS485ANALYSIS_H_
#define RS485ANALYSIS_H_

#define SensorTimeout 1000*15

typedef enum
{
	RECEIVE = 0,
    TRANSMIT = 1

}RS485_STATE;


typedef enum
{

	MASTER_DATA_LEN_NUM    = 2,
	MASTER_SENSOR_ID_NUM   = 3,
	MASTER_FUNCTION_NUM    = 4,            //0X51 检查设备在线命令             0X52 读取数据命令              0X53
	         S             = 5,            //0X10
	MASTER_CRCVAL_HIGH_NUM = 6,
	MASTER_CRCVAL_LOW_NUM  = 7,

}MASTER_RECIEVE_PACKET;

typedef enum
{
	MASTER_51ACK_HEAD_NUM1       = 0,
	MASTER_51ACK_HEAD_NUM2       = 1,
	MASTER_51ACK_LEN_NUM         = 2,
	MASTER_51ACK_ID_NUM          = 3,
	MASTER_51ACK_FUNC_NUM        = 4,
	MASTER_51ACK_TYPE_NUM        = 5,
	MASTER_51ACK_CRCVAL_HIGH_NUM = 6,
	MASTER_51ACK_CRCVAL_LOW_NUM  = 7,

}MASTER_51ACK_PACKET;

typedef enum
{
	MASTER_52ACK_HEAD_NUM1       = 0,
	MASTER_52ACK_HEAD_NUM2       = 1,

	MASTER_52ACK_ID_NUM          = 3,
	MASTER_52ACK_FUNC_NUM        = 4,

}MASTER_52ACK_PACKET;






typedef enum
{
	TEMHUM_MODBUS_SLAVE_ADDR2 = 2,
	TEMHUM_MODBUS_SLAVE_ADDR3 = 3,
	TEMHUM_MODBUS_SLAVE_ADDR4 = 4,
	TEMHUM_MODBUS_SLAVE_ADDR5 = 5,
	CO2_MODBUS_SLAVE_ADDR     = 8,
	NH3_MODBUS_SLAVE_ADDR     = 9,

	FUNC_VALUE_READ           = 3,

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

void MasterData_Analysis(void);
void Mater_SensorData_Ack(uint8_t DEV_ADDR, uint8_t DEV_TYPE);

void ModbusData_Analysis(void);
void ModBus_SensorData_Ack(uint8_t DEV_ADDR);
void ModBus_SensorOutline_Ack(uint8_t DEV_ADDR);

void RS485Uart_RX_TX_Switch(RS485_STATE RS485state);
uint16_t CRC16(uint8_t *puchMsg, uint8_t usDataLen);









#endif /* RS485ANALYSIS_H_ */
