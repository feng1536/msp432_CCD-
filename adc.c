///****************************************************/
////MSP432P401R
////ADC采集
////Bilibili：m-RNA
////E-mail:m-RNA@qq.com
////创建日期:2021/9/13
///****************************************************/

///*********************
// *
// * 最大采集电压 3.3V
// *
// * ADC采集引脚：
// * 单路 为 P5.5
// * 双路 为 P5.5 P5.4
// * 三路 为 P5.5 P5.4 P5.3
// *
// ************************/
// 

// 
////总时间  M*N*21us
//#define N 200 //采样次数
//#define M 1   //采样通道个数


#include "adc.h"

#define N 200 //采样次数
#define M 1   //采样通道个数

volatile uint_fast16_t resultsBuffer[N];
volatile uint8_t resPos;

void ADC_Config(void)
{
//	/* 启用浮点运算的FPU */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();
	
//![Single Sample Mode Configure]
    /* Initializing ADC (MCLK/1/4) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4,
            0);
            
    /* Configuring GPIOs (5.5 A0) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5,
    GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory */
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);//多次转换
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
    ADC_INPUT_A0, false);//系统电源电压

    /* Configuring Sample Timer */
    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
		
    /* Enabling interrupts */
    MAP_ADC14_enableInterrupt(ADC_INT0);
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    MAP_Interrupt_enableMaster();
		
    /* Enabling/Toggling Conversion */
    MAP_ADC14_enableConversion();
    MAP_ADC14_toggleConversionTrigger();//触发第一次
    //![Single Sample Mode Configure]
		MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4);
		MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7);

}

uint16_t ADV[128];
void RD_TSL(void)
{
	uint8_t i=0,tslp=0;
	
	TSL_CLK_LOW;
  TSL_SI_LOW; 
  delay_us(20);

  TSL_SI_HIGH ;
  delay_us(20);	
	
  TSL_CLK_LOW;
  delay_us(20);
	
  TSL_CLK_HIGH;
	delay_us(20); 
	
  TSL_SI_LOW;
  delay_us(20); 
  for(i=0;i<128;i++)
  { 
    TSL_CLK_LOW; 
    delay_us(20);  //调节曝光时间
		MAP_ADC14_toggleConversionTrigger();
		delay_us(20);
    ADV[tslp]=(resultsBuffer[0])>>4;
    ++tslp;
    TSL_CLK_HIGH;
    delay_us(20);

  }  
	
	
}

int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}


uint16_t i,j,Left,Right,Last_CCD_Zhongzhi=70,CCD_Zhongzhi,CCD_Yuzhi,CCD_Width;
uint16_t value1_max,value1_min;
void CCD_Find_Center(void)
{

	
	
	   value1_max=ADV[0];  //动态阈值算法，读取最大和最小值
     for(i=14;i<123;i++)   //两边各去掉5个点
     {
        if(value1_max<=ADV[i])
        value1_max=ADV[i];
     }
	   value1_min=ADV[0];  //最小值
     for(i=5;i<123;i++) 
     {
        if(value1_min>=ADV[i])
        value1_min=ADV[i];
     }
   CCD_Yuzhi=(value1_max+value1_min)/2;	  //计算出本次中线提取的阈值
		
	 for(i = 14;i<118; i++)   //寻找左边跳变沿
  {
		if(ADV[i-2]>CCD_Yuzhi&&ADV[i-1]>CCD_Yuzhi&&ADV[i]>CCD_Yuzhi&&ADV[i+1]<CCD_Yuzhi&&ADV[i+2]<CCD_Yuzhi&&ADV[i+3]<CCD_Yuzhi)
		{	
		  Left=i;
		  break;	
		}
  }
	 for(j = 118;j>14; j--)//寻找右边跳变沿
  {
		if(ADV[j-2]<CCD_Yuzhi&&ADV[j-1]<CCD_Yuzhi&&ADV[j]<CCD_Yuzhi&&ADV[j+1]>CCD_Yuzhi&&ADV[j+2]>CCD_Yuzhi&&ADV[j+3]>CCD_Yuzhi)
		{	
		  Right=j;
		  break;	
		}
  }
	
//		 for(i = 10;i<113; i++)   //寻找左边跳变沿
//  {
//		if(ADV[i]>CCD_Yuzhi&&ADV[i+1]>CCD_Yuzhi&&ADV[i+2]>CCD_Yuzhi&&
//			 ADV[i+3]<CCD_Yuzhi&&ADV[i+4]<CCD_Yuzhi&&ADV[i+5]<CCD_Yuzhi&&ADV[i+6]<CCD_Yuzhi&&ADV[i+7]<CCD_Yuzhi&&ADV[i+8]<CCD_Yuzhi)
//		{	
//		  Left=i;
//		  break;	
//		}
//  }
//	 for(j = 113;j>10; j--)//寻找右边跳变沿
//  {
//		if(ADV[j]<CCD_Yuzhi&&ADV[j+1]<CCD_Yuzhi&&ADV[j+2]<CCD_Yuzhi&&ADV[j+3]<CCD_Yuzhi&&ADV[j+4]<CCD_Yuzhi&&ADV[j+5]<CCD_Yuzhi&&
//			ADV[j+6]<CCD_Yuzhi&&ADV[j+7]<CCD_Yuzhi&&ADV[j+8]<CCD_Yuzhi
//			)
//		{	
//		  Right=j;
//		  break;	
//		}
//  }
	CCD_Width = Right-Left;
	CCD_Zhongzhi=(Right+Left)/2;//计算中线位置
	if(myabs(CCD_Zhongzhi-Last_CCD_Zhongzhi)>70)   //计算中线的偏差，如果太大
	CCD_Zhongzhi=Last_CCD_Zhongzhi;    //则取上一次的值
	Last_CCD_Zhongzhi=CCD_Zhongzhi;  //保存上一次的偏差
	
}

void ADC14_IRQHandler(void)
{
    uint_fast64_t status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    if (ADC_INT0 & status)
		{
//			for(resPos= 0;resPos<N;resPos++)
	//		{
        resultsBuffer[resPos] = MAP_ADC14_getResult(ADC_MEM0);
//			delay_us(100);
//      normalizedADCRes = (curADCResult * 3.3) / 16384;
//			} 
		}
				
}
