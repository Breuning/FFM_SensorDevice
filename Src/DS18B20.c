/*
 * DS18B20.c
 *
 *  Created on: 2019年8月8日
 *      Author: Breuning
 */
#include "DS18B20.h"
#include "gpio.h"
#include "tim.h"
#include "main.h"
#include "string.h"
#include <stdio.h>
#include "SensorAnalysis.h"
#include <stdlib.h>

static void DS18B20_Rst(void);
static uint8_t DS18B20_Check(void);
static void DS18B20_Write_Byte(uint8_t dat);
static uint8_t DS18B20_Read_Byte(void);
static uint8_t DS18B20_Read_Bit(void);
static uint8_t B20DataCheckCRC(uint8_t *p, uint8_t len);
static void DS18B20_DQ_IN(void);
static void DS18B20_DQ_OUT(void);


const uint8_t DS18B20_CRC_Tab[256]=
{
  0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
 35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
 70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
 17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
 50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
 87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};


//--------------------------------------------------------------------------------------------
//	@funtion:	   从DS18B20得到温度值
//	@parameters:  void
//	@return:	   温度值 (-550~1250, 精度为0.1C)
//--------------------------------------------------------------------------------------------
uint8_t GetWaterTemFrom18B20(void)
{
		uint8_t temp;                //温度正负标志
		uint8_t tem_L, tem_H;
		short tem;

		uint8_t B20Data[9];

		uint16_t EffectTem = 0;

		DS18B20_Rst();
		if(DS18B20_Check())
			return 1;

		DS18B20_Write_Byte(0xcc);    // 0xcc 跳过ROM指令  当总线上只有一个器件的时候，可以跳过 ROM，不进行ROM检测
		DS18B20_Write_Byte(0x44);    // 0x44 启动温度转化命令   启动一次温度转换
		HAL_Delay(750);              // 最大的转换时间是 750ms, 当启动转换后，至少要再等 750ms 之后才能读取温度

		DS18B20_Rst();
		DS18B20_Check();
		DS18B20_Write_Byte(0xcc);    // 0xcc 跳过ROM指令  当总线上只有一个器件的时候，可以跳过 ROM，不进行ROM检测
		DS18B20_Write_Byte(0xbe);    // 0xbe 读暂存寄存器指令   读取温度数值

		//读取高速暂存存储器9个字节的数据
		B20Data[0]=DS18B20_Read_Byte();   //温度低位数据
		B20Data[1]=DS18B20_Read_Byte();   //温度高位数据
		B20Data[2]=DS18B20_Read_Byte();   //TH值
		B20Data[3]=DS18B20_Read_Byte();   //TL值
		B20Data[4]=DS18B20_Read_Byte();   //配置寄存器值
		B20Data[5]=DS18B20_Read_Byte();   //第6~8字节未用，表现为全逻辑1
		B20Data[6]=DS18B20_Read_Byte();
		B20Data[7]=DS18B20_Read_Byte();
		B20Data[8]=DS18B20_Read_Byte();   //前面所有8个字节的CRC码，可用来保证通信正确

		if(B20DataCheckCRC(B20Data,9) == 0)
		{
			tem_L = B20Data[0];     		 //读取的温度低位数据
			tem_H = B20Data[1];     		 //读取的温度高位数据

			if(tem_H>7)                 	 //高八位大于7则符号位为1
			{
				tem_H = ~tem_H;              //将补码变换为原码
				tem_L = ~tem_L;
				temp = 0;                    //温度为负
			}
			else
			{
				temp = 1;                    //温度为正
			}
			tem = (tem_H << 8) + tem_L;
			tem = (float)tem * 0.625;        //转换，10倍值传输，正常值为 * 0.0625


			if(temp)
			{
				EffectTem = tem;
			}
			else
			{
				EffectTem = (uint16_t)abs(tem);
				EffectTem |= 0x8000;
			}
		}

		Sensor_Data.WaterTemperature = EffectTem;     //返回温度值
		Tick_GetSensorData = HAL_GetTick();
		return 0;
}

//--------------------------------------------------------------------------------------------
//	@funtion:	   复位DS18B20，stm32给DS18B20发送复位脉冲
//--------------------------------------------------------------------------------------------
static void DS18B20_Rst(void)
{
		DS18B20_DQ_OUT();           //DQ脚配置为输出模式

		DS18B20_DQ_LOW();           //拉低DQ
		Delay_us(750);              //拉低 480~960us，stm32至少产生480us的低电平复位信号
		DS18B20_DQ_HIGH();          //拉高DQ，stm32在产生复位信号后，需将总线拉高
		Delay_us(15);               //拉高 15~60us， DS18B20接收到stm32的复位信号后，会在15~60us后给主机发一个存在脉冲
}

//--------------------------------------------------------------------------------------------
//	@funtion:	   检测DS18B20给stm32返回的存在脉冲, 存在脉冲为一个60~240us的低电平信号
//  @return:      1 未检测到DS18B20的存在; 0 检测到DS18B20的存在
//--------------------------------------------------------------------------------------------
static uint8_t DS18B20_Check(void)
{
		uint8_t cnt = 0;
		DS18B20_DQ_IN();  			               //DQ脚配置为输入模式

		/*检测存在脉冲*/
		while (DS18B20_DQ_IN_Read && cnt < 200)
		{
			cnt++;
			Delay_us(1);
		}
		if (cnt >= 200)                             //经过200us后，存在脉冲都还没有到来
			return 1;
		else
			cnt = 0 ;

		/*检测DS18B20是否释放总线*/
		while (!DS18B20_DQ_IN_Read && cnt < 240)   //存在脉冲到来，且存在的时间不能超过240us
		{
			cnt++;
			Delay_us(1);
		}
		if (cnt >= 240)
			return 1;
		else
			return 0;
}

//--------------------------------------------------------------------------------------------
//	@funtion:	    写一个字节到DS18B20
//	@parameters:  dat 要写入的字节
//--------------------------------------------------------------------------------------------
static void DS18B20_Write_Byte(uint8_t dat)
{
		uint8_t i;
		uint8_t testb;
		DS18B20_DQ_OUT();           // DS18B20_DQ脚配置为输出模式

		for (i = 1; i <= 8; i++)
		{
			testb = dat & 0x01;     // dat末位是0返回0，末位是1返回1
			dat = dat >> 1;		    // dat右移一位
			if (testb)              // 末位是1，即写入1
			{
				DS18B20_DQ_LOW();   // Write 1
				Delay_us(2);        // 拉低大于1us
				DS18B20_DQ_HIGH();
				Delay_us(70);       // 拉高持续时间要大于 60us，DS18B20 会在15us到 60us之间来读取这个1
			}
			else                    // 末位是0，即写入0
			{
				DS18B20_DQ_LOW();   // Write 0
				Delay_us(70);       // 拉低60~120us，DS18B20 会在从 15us到60us之间的时间来读取这个0
				DS18B20_DQ_HIGH();
				Delay_us(2);
			}
		}
}

//--------------------------------------------------------------------------------------------
//	@funtion:	    从DS18B20读取一个字节
//	@return:      读到的数据
//--------------------------------------------------------------------------------------------
static uint8_t DS18B20_Read_Byte(void)
{
		uint8_t i,j,dat;
		dat=0;
		for (i = 1; i <= 8; i++)
		{
			j = DS18B20_Read_Bit();
			dat = (j<<7)|(dat>>1);
		}
		return dat;
}

//--------------------------------------------------------------------------------------------
//	@funtion:	    从DS18B20读取一个位
//	@return:      1/0
//--------------------------------------------------------------------------------------------
static uint8_t DS18B20_Read_Bit(void)
{
		uint8_t data;

		DS18B20_DQ_OUT();
		DS18B20_DQ_LOW();
		Delay_us(10);         //拉低至少保持1us的时间,从拉低引脚到读取引脚状态,不能超过 15us(建议设为10，11，12)

		DS18B20_DQ_IN();
		if(DS18B20_DQ_IN_Read)
			data = 1;
		else
			data = 0;
		Delay_us(50);       //读取完成后需延时至少45us
		return data;
}

//--------------------------------------------------------------------------------------------
//	@funtion:	  DS18B20_DQ脚配置为输入模式
//--------------------------------------------------------------------------------------------
/*此方法占用时间太久，几十个us
static void DS18B20_DQ_IN(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = DS18B20_DQ_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(DS18B20_DQ_GPIO_Port, &GPIO_InitStruct);
}
*/
static void DS18B20_DQ_IN(void)
{
	GPIOB->CRH &= 0XFFFFFFF0;
	GPIOB->CRH |= 8 << 0;
}
//--------------------------------------------------------------------------------------------
//	@funtion:	  DS18B20_DQ脚配置为输出模式
//--------------------------------------------------------------------------------------------
/*此方法占用时间太久，几十个us
static void DS18B20_DQ_OUT(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = DS18B20_DQ_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(DS18B20_DQ_GPIO_Port, &GPIO_InitStruct);
}
*/
static void DS18B20_DQ_OUT(void)
{
	GPIOB->CRH &= 0XFFFFFFF0;
	GPIOB->CRH |= 3 << 0;
}

//--------------------------------------------------------------------------------------------
//	@funtion:	    查表法计算CRC
//--------------------------------------------------------------------------------------------
static uint8_t B20DataCheckCRC(uint8_t *p, uint8_t len)
{
		uint8_t i = 0;
		uint8_t DS18B20_CRC_result = 0;
		for(i = 0; i < len; i++)
			DS18B20_CRC_result = DS18B20_CRC_Tab[(*p++) ^ DS18B20_CRC_result];
		return DS18B20_CRC_result;
}

/*朱泽鹏写的滤波算法，暂时不用
static uint8_t FilterAlgorithm(short* tem)//DS18B20滤波算法
{
		static short RegularTemperature[SIZE]={0};//常规温度数据
		static short SuspiciousTemperature[SIZE]={0};//可疑温度数据
		static uint8_t RegularCollectNum=0;//常规温度收集次数
		uint8_t RegularNum=0;//常规突变次数统计

		static uint8_t SuspiciousCollectNum=0;//可疑温度收集次数
		uint8_t SuspiciousNum=0;//可疑突变次数统计
		short temp =0;
		temp= *tem;
		uint8_t i=0;

		for(i=0;i<RegularCollectNum;i++)
		{
			if(abs(*(RegularTemperature+i)-temp)>T_MUTATION_MAXIMUN)//判断是否出现突变,单次突变不超过10度
			{
				RegularNum++;
			}
		}

		if(RegularNum>0)//已经存在温度突变现象
		{
			RegularNum = 0;
			*(SuspiciousTemperature+SuspiciousCollectNum) = temp;//可疑温度数组收集数据
			//隔离可疑数据，进一步判断可疑数据是否可以作为有效值
			for(i=0;i<SuspiciousCollectNum;i++)
			{
				if(abs(*(SuspiciousTemperature+i)-temp)>T_MUTATION_MAXIMUN)
				{
					SuspiciousNum++;
				}
			}
			if(SuspiciousCollectNum<(SIZE-1))
			{
				SuspiciousCollectNum++;
				return 0;
			}
			else
			{
				if(SuspiciousNum>0)
				{
					SuspiciousNum=0;
					if(SuspiciousCollectNum == (SIZE-1))//可疑数据是干扰很强的数据
					{
						memset(RegularTemperature,0,sizeof(RegularTemperature));
						RegularCollectNum = 0;

						memset(SuspiciousTemperature,0,sizeof(SuspiciousTemperature));
						SuspiciousCollectNum = 0;
					}
					return 0;
				}
				else//鉴别出可疑数据就是有效数据
				{
					for(i=0;i<(SIZE-1);i++)
					{
						*(SuspiciousTemperature+i)=*(SuspiciousTemperature+i+1);//可疑数据更新
					}
					memcpy(RegularTemperature,SuspiciousTemperature,sizeof(SuspiciousTemperature));
					memset(SuspiciousTemperature,0,sizeof(SuspiciousTemperature));
					RegularCollectNum = (SIZE-1);
					return 1;
				}
			}
			return 0;
		}
		else
		{
			if(RegularCollectNum<(SIZE-1))//数组里面需要装满5个数据
			{
				*(RegularTemperature+RegularCollectNum) = temp;//常规温度数组收集数据
				RegularCollectNum++;
			}
			else//无突变的数据
			{
				for(i=0;i<(SIZE-1);i++)
				{
					*(RegularTemperature+i)=*(RegularTemperature+i+1);//常规数据更新
				}

				*(RegularTemperature+(SIZE-1)) = temp;//常规温度数组收集数据
			}
			memset(SuspiciousTemperature,0,sizeof(SuspiciousTemperature));//可疑数据是干扰较弱的数据，
			SuspiciousCollectNum = 0;
			return 1;
		}
}
*/














