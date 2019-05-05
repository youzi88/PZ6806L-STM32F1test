#include "beep.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"
#include "SysTick.h"


void BEEP_Init()	  //端口初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;	//声明一个结构体变量，用来初始化GPIO

	RCC_APB2PeriphClockCmd(BEEP_PORT_RCC,ENABLE);   /* 开启GPIO时钟 */

	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=BEEP_PIN;		//选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		  //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(BEEP_PORT,&GPIO_InitStructure); 	 /* 初始化GPIO */
}

#define ICON2_XS   ICON_XFRAM+ICON_XLEN+ICON_XSPACE
#define ICON2_YS   ICON_YFRAM
#define ICON2_XE   ICON_XFRAM+2*ICON_XLEN+ICON_XSPACE
#define ICON2_YE   ICON_YFRAM+ICON_YLEN

void LCD_DisplayWindows(void);

void BEEP_Test(void)
{
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//关闭RTC秒中断
	
	LCD_Clear(BLACK);
	BEEP_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"蜂鸣器测试");
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
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//开启RTC秒中断
			break;					
		}
		TOUCH_Scan();
	}
}

