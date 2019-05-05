#include "beep.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"
#include "SysTick.h"


void BEEP_Init()	  //�˿ڳ�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;	//����һ���ṹ�������������ʼ��GPIO

	RCC_APB2PeriphClockCmd(BEEP_PORT_RCC,ENABLE);   /* ����GPIOʱ�� */

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=BEEP_PIN;		//ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		  //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(BEEP_PORT,&GPIO_InitStructure); 	 /* ��ʼ��GPIO */
}

#define ICON2_XS   ICON_XFRAM+ICON_XLEN+ICON_XSPACE
#define ICON2_YS   ICON_YFRAM
#define ICON2_XE   ICON_XFRAM+2*ICON_XLEN+ICON_XSPACE
#define ICON2_YE   ICON_YFRAM+ICON_YLEN

void LCD_DisplayWindows(void);

void BEEP_Test(void)
{
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//�ر�RTC���ж�
	
	LCD_Clear(BLACK);
	BEEP_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"����������");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	
	while(1)
	{
		GPIO_SetBits(BEEP_PORT,BEEP_PIN);
		delay_ms(1);
		GPIO_ResetBits(BEEP_PORT,BEEP_PIN);
		delay_ms(1);
		
		if(TouchData.lcdx>tftlcd_data.width-RETURN_X&&TouchData.lcdx<tftlcd_data.width&&TouchData.lcdy>tftlcd_data.height-RETURN_Y&&TouchData.lcdy<tftlcd_data.height)	
		{
			beep=0;
			LCD_DisplayWindows();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//����RTC���ж�
			break;					
		}
		TOUCH_Scan();
	}
}

