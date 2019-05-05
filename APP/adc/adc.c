#include "adc.h"
#include "SysTick.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"

/*******************************************************************************
* 函 数 名         : ADCx_Init
* 函数功能		   : ADC初始化	
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void ADCx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量	
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//ADC
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	//模拟输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
	ADC_InitStructure.ADC_NbrOfChannel = 1;//1个转换在规则序列中 也就是只转换规则序列1 
	ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器
	
	ADC_ResetCalibration(ADC1);//重置指定的ADC的校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));//获取ADC重置校准寄存器的状态
	
	ADC_StartCalibration(ADC1);//开始指定ADC的校准状态
	while(ADC_GetCalibrationStatus(ADC1));//获取指定ADC的校准程序

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能或者失能指定的ADC的软件转换启动功能
}

/*******************************************************************************
* 函 数 名         : Get_ADC_Value
* 函数功能		   : 获取通道ch的转换值，取times次,然后平均 	
* 输    入         : ch:通道编号
					 times:获取次数
* 输    出         : 通道ch的times次转换结果平均值
*******************************************************************************/
u16 Get_ADC_Value(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADC通道,239个周期,提高采样时间可以提高精确度			    
	
	for(t=0;t<times;t++)
	{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能指定的ADC1的软件转换启动功能	
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
		temp_val+=ADC_GetConversionValue(ADC1);
		delay_ms(5);
	}
	return temp_val/times;
} 




#define ICON4_XS   ICON_XFRAM+3*ICON_XLEN+3*ICON_XSPACE
#define ICON4_YS   ICON_YFRAM
#define ICON4_XE   ICON_XFRAM+4*ICON_XLEN+3*ICON_XSPACE
#define ICON4_YE   ICON_YFRAM+ICON_YLEN

void LCD_DisplayWindows(void);

void ADC_Test(void)
{
	u16 adc_value;
	float adc_vol;
	u8 dat[6];
	
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//关闭RTC秒中断
	
	LCD_Clear(BLACK);
	ADCx_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"ADC测试");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	
	LCD_ShowFont12Char(10,50,"调节ADC模块内的电位器可以改变检测电压");	
	LCD_ShowFont12Char(10,80,"AD检测值是：");
	
	while(1)
	{
		adc_value=Get_ADC_Value(ADC_Channel_1,10);
		adc_vol=((float)adc_value*3.3)/4096;
		adc_value=(u16)(adc_vol*100);
		
		dat[0]=adc_value/100+0x30;
		dat[1]='.';
		dat[2]=adc_value%100/10+0x30;
		dat[3]=adc_value%100%10+0x30;
		dat[4]='V';
		dat[5]='\0';
		
		LCD_ShowFont12Char(107,80,dat);
		
		if(TouchData.lcdx>tftlcd_data.width-RETURN_X&&TouchData.lcdx<tftlcd_data.width&&TouchData.lcdy>tftlcd_data.height-RETURN_Y&&TouchData.lcdy<tftlcd_data.height)	
		{
			LCD_DisplayWindows();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//开启RTC秒中断
			break;					
		}
		TOUCH_Scan();
	}
}
