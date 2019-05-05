#include "smg.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "SysTick.h"
#include "gui.h"



void SMG_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;  //声明一个结构体变量，用来初始化GPIO
	/* 开启GPIO时钟 */
	RCC_APB2PeriphClockCmd(SMG_PORT_RCC,ENABLE);

	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=SMG_PIN;	  //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(SMG_PORT,&GPIO_InitStructure);		/* 初始化GPIO */
}



#define ICON5_XS   ICON_XFRAM
#define ICON5_YS   ICON_YFRAM+ICON_YSPACE+ICON_YLEN
#define ICON5_XE   ICON_XFRAM+ICON_XLEN
#define ICON5_YE   ICON_YFRAM+ICON_YSPACE+2*ICON_YLEN

void LCD_DisplayWindows(void);

u8 smgduan[16]={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
             0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};//0~F 数码管段选数据

void SMG_Test(void)
{
	u8 i,j;
	
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//关闭RTC秒中断
	
	LCD_Clear(BLACK);
	SMG_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"数码管测试");
	LCD_ShowFont12Char(10,40,"数码管循环显示0-F");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	
	
	while(1)
	{
		GPIO_Write(SMG_PORT,(u16)(~smgduan[i]));
		if(j>=30)
		{
			j=0;
			i++;
			if(i==16)i=0;
		}
		j++;
		delay_ms(10);
		
		if(TouchData.lcdx>tftlcd_data.width-RETURN_X&&TouchData.lcdx<tftlcd_data.width&&TouchData.lcdy>tftlcd_data.height-RETURN_Y&&TouchData.lcdy<tftlcd_data.height)	
		{
			GPIO_SetBits(SMG_PORT,SMG_PIN);
			LCD_DisplayWindows();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//开启RTC秒中断	
			break;					
		}
		TOUCH_Scan();
		
	}
}

