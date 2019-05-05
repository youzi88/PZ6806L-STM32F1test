#include "key.h"
#include "SysTick.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"

/*******************************************************************************
* �� �� ��         : KEY_Init
* ��������		   : ������ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=KEY_UP_Pin;	   //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//��������  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	   //���ô�������
	GPIO_Init(KEY_UP_Port,&GPIO_InitStructure);		  /* ��ʼ��GPIO */
	
	GPIO_InitStructure.GPIO_Pin=KEY_DOWN_Pin|KEY_LEFT_Pin|KEY_RIGHT_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	//��������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(KEY_Port,&GPIO_InitStructure);
}

/*******************************************************************************
* �� �� ��         : KEY_Scan
* ��������		   : ����ɨ����
* ��    ��         : mode=0:���ΰ��°���
					 mode=1���������°���
* ��    ��         : 0��δ�а�������
					 KEY_UP��K_UP������
					 KEY_DOWN��K_DOWN������
					 KEY_LEFT��K_LEFT������
					 KEY_RIGHT��K_RIGHT������
*******************************************************************************/
u8 KEY_Scan(u8 mode)
{
	static u8 key=1;
	if(key==1&&(K_UP==1||K_DOWN==0||K_LEFT==0||K_RIGHT==0)) //����һ����������
	{
		delay_ms(10);  //����
		key=0;
		if(K_UP==1)
		{
			return KEY_UP; 
		}
		else if(K_DOWN==0)
		{
			return KEY_DOWN; 
		}
		else if(K_LEFT==0)
		{
			return KEY_LEFT; 
		}
		else
		{
			return KEY_RIGHT;
		}
	}
	else if(K_UP==0&&K_DOWN==1&&K_LEFT==1&&K_RIGHT==1)    //�ް�������
	{
		key=1;
	}
	if(mode==1) //������������
	{
		key=1;
	}
	return 0;
}


#define ICON3_XS   ICON_XFRAM+2*ICON_XLEN+2*ICON_XSPACE
#define ICON3_YS   ICON_YFRAM
#define ICON3_XE   ICON_XFRAM+3*ICON_XLEN+2*ICON_XSPACE
#define ICON3_YE   ICON_YFRAM+ICON_YLEN

void LCD_DisplayWindows(void);

void KEY_Test(void)
{
	u8 key;
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//�ر�RTC���ж�
	
	LCD_Clear(BLACK);
	KEY_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"��������");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	
	LCD_ShowFont12Char(10,40,"���¿���������İ������ڲ����ϵķ���任��ɫ");
	LCD_Fill(10, 180, 70,220, YELLOW);
	LCD_Fill(90, 180, 150,220, YELLOW);
	LCD_Fill(10, 240, 70,280, YELLOW);	
	LCD_Fill(90, 240, 150,280, YELLOW);
	
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY_UP:     LCD_Fill(10, 180, 70,220, BLUE);break;
			case KEY_DOWN:   LCD_Fill(90, 180, 150,220, BLUE);break;
			case KEY_LEFT:   LCD_Fill(10, 240, 70,280, BLUE);	break;
			case KEY_RIGHT:  LCD_Fill(90, 240, 150,280, BLUE);break;
		}
		if(TouchData.lcdx>tftlcd_data.width-RETURN_X&&TouchData.lcdx<tftlcd_data.width&&TouchData.lcdy>tftlcd_data.height-RETURN_Y&&TouchData.lcdy<tftlcd_data.height)	
		{
			LCD_DisplayWindows();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//����RTC���ж�	
			break;					
		}
		TOUCH_Scan();
		
	}
}
