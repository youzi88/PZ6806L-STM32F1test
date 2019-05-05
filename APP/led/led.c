#include "led.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "SysTick.h"
#include "gui.h"


/*******************************************************************************
* 函 数 名         : LED_Init
* 函数功能		   : LED初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	
	RCC_APB2PeriphClockCmd(LED_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=LED_PIN;  //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(LED_PORT,&GPIO_InitStructure); 	   /* 初始化GPIO */
	
	GPIO_SetBits(LED_PORT,LED_PIN);   //将LED端口拉高，熄灭所有LED
}


#define ICON1_XS   ICON_XFRAM
#define ICON1_YS   ICON_YFRAM
#define ICON1_XE   ICON_XFRAM+ICON_XLEN
#define ICON1_YE   ICON_YFRAM+ICON_YLEN
void LCD_DisplayWindows(void);


void LED_Test(void)
{
	u8 i;
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//关闭RTC秒中断
	
	LCD_Clear(BLACK);
	LED_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"LED灯测试");
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
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//开启RTC秒中断
			break;					
		}
		TOUCH_Scan();
	}
	
}

