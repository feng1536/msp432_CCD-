/****************************************************/
//MSP432P401R
//ADC采集
//Bilibili：m-RNA
//E-mail:m-RNA@qq.com
//创建日期:2021/9/13
/****************************************************/

#ifndef __ADC_H
#define __ADC_H
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "usart.h"
#include "delay.h"

#define TSL_SI_LOW 	MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN4)
#define TSL_CLK_LOW MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN7)

#define TSL_SI_HIGH 	MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN4)
#define TSL_CLK_HIGH 	MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN7)

#define N 200 //采样次数
#define M 1   //采样通道个数

void ADC_Config(void);
void RD_TSL(void);
int myabs(int a);
void CCD_Find_Center(void);

typedef struct 
{
	int16_t error;							//误差
	int16_t error_last;				//上次误差
	int16_t error_division;		//误差微分
	int16_t error_intergrate;	//误差积分
}CCD_Data_stru;

extern uint16_t i,j,Left,Right,Last_CCD_Zhongzhi,CCD_Zhongzhi,CCD_Yuzhi,CCD_Width;
extern uint16_t value1_max,value1_min;

extern volatile uint_fast16_t resultsBuffer[N];
extern volatile uint8_t resPos;
extern uint16_t ADV[128];
#endif
