/*
 * NTC.c
 *
 *  Created on: 2019年12月10日
 *      Author: Breuning
 */

#include "NTC.h"
#include "adc.h"
#include "SensorAnalysis.h"

const ntcData_t ntc_3950_data[] =
{
		{	-20	,	95.337	},
		{	-19.5	,	92.6559	},
		{	-19	,	90.058	},
		{	-18.5	,	87.5406	},
		{	-18	,	85.1009	},
		{	-17.5	,	82.7364	},
		{	-17	,	80.4445	},
		{	-16.5	,	78.2227	},
		{	-16	,	76.0689	},
		{	-15.5	,	73.9806	},
		{	-15	,	71.9558	},
		{	-14.5	,	69.9923	},
		{	-14	,	68.0881	},
		{	-13.5	,	66.2412	},
		{	-13	,	64.4499	},
		{	-12.5	,	62.7122	},
		{	-12	,	61.0264	},
		{	-11.5	,	59.3908	},
		{	-11	,	57.8038	},
		{	-10.5	,	56.2639	},
		{	-10	,	54.7694	},
		{	-9.5	,	53.3189	},
		{	-9	,	51.9111	},
		{	-8.5	,	50.5445	},
		{	-8	,	49.2178	},
		{	-7.5	,	47.9298	},
		{	-7	,	46.6792	},
		{	-6.5	,	45.4649	},
		{	-6	,	44.2856	},
		{	-5.5	,	43.1403	},
		{	-5	,	42.0279	},
		{	-4.5	,	40.9474	},
		{	-4	,	39.8978	},
		{	-3.5	,	38.878	},
		{	-3	,	37.8873	},
		{	-2.5	,	36.9246	},
		{	-2	,	35.9892	},
		{	-1.5	,	35.0801	},
		{	-1	,	34.1965	},
		{	-0.5	,	33.3378	},
		{	0	,	32.503	},
		{	0.5	,	31.6915	},
		{	1	,	30.9026	},
		{	1.5	,	30.1355	},
		{	2	,	29.3896	},
		{	2.5	,	28.6644	},
		{	3	,	27.959	},
		{	3.5	,	27.273	},
		{	4	,	26.6058	},
		{	4.5	,	25.9567	},
		{	5	,	25.3254	},
		{	5.5	,	24.7111	},
		{	6	,	24.1135	},
		{	6.5	,	23.532	},
		{	7	,	22.9661	},
		{	7.5	,	22.4154	},
		{	8	,	21.8795	},
		{	8.5	,	21.3579	},
		{	9	,	20.8502	},
		{	9.5	,	20.3559	},
		{	10	,	19.8747	},
		{	10.5	,	19.4063	},
		{	11	,	18.9502	},
		{	11.5	,	18.506	},
		{	12	,	18.0735	},
		{	12.5	,	17.6523	},
		{	13	,	17.2421	},
		{	13.5	,	16.8426	},
		{	14	,	16.4534	},
		{	14.5	,	16.0743	},
		{	15	,	15.7049	},
		{	15.5	,	15.345	},
		{	16	,	14.9944	},
		{	16.5	,	14.6528	},
		{	17	,	14.3198	},
		{	17.5	,	13.9954	},
		{	18	,	13.6792	},
		{	18.5	,	13.371	},
		{	19	,	13.0705	},
		{	19.5	,	12.7777	},
		{	20	,	12.4922	},
		{	20.5	,	12.2138	},
		{	21	,	11.9425	},
		{	21.5	,	11.6778	},
		{	22	,	11.4198	},
		{	22.5	,	11.1681	},
		{	23	,	10.9227	},
		{	23.5	,	10.6834	},
		{	24	,	10.4499	},
		{	24.5	,	10.2222	},
		{	25	,	10	},
		{	25.5	,	9.7833	},
		{	26	,	9.5718	},
		{	26.5	,	9.3655	},
		{	27	,	9.1642	},
		{	27.5	,	8.9677	},
		{	28	,	8.776	},
		{	28.5	,	8.5889	},
		{	29	,	8.4063	},
		{	29.5	,	8.2281	},
		{	30	,	8.0541	},
		{	30.5	,	7.8842	},
		{	31	,	7.7184	},
		{	31.5	,	7.5565	},
		{	32	,	7.3985	},
		{	32.5	,	7.2442	},
		{	33	,	7.0935	},
		{	33.5	,	6.9463	},
		{	34	,	6.8026	},
		{	34.5	,	6.6622	},
		{	35	,	6.5251	},
		{	35.5	,	6.3912	},
		{	36	,	6.2604	},
		{	36.5	,	6.1326	},
		{	37	,	6.0077	},
		{	37.5	,	5.8858	},
		{	38	,	5.7666	},
		{	38.5	,	5.6501	},
		{	39	,	5.5363	},
		{	39.5	,	5.4251	},
		{	40	,	5.3164	},
		{	40.5	,	5.2102	},
		{	41	,	5.1064	},
		{	41.5	,	5.0049	},
		{	42	,	4.9057	},
		{	42.5	,	4.8088	},
		{	43	,	4.714	},
		{	43.5	,	4.6213	},
		{	44	,	4.5307	},
		{	44.5	,	4.4421	},
		{	45	,	4.3554	},
		{	45.5	,	4.2707	},
		{	46	,	4.1878	},
		{	46.5	,	4.1068	},
		{	47	,	4.0275	},
		{	47.5	,	3.95	},
		{	48	,	3.8742	},
		{	48.5	,	3.8	},
		{	49	,	3.7275	},
		{	49.5	,	3.6565	},
		{	50	,	3.587	},
		{	50.5	,	3.519	},
		{	51	,	3.4525	},
		{	51.5	,	3.3875	},
		{	52	,	3.3238	},
		{	52.5	,	3.2615	},
		{	53	,	3.2005	},
		{	53.5	,	3.1408	},
		{	54	,	3.0824	},
		{	54.5	,	3.0252	},
		{	55	,	2.9692	},
		{	55.5	,	2.9144	},
		{	56	,	2.8608	},
		{	56.5	,	2.8082	},
		{	57	,	2.7568	},
		{	57.5	,	2.7065	},
		{	58	,	2.6572	},
		{	58.5	,	2.6089	},
		{	59	,	2.5616	},
		{	59.5	,	2.5153	},
		{	60	,	2.47	},
		{	60.5	,	2.4255	},
		{	61	,	2.382	},
		{	61.5	,	2.3394	},
		{	62	,	2.2977	},
		{	62.5	,	2.2568	},
		{	63	,	2.2167	},
		{	63.5	,	2.1775	},
		{	64	,	2.139	},
		{	64.5	,	2.1013	},
		{	65	,	2.0644	},
		{	65.5	,	2.0282	},
		{	66	,	1.9928	},
		{	66.5	,	1.958	},
		{	67	,	1.924	},
		{	67.5	,	1.8906	},
		{	68	,	1.8579	},
		{	68.5	,	1.8258	},
		{	69	,	1.7944	},
		{	69.5	,	1.7636	},
		{	70	,	1.7334	},
		{	70.5	,	1.7037	},
		{	71	,	1.6747	},
		{	71.5	,	1.6462	},
		{	72	,	1.6183	},
		{	72.5	,	1.591	},
		{	73	,	1.5641	},
		{	73.5	,	1.5378	},
		{	74	,	1.512	},
		{	74.5	,	1.4867	},
		{	75	,	1.4619	},
		{	75.5	,	1.4375	},
		{	76	,	1.4136	},
		{	76.5	,	1.3902	},
		{	77	,	1.3672	},
		{	77.5	,	1.3447	},
		{	78	,	1.3225	},
		{	78.5	,	1.3008	},
		{	79	,	1.2795	},
		{	79.5	,	1.2586	},
		{	80	,	1.2381	},
		{	80.5	,	1.218	},
		{	81	,	1.1983	},
		{	81.5	,	1.1789	},
		{	82	,	1.1599	},
		{	82.5	,	1.1412	},
		{	83	,	1.1229	},
		{	83.5	,	1.105	},
		{	84	,	1.0873	},
		{	84.5	,	1.07	},
		{	85	,	1.053	},
		{	85.5	,	1.0363	},
		{	86	,	1.0199	},
		{	86.5	,	1.0038	},
		{	87	,	0.988	},
		{	87.5	,	0.9725	},
		{	88	,	0.9573	},
		{	88.5	,	0.9424	},
		{	89	,	0.9277	},
		{	89.5	,	0.9133	},
		{	90	,	0.8991	},
		{	90.5	,	0.8852	},
		{	91	,	0.8715	},
		{	91.5	,	0.8581	},
		{	92	,	0.845	},
		{	92.5	,	0.832	},
		{	93	,	0.8193	},
		{	93.5	,	0.8068	},
		{	94	,	0.7945	},
		{	94.5	,	0.7825	},
		{	95	,	0.7707	},
		{	95.5	,	0.759	},
		{	96	,	0.7476	},
		{	96.5	,	0.7364	},
		{	97	,	0.7253	},
		{	97.5	,	0.7145	},
		{	98	,	0.7038	},
		{	98.5	,	0.6933	},
		{	99	,	0.6831	},
		{	99.5	,	0.6729	},
		{	100	,	0.663	},
};

BOOL NTC_ifAvailable(float valueOfResistance )
{
	if ((valueOfResistance > 90) || (valueOfResistance < 0.4))
	{
		return FALSE;
	}
	return TRUE;
}

//获取NTC热敏电阻数值 (PC2-ADC3 电阻信号)
void GetWaterTemFromNTC(void)
{
	double r = 0;
	double t = 0;

 	r = (double)ADC_Average(&hadc3, 10);          //10次数据的平均值
 	r = (double)(r * 3.3 / 4095.0);               //采集到的实际电压值
	r = NTC_PUSH_UP_RESISTOR_KR / (3.3/r - 1);    //电压值转化为电阻值

	if(NTC_ifAvailable(r) != TRUE)
		return;
	else
		Tick_GetSensorData = HAL_GetTick();


	if(r > 95.3)
	{
		t = -20.0;
	}

	if(r < 0.673)
	{
		t = 100.0;
	}

	for (uint32_t i = 0; i < sizeof(ntc_3950_data) / sizeof(ntcData_t) - 1; i++)
	{
		if((r <= ntc_3950_data[i].r) && ( r >= ntc_3950_data[i + 1].r))
		{

			t = (ntc_3950_data[i + 1].t - ntc_3950_data[i].t) / (ntc_3950_data[i].r - ntc_3950_data[i + 1].r);
			t *= (r - ntc_3950_data[i + 1].r);
			t +=  ntc_3950_data[i].t;
			t += 0.05;
		}
	}

	Sensor_Data.WaterTemperature = (uint16_t)(t * 10);               //温度值放大10倍传输
}

