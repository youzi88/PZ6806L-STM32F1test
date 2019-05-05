#include "led.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "SysTick.h"
#include "gui.h"


/*******************************************************************************
* �� �� ��         : LED_Init
* ��������		   : LED��ʼ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;//����ṹ�����
	
	RCC_APB2PeriphClockCmd(LED_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=LED_PIN;  //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(LED_PORT,&GPIO_InitStructure); 	   /* ��ʼ��GPIO */
	
	GPIO_SetBits(LED_PORT,LED_PIN);   //��LED�˿����ߣ�Ϩ������LED
}


#define ICON1_XS   ICON_XFRAM
#define ICON1_YS   ICON_YFRAM
#define ICON1_XE   ICON_XFRAM+ICON_XLEN
#define ICON1_YE   ICON_YFRAM+ICON_YLEN
void LCD_DisplayWindows(void);


void LED_Test(void)
{
	u8 i;
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//�ر�RTC���ж�
	
	LCD_Clear(BLACK);
	LED_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"LED�Ʋ���");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	while(1)
	{	
		if(i<=10)
		{
			GPIO_SetBits(LED_PORT,LED_PIN);
		}
		if(i>=20&&i<40)
		{
			GPIO_ResetBits(LED_PORT,LED_PIN);	
		}
		if(i>=40) i=0;		
		delay_ms(10);
		i++;				
		if(TouchData.lcdx>tftlcd_data.width-RETURN_X&&TouchData.lcdx<tftlcd_data.width&&TouchData.lcdy>tftlcd_data.height-RETURN_Y&&TouchData.lcdy<tftlcd_data.height)	
		{
			GPIO_SetBits(LED_PORT,LED_PIN);
			LCD_DisplayWindows();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//����RTC���ж�
			break;					
		}
		TOUCH_Scan();
	}
	
}

