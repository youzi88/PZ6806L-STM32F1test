#include "adc.h"
#include "SysTick.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"

/*******************************************************************************
* �� �� ��         : ADCx_Init
* ��������		   : ADC��ʼ��	
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void ADCx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����	
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//ADC
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	//ģ������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
	ADC_InitStructure.ADC_NbrOfChannel = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
	ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
	ADC_Cmd(ADC1, ENABLE);//����ADת����
	
	ADC_ResetCalibration(ADC1);//����ָ����ADC��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡADC����У׼�Ĵ�����״̬
	
	ADC_StartCalibration(ADC1);//��ʼָ��ADC��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));//��ȡָ��ADC��У׼����

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ�ܻ���ʧ��ָ����ADC�����ת����������
}

/*******************************************************************************
* �� �� ��         : Get_ADC_Value
* ��������		   : ��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 	
* ��    ��         : ch:ͨ�����
					 times:��ȡ����
* ��    ��         : ͨ��ch��times��ת�����ƽ��ֵ
*******************************************************************************/
u16 Get_ADC_Value(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��,239������,��߲���ʱ�������߾�ȷ��			    
	
	for(t=0;t<times;t++)
	{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ��ָ����ADC1�����ת����������	
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
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
	
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//�ر�RTC���ж�
	
	LCD_Clear(BLACK);
	ADCx_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"ADC����");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	
	LCD_ShowFont12Char(10,50,"����ADCģ���ڵĵ�λ�����Ըı����ѹ");	
	LCD_ShowFont12Char(10,80,"AD���ֵ�ǣ�");
	
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
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//����RTC���ж�
			break;					
		}
		TOUCH_Scan();
	}
}
