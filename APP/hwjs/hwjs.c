#include "hwjs.h"
#include "SysTick.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"


u32 hw_jsm;	  //定义一个32位数据变量，保存接收码
u8  hw_jsbz;  //定义一个8位数据的变量，用于指示接收标志


/*******************************************************************************
* 函 数 名         : Hwjs_Init
* 函数功能		   : 红外端口初始化函数	  时钟端口及外部中断初始化 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Hwjs_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 开启GPIO时钟及管脚复用时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;//红外接收
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource15); //选择GPIO管脚用作外部中断线路
	EXTI_ClearITPendingBit(EXTI_Line15);
	
	/* 设置外部中断的模式 */ 
	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

	/* 设置NVIC参数 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;   //打开全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 	 //响应优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   //使能
	NVIC_Init(&NVIC_InitStructure);

}


/*******************************************************************************
* 函 数 名         : HW_jssj
* 函数功能		   : 高电平持续时间，将记录的时间保存在t中返回，其中一次大约20us 
* 输    入         : 无
* 输    出         : t
*******************************************************************************/
u8 HW_jssj()
{
	u8 t=0;
	while(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_15)==1)//高电平
	{
		t++;
		delay_us(20);
		if(t>=250) return t;//超时溢出
	}
	return t;
}


void EXTI15_10_IRQHandler(void)	  //红外遥控外部中断
{
	u8 Tim=0,Ok=0,Data,Num=0;

   while(1)
   {
	   	if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_15)==1)
		{
			 Tim=HW_jssj();//获得此次高电平时间

			 if(Tim>=250) break;//不是有用的信号

			 if(Tim>=200 && Tim<250)
			 {
			 	Ok=1;//收到起始信号
			 }
			 else if(Tim>=60 && Tim<90)
			 {
			 	Data=1;//收到数据 1
			 }
			 else if(Tim>=10 && Tim<50)
			 {
			 	Data=0;//收到数据 0
			 }

			 if(Ok==1)
			 {
			 	hw_jsm<<=1;
				hw_jsm+=Data;

				if(Num>=32)
				{
					hw_jsbz=1;
				  	break;
				}
			 }

			 Num++;
		}
   }

   EXTI_ClearITPendingBit(EXTI_Line15);	
}



#define ICON8_XS   ICON_XFRAM+3*ICON_XLEN+3*ICON_XSPACE
#define ICON8_YS   ICON_YFRAM+ICON_YSPACE+ICON_YLEN
#define ICON8_XE   ICON_XFRAM+4*ICON_XLEN+3*ICON_XSPACE
#define ICON8_YE   ICON_YFRAM+ICON_YSPACE+2*ICON_YLEN

void LCD_DisplayWindows(void);
void IRED_Test(void)
{
	u8 dat[4],irdisp[9];
	u8 num[16]="0123456789ABCDEF";
	
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//关闭RTC秒中断
	
	LCD_Clear(BLACK);
	Hwjs_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"红外遥控测试");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	LCD_ShowFont12Char(10,50,"红外遥控键码：");
	
	while(1)
	{

		if(hw_jsbz==1)	//如果红外接收到
		{
			hw_jsbz=0;	   //清零
			dat[0]=hw_jsm>>24;
			dat[1]=(hw_jsm>>16)&0xff;
			dat[2]=(hw_jsm>>8)&0xff;
			dat[3]=hw_jsm&0xff;
			hw_jsm=0;					//接收码清零
			irdisp[0]=num[dat[0]/16];//显示高位
			irdisp[1]=num[dat[0]%16];//显示低位  
			irdisp[2]=num[dat[1]/16];
			irdisp[3]=num[dat[1]%16];	 //同一个遥控器此2个字节的引导码数据是不会改变的，改变的只是数据码及反码
			irdisp[4]=num[dat[2]/16];
			irdisp[5]=num[dat[2]%16];
			irdisp[6]=num[dat[3]/16];
			irdisp[7]=num[dat[3]%16];
			irdisp[8]='\0';
			LCD_ShowFont12Char(131,50,irdisp);
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
