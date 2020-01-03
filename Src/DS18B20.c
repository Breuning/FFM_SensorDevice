/*
 * DS18B20.c
 *
 *  Created on: 2019��8��8��
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
//	@funtion:	   ��DS18B20�õ��¶�ֵ
//	@parameters:  void
//	@return:	   �¶�ֵ (-550~1250, ����Ϊ0.1C)
//--------------------------------------------------------------------------------------------
uint8_t GetWaterTemFrom18B20(void)
{
		uint8_t temp;                //�¶�������־
		uint8_t tem_L, tem_H;
		short tem;

		uint8_t B20Data[9];

		uint16_t EffectTem = 0;

		DS18B20_Rst();
		if(DS18B20_Check())
			return 1;

		DS18B20_Write_Byte(0xcc);    // 0xcc ����ROMָ��  ��������ֻ��һ��������ʱ�򣬿������� ROM��������ROM���
		DS18B20_Write_Byte(0x44);    // 0x44 �����¶�ת������   ����һ���¶�ת��
		HAL_Delay(750);              // ����ת��ʱ���� 750ms, ������ת��������Ҫ�ٵ� 750ms ֮����ܶ�ȡ�¶�

		DS18B20_Rst();
		DS18B20_Check();
		DS18B20_Write_Byte(0xcc);    // 0xcc ����ROMָ��  ��������ֻ��һ��������ʱ�򣬿������� ROM��������ROM���
		DS18B20_Write_Byte(0xbe);    // 0xbe ���ݴ�Ĵ���ָ��   ��ȡ�¶���ֵ

		//��ȡ�����ݴ�洢��9���ֽڵ�����
		B20Data[0]=DS18B20_Read_Byte();   //�¶ȵ�λ����
		B20Data[1]=DS18B20_Read_Byte();   //�¶ȸ�λ����
		B20Data[2]=DS18B20_Read_Byte();   //THֵ
		B20Data[3]=DS18B20_Read_Byte();   //TLֵ
		B20Data[4]=DS18B20_Read_Byte();   //���üĴ���ֵ
		B20Data[5]=DS18B20_Read_Byte();   //��6~8�ֽ�δ�ã�����Ϊȫ�߼�1
		B20Data[6]=DS18B20_Read_Byte();
		B20Data[7]=DS18B20_Read_Byte();
		B20Data[8]=DS18B20_Read_Byte();   //ǰ������8���ֽڵ�CRC�룬��������֤ͨ����ȷ

		if(B20DataCheckCRC(B20Data,9) == 0)
		{
			tem_L = B20Data[0];     		 //��ȡ���¶ȵ�λ����
			tem_H = B20Data[1];     		 //��ȡ���¶ȸ�λ����

			if(tem_H>7)                 	 //�߰�λ����7�����λΪ1
			{
				tem_H = ~tem_H;              //������任Ϊԭ��
				tem_L = ~tem_L;
				temp = 0;                    //�¶�Ϊ��
			}
			else
			{
				temp = 1;                    //�¶�Ϊ��
			}
			tem = (tem_H << 8) + tem_L;
			tem = (float)tem * 0.625;        //ת����10��ֵ���䣬����ֵΪ * 0.0625


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

		Sensor_Data.WaterTemperature = EffectTem;     //�����¶�ֵ
		Tick_GetSensorData = HAL_GetTick();
		return 0;
}

//--------------------------------------------------------------------------------------------
//	@funtion:	   ��λDS18B20��stm32��DS18B20���͸�λ����
//--------------------------------------------------------------------------------------------
static void DS18B20_Rst(void)
{
		DS18B20_DQ_OUT();           //DQ������Ϊ���ģʽ

		DS18B20_DQ_LOW();           //����DQ
		Delay_us(750);              //���� 480~960us��stm32���ٲ���480us�ĵ͵�ƽ��λ�ź�
		DS18B20_DQ_HIGH();          //����DQ��stm32�ڲ�����λ�źź��轫��������
		Delay_us(15);               //���� 15~60us�� DS18B20���յ�stm32�ĸ�λ�źź󣬻���15~60us���������һ����������
}

//--------------------------------------------------------------------------------------------
//	@funtion:	   ���DS18B20��stm32���صĴ�������, ��������Ϊһ��60~240us�ĵ͵�ƽ�ź�
//  @return:      1 δ��⵽DS18B20�Ĵ���; 0 ��⵽DS18B20�Ĵ���
//--------------------------------------------------------------------------------------------
static uint8_t DS18B20_Check(void)
{
		uint8_t cnt = 0;
		DS18B20_DQ_IN();  			               //DQ������Ϊ����ģʽ

		/*����������*/
		while (DS18B20_DQ_IN_Read && cnt < 200)
		{
			cnt++;
			Delay_us(1);
		}
		if (cnt >= 200)                             //����200us�󣬴������嶼��û�е���
			return 1;
		else
			cnt = 0 ;

		/*���DS18B20�Ƿ��ͷ�����*/
		while (!DS18B20_DQ_IN_Read && cnt < 240)   //�������嵽�����Ҵ��ڵ�ʱ�䲻�ܳ���240us
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
//	@funtion:	    дһ���ֽڵ�DS18B20
//	@parameters:  dat Ҫд����ֽ�
//--------------------------------------------------------------------------------------------
static void DS18B20_Write_Byte(uint8_t dat)
{
		uint8_t i;
		uint8_t testb;
		DS18B20_DQ_OUT();           // DS18B20_DQ������Ϊ���ģʽ

		for (i = 1; i <= 8; i++)
		{
			testb = dat & 0x01;     // datĩλ��0����0��ĩλ��1����1
			dat = dat >> 1;		    // dat����һλ
			if (testb)              // ĩλ��1����д��1
			{
				DS18B20_DQ_LOW();   // Write 1
				Delay_us(2);        // ���ʹ���1us
				DS18B20_DQ_HIGH();
				Delay_us(70);       // ���߳���ʱ��Ҫ���� 60us��DS18B20 ����15us�� 60us֮������ȡ���1
			}
			else                    // ĩλ��0����д��0
			{
				DS18B20_DQ_LOW();   // Write 0
				Delay_us(70);       // ����60~120us��DS18B20 ���ڴ� 15us��60us֮���ʱ������ȡ���0
				DS18B20_DQ_HIGH();
				Delay_us(2);
			}
		}
}

//--------------------------------------------------------------------------------------------
//	@funtion:	    ��DS18B20��ȡһ���ֽ�
//	@return:      ����������
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
//	@funtion:	    ��DS18B20��ȡһ��λ
//	@return:      1/0
//--------------------------------------------------------------------------------------------
static uint8_t DS18B20_Read_Bit(void)
{
		uint8_t data;

		DS18B20_DQ_OUT();
		DS18B20_DQ_LOW();
		Delay_us(10);         //�������ٱ���1us��ʱ��,���������ŵ���ȡ����״̬,���ܳ��� 15us(������Ϊ10��11��12)

		DS18B20_DQ_IN();
		if(DS18B20_DQ_IN_Read)
			data = 1;
		else
			data = 0;
		Delay_us(50);       //��ȡ��ɺ�����ʱ����45us
		return data;
}

//--------------------------------------------------------------------------------------------
//	@funtion:	  DS18B20_DQ������Ϊ����ģʽ
//--------------------------------------------------------------------------------------------
/*�˷���ռ��ʱ��̫�ã���ʮ��us
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
//	@funtion:	  DS18B20_DQ������Ϊ���ģʽ
//--------------------------------------------------------------------------------------------
/*�˷���ռ��ʱ��̫�ã���ʮ��us
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
//	@funtion:	    �������CRC
//--------------------------------------------------------------------------------------------
static uint8_t B20DataCheckCRC(uint8_t *p, uint8_t len)
{
		uint8_t i = 0;
		uint8_t DS18B20_CRC_result = 0;
		for(i = 0; i < len; i++)
			DS18B20_CRC_result = DS18B20_CRC_Tab[(*p++) ^ DS18B20_CRC_result];
		return DS18B20_CRC_result;
}

/*������д���˲��㷨����ʱ����
static uint8_t FilterAlgorithm(short* tem)//DS18B20�˲��㷨
{
		static short RegularTemperature[SIZE]={0};//�����¶�����
		static short SuspiciousTemperature[SIZE]={0};//�����¶�����
		static uint8_t RegularCollectNum=0;//�����¶��ռ�����
		uint8_t RegularNum=0;//����ͻ�����ͳ��

		static uint8_t SuspiciousCollectNum=0;//�����¶��ռ�����
		uint8_t SuspiciousNum=0;//����ͻ�����ͳ��
		short temp =0;
		temp= *tem;
		uint8_t i=0;

		for(i=0;i<RegularCollectNum;i++)
		{
			if(abs(*(RegularTemperature+i)-temp)>T_MUTATION_MAXIMUN)//�ж��Ƿ����ͻ��,����ͻ�䲻����10��
			{
				RegularNum++;
			}
		}

		if(RegularNum>0)//�Ѿ������¶�ͻ������
		{
			RegularNum = 0;
			*(SuspiciousTemperature+SuspiciousCollectNum) = temp;//�����¶������ռ�����
			//����������ݣ���һ���жϿ��������Ƿ������Ϊ��Чֵ
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
					if(SuspiciousCollectNum == (SIZE-1))//���������Ǹ��ź�ǿ������
					{
						memset(RegularTemperature,0,sizeof(RegularTemperature));
						RegularCollectNum = 0;

						memset(SuspiciousTemperature,0,sizeof(SuspiciousTemperature));
						SuspiciousCollectNum = 0;
					}
					return 0;
				}
				else//������������ݾ�����Ч����
				{
					for(i=0;i<(SIZE-1);i++)
					{
						*(SuspiciousTemperature+i)=*(SuspiciousTemperature+i+1);//�������ݸ���
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
			if(RegularCollectNum<(SIZE-1))//����������Ҫװ��5������
			{
				*(RegularTemperature+RegularCollectNum) = temp;//�����¶������ռ�����
				RegularCollectNum++;
			}
			else//��ͻ�������
			{
				for(i=0;i<(SIZE-1);i++)
				{
					*(RegularTemperature+i)=*(RegularTemperature+i+1);//�������ݸ���
				}

				*(RegularTemperature+(SIZE-1)) = temp;//�����¶������ռ�����
			}
			memset(SuspiciousTemperature,0,sizeof(SuspiciousTemperature));//���������Ǹ��Ž��������ݣ�
			SuspiciousCollectNum = 0;
			return 1;
		}
}
*/














