#include "key.h"
#include "SysTick.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"

/*******************************************************************************
* 函 数 名         : KEY_Init
* 函数功能		   : 按键初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=KEY_UP_Pin;	   //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//下拉输入  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	   //设置传输速率
	GPIO_Init(KEY_UP_Port,&GPIO_InitStructure);		  /* 初始化GPIO */
	
	GPIO_InitStructure.GPIO_Pin=KEY_DOWN_Pin|KEY_LEFT_Pin|KEY_RIGHT_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	//上拉输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(KEY_Port,&GPIO_InitStructure);
}

/*******************************************************************************
* 函 数 名         : KEY_Scan
* 函数功能		   : 按键扫描检测
* 输    入         : mode=0:单次按下按键
					 mode=1：连续按下按键
* 输    出         : 0：未有按键按下
					 KEY_UP：K_UP键按下
					 KEY_DOWN：K_DOWN键按下
					 KEY_LEFT：K_LEFT键按下
					 KEY_RIGHT：K_RIGHT键按下
*******************************************************************************/
u8 KEY_Scan(u8 mode)
{
	static u8 key=1;
	if(key==1&&(K_UP==1||K_DOWN==0||K_LEFT==0||K_RIGHT==0)) //任意一个按键按下
	{
		delay_ms(10);  //消抖
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
	else if(K_UP==0&&K_DOWN==1&&K_LEFT==1&&K_RIGHT==1)    //无按键按下
	{
		key=1;
	}
	if(mode==1) //连续按键按下
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
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//关闭RTC秒中断
	
	LCD_Clear(BLACK);
	KEY_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"按键测试");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	
	LCD_ShowFont12Char(10,40,"按下开发板上面的按键可在彩屏上的方块变换颜色");
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
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//开启RTC秒中断	
			break;					
		}
		TOUCH_Scan();
		
	}
}
