#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"
#include "key.h"
#include "sram.h"
#include "malloc.h" 
#include "sd.h"
#include "flash.h"
#include "ff.h" 
#include "fatfs_app.h"
#include "font_show.h"
#include "touch.h"
#include "gui.h"
#include "rtc.h"
#include "beep.h"
#include "adc.h"
#include "dac.h"
#include "ds18b20.h"
#include "24cxx.h"
#include "hwjs.h"
#include "sram.h"
#include "adxl345.h"
#include "rs485.h"
#include "can.h"
#include "smg.h"
#include "enc28j60.h"



#include "icon_background.h"
#include "icon_led.h"
#include "icon_beep.h"
#include "icon_key.h"
#include "icon_adc.h"
#include "icon_smg.h"
#include "icon_dac.h"
#include "icon_ds18b20.h"
#include "icon_eeprom.h"
#include "icon_ired.h"
#include "icon_flash.h"
#include "icon_sram.h"
#include "icon_adx345.h"
#include "icon_sd.h"
#include "icon_rs232.h"
#include "icon_rs485.h"
#include "icon_can.h"
#include "icon_internet.h"
#include "icon_map.h"
#include "icon_phone.h"
#include "icon_message.h"



#define ICON1_XS   ICON_XFRAM
#define ICON1_YS   ICON_YFRAM
#define ICON1_XE   ICON_XFRAM+ICON_XLEN
#define ICON1_YE   ICON_YFRAM+ICON_YLEN

#define ICON2_XS   ICON_XFRAM+ICON_XLEN+ICON_XSPACE
#define ICON2_YS   ICON_YFRAM
#define ICON2_XE   ICON_XFRAM+2*ICON_XLEN+ICON_XSPACE
#define ICON2_YE   ICON_YFRAM+ICON_YLEN

#define ICON3_XS   ICON_XFRAM+2*ICON_XLEN+2*ICON_XSPACE
#define ICON3_YS   ICON_YFRAM
#define ICON3_XE   ICON_XFRAM+3*ICON_XLEN+2*ICON_XSPACE
#define ICON3_YE   ICON_YFRAM+ICON_YLEN

#define ICON4_XS   ICON_XFRAM+3*ICON_XLEN+3*ICON_XSPACE
#define ICON4_YS   ICON_YFRAM
#define ICON4_XE   ICON_XFRAM+4*ICON_XLEN+3*ICON_XSPACE
#define ICON4_YE   ICON_YFRAM+ICON_YLEN



#define ICON5_XS   ICON_XFRAM
#define ICON5_YS   ICON_YFRAM+ICON_YSPACE+ICON_YLEN
#define ICON5_XE   ICON_XFRAM+ICON_XLEN
#define ICON5_YE   ICON_YFRAM+ICON_YSPACE+2*ICON_YLEN

#define ICON6_XS   ICON_XFRAM+ICON_XLEN+ICON_XSPACE
#define ICON6_YS   ICON_YFRAM+ICON_YSPACE+ICON_YLEN
#define ICON6_XE   ICON_XFRAM+2*ICON_XLEN+ICON_XSPACE
#define ICON6_YE   ICON_YFRAM+ICON_YSPACE+2*ICON_YLEN

#define ICON7_XS   ICON_XFRAM+2*ICON_XLEN+2*ICON_XSPACE      
#define ICON7_YS   ICON_YFRAM+ICON_YSPACE+ICON_YLEN
#define ICON7_XE   ICON_XFRAM+3*ICON_XLEN+2*ICON_XSPACE
#define ICON7_YE   ICON_YFRAM+ICON_YSPACE+2*ICON_YLEN

#define ICON8_XS   ICON_XFRAM+3*ICON_XLEN+3*ICON_XSPACE
#define ICON8_YS   ICON_YFRAM+ICON_YSPACE+ICON_YLEN
#define ICON8_XE   ICON_XFRAM+4*ICON_XLEN+3*ICON_XSPACE
#define ICON8_YE   ICON_YFRAM+ICON_YSPACE+2*ICON_YLEN



#define ICON9_XS   ICON_XFRAM
#define ICON9_YS   ICON_YFRAM+2*ICON_YSPACE+2*ICON_YLEN
#define ICON9_XE   ICON_XFRAM+ICON_XLEN
#define ICON9_YE   ICON_YFRAM+2*ICON_YSPACE+3*ICON_YLEN

#define ICON10_XS   ICON_XFRAM+ICON_XLEN+ICON_XSPACE
#define ICON10_YS   ICON_YFRAM+2*ICON_YSPACE+2*ICON_YLEN
#define ICON10_XE   ICON_XFRAM+2*ICON_XLEN+ICON_XSPACE
#define ICON10_YE   ICON_YFRAM+2*ICON_YSPACE+3*ICON_YLEN

#define ICON11_XS   ICON_XFRAM+2*ICON_XLEN+2*ICON_XSPACE
#define ICON11_YS   ICON_YFRAM+2*ICON_YSPACE+2*ICON_YLEN
#define ICON11_XE   ICON_XFRAM+3*ICON_XLEN+2*ICON_XSPACE
#define ICON11_YE   ICON_YFRAM+2*ICON_YSPACE+3*ICON_YLEN

#define ICON12_XS   ICON_XFRAM+3*ICON_XLEN+3*ICON_XSPACE
#define ICON12_YS   ICON_YFRAM+2*ICON_YSPACE+2*ICON_YLEN
#define ICON12_XE   ICON_XFRAM+4*ICON_XLEN+3*ICON_XSPACE
#define ICON12_YE   ICON_YFRAM+2*ICON_YSPACE+3*ICON_YLEN



#define ICON13_XS   ICON_XFRAM
#define ICON13_YS   ICON_YFRAM+3*ICON_YSPACE+3*ICON_YLEN
#define ICON13_XE   ICON_XFRAM+ICON_XLEN
#define ICON13_YE   ICON_YFRAM+3*ICON_YSPACE+4*ICON_YLEN

#define ICON14_XS   ICON_XFRAM+ICON_XLEN+ICON_XSPACE
#define ICON14_YS   ICON_YFRAM+3*ICON_YSPACE+3*ICON_YLEN
#define ICON14_XE   ICON_XFRAM+2*ICON_XLEN+ICON_XSPACE
#define ICON14_YE   ICON_YFRAM+3*ICON_YSPACE+4*ICON_YLEN

#define ICON15_XS   ICON_XFRAM+2*ICON_XLEN+2*ICON_XSPACE
#define ICON15_YS   ICON_YFRAM+3*ICON_YSPACE+3*ICON_YLEN
#define ICON15_XE   ICON_XFRAM+3*ICON_XLEN+2*ICON_XSPACE
#define ICON15_YE   ICON_YFRAM+3*ICON_YSPACE+4*ICON_YLEN

#define ICON16_XS   ICON_XFRAM+3*ICON_XLEN+3*ICON_XSPACE
#define ICON16_YS   ICON_YFRAM+3*ICON_YSPACE+3*ICON_YLEN
#define ICON16_XE   ICON_XFRAM+4*ICON_XLEN+3*ICON_XSPACE
#define ICON16_YE   ICON_YFRAM+3*ICON_YSPACE+4*ICON_YLEN



#define ICON17_XS   ICON_XFRAM
#define ICON17_YS   ICON_YFRAM+4*ICON_YSPACE+4*ICON_YLEN
#define ICON17_XE   ICON_XFRAM+ICON_XLEN
#define ICON17_YE   ICON_YFRAM+4*ICON_YSPACE+5*ICON_YLEN

#define ICON18_XS   ICON_XFRAM+ICON_XLEN+ICON_XSPACE
#define ICON18_YS   ICON_YFRAM+4*ICON_YSPACE+4*ICON_YLEN
#define ICON18_XE   ICON_XFRAM+2*ICON_XLEN+ICON_XSPACE
#define ICON18_YE   ICON_YFRAM+4*ICON_YSPACE+5*ICON_YLEN

#define ICON19_XS   ICON_XFRAM+2*ICON_XLEN+2*ICON_XSPACE
#define ICON19_YS   ICON_YFRAM+4*ICON_YSPACE+4*ICON_YLEN
#define ICON19_XE   ICON_XFRAM+3*ICON_XLEN+2*ICON_XSPACE
#define ICON19_YE   ICON_YFRAM+4*ICON_YSPACE+5*ICON_YLEN

#define ICON20_XS   ICON_XFRAM+3*ICON_XLEN+3*ICON_XSPACE
#define ICON20_YS   ICON_YFRAM+4*ICON_YSPACE+4*ICON_YLEN
#define ICON20_XE   ICON_XFRAM+4*ICON_XLEN+3*ICON_XSPACE
#define ICON20_YE   ICON_YFRAM+4*ICON_YSPACE+5*ICON_YLEN





void LCD_DisplayWindows(void)
{

	//LCD_ShowPicture(0,0,tftlcd_data.width,tftlcd_data.height,(u8 *)gImage_back);
	LCD_Clear(LIGHTBLUE);
	
	LCD_ShowPicture(ICON1_XS,ICON1_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_led);
	LCD_ShowStringTrans(ICON1_XS+(ICON_XLEN-3*6)/2,ICON1_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"LED");
	
	LCD_ShowPicture(ICON2_XS,ICON2_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_beep);
	LCD_ShowStringTrans(ICON2_XS+(ICON_XLEN-4*6)/2,ICON2_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"BEEP");
	
	LCD_ShowPicture(ICON3_XS,ICON3_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_key);
	LCD_ShowStringTrans(ICON3_XS+(ICON_XLEN-3*6)/2,ICON3_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"KEY");
	
	LCD_ShowPicture(ICON4_XS,ICON4_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_dac);
	LCD_ShowStringTrans(ICON4_XS+(ICON_XLEN-3*6)/2,ICON4_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"ADC");
	
	LCD_ShowPicture(ICON5_XS,ICON5_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_smg);
	LCD_ShowStringTrans(ICON5_XS+(ICON_XLEN-3*6)/2,ICON5_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"SMG");
	
	LCD_ShowPicture(ICON6_XS,ICON6_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_ds18b20);
	LCD_ShowStringTrans(ICON6_XS+(ICON_XLEN-5*6)/2,ICON6_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"18B20");
	
	LCD_ShowPicture(ICON7_XS,ICON7_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_eeprom);
	LCD_ShowStringTrans(ICON7_XS+(ICON_XLEN-6*6)/2,ICON7_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"EEPROM");
	
	LCD_ShowPicture(ICON8_XS,ICON8_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_ired);
	LCD_ShowStringTrans(ICON8_XS+(ICON_XLEN-4*6)/2,ICON8_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"IRED");
	
	LCD_ShowPicture(ICON9_XS,ICON9_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_flash);
	LCD_ShowStringTrans(ICON9_XS+(ICON_XLEN-5*6)/2,ICON9_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"FLASH");
	
	LCD_ShowPicture(ICON10_XS,ICON10_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_sram);
	LCD_ShowStringTrans(ICON10_XS+(ICON_XLEN-4*6)/2,ICON10_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"SRAM");
	
	LCD_ShowPicture(ICON11_XS,ICON11_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_adx345);
	LCD_ShowStringTrans(ICON11_XS+(ICON_XLEN-6*6)/2,ICON11_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"ADX345");
	
	LCD_ShowPicture(ICON12_XS,ICON12_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_sd);
	LCD_ShowStringTrans(ICON12_XS+(ICON_XLEN-2*6)/2,ICON12_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"SD");
	
	LCD_ShowPicture(ICON13_XS,ICON13_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_rs232);
	LCD_ShowStringTrans(ICON13_XS+(ICON_XLEN-5*6)/2,ICON13_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"RS232");
	
	LCD_ShowPicture(ICON14_XS,ICON14_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_rs485);
	LCD_ShowStringTrans(ICON14_XS+(ICON_XLEN-5*6)/2,ICON14_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"RS485");
	
	LCD_ShowPicture(ICON15_XS,ICON15_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_can);
	LCD_ShowStringTrans(ICON15_XS+(ICON_XLEN-3*6)/2,ICON15_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"CAN");
	
	LCD_ShowPicture(ICON16_XS,ICON16_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_internet);
	LCD_ShowStringTrans(ICON16_XS+(ICON_XLEN-5*6)/2,ICON16_YS+ICON_YLEN+(ICON_YSPACE-8)/2,100,20,12,"Inter");
	
	LCD_ShowPicture(ICON17_XS,ICON17_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_map);
	LCD_ShowPicture(ICON18_XS,ICON18_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_phone);
	LCD_ShowPicture(ICON19_XS,ICON19_YS,ICON_SIZE,ICON_SIZE,(u8 *)gImage_message);
	
}


int main()
{
	u8 start=0;
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	USART1_Init(9600);
	LED_Init();
	KEY_Init();
	TFTLCD_Init();			//LCD初始化
	TOUCH_Init();
	
	EN25QXX_Init();				//初始化EN25QXX 
	FSMC_SRAM_Init(); 
	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部SRAM内存池
	
	LCD_DisplayWindows();
	RTC_Init();
	
	
	while(1)
	{
		if(KEY_Scan(0)==KEY_UP)
		{
			RTC_ITConfig(RTC_IT_SEC, DISABLE);//关闭RTC秒中断
			TOUCH_Adjust();
			LCD_DisplayWindows();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//开启RTC秒中断
		}
		if(TOUCH_Scan() == 0)
		{		
			start=1;
		}	
		if(start==1)
		{
			if(TouchData.lcdx>ICON1_XS&&TouchData.lcdx<ICON1_XE&&TouchData.lcdy>ICON1_YS&&TouchData.lcdy<ICON1_YE)	
			{
				LED_Test();
			}
			if(TouchData.lcdx>ICON2_XS&&TouchData.lcdx<ICON2_XE&&TouchData.lcdy>ICON2_YS&&TouchData.lcdy<ICON2_YE)	
			{
				BEEP_Test();
			}
			if(TouchData.lcdx>ICON3_XS&&TouchData.lcdx<ICON3_XE&&TouchData.lcdy>ICON3_YS&&TouchData.lcdy<ICON3_YE)	
			{
				KEY_Test();
			}
			if(TouchData.lcdx>ICON4_XS&&TouchData.lcdx<ICON4_XE&&TouchData.lcdy>ICON4_YS&&TouchData.lcdy<ICON4_YE)	
			{
				ADC_Test();
			}
			if(TouchData.lcdx>ICON5_XS&&TouchData.lcdx<ICON5_XE&&TouchData.lcdy>ICON5_YS&&TouchData.lcdy<ICON5_YE)	
			{
				SMG_Test();
			}
			if(TouchData.lcdx>ICON6_XS&&TouchData.lcdx<ICON6_XE&&TouchData.lcdy>ICON6_YS&&TouchData.lcdy<ICON6_YE)	
			{
				DS18B20_Test();
			}
			if(TouchData.lcdx>ICON7_XS&&TouchData.lcdx<ICON7_XE&&TouchData.lcdy>ICON7_YS&&TouchData.lcdy<ICON7_YE)	
			{
				EEPROM_Test();
			}
			if(TouchData.lcdx>ICON8_XS&&TouchData.lcdx<ICON8_XE&&TouchData.lcdy>ICON8_YS&&TouchData.lcdy<ICON8_YE)	
			{
				IRED_Test();
			}
			if(TouchData.lcdx>ICON9_XS&&TouchData.lcdx<ICON9_XE&&TouchData.lcdy>ICON9_YS&&TouchData.lcdy<ICON9_YE)	
			{
				FLASH_Test();
			}
			if(TouchData.lcdx>ICON10_XS&&TouchData.lcdx<ICON10_XE&&TouchData.lcdy>ICON10_YS&&TouchData.lcdy<ICON10_YE)	
			{
				SRAM_Test();
			}
			if(TouchData.lcdx>ICON11_XS&&TouchData.lcdx<ICON11_XE&&TouchData.lcdy>ICON11_YS&&TouchData.lcdy<ICON11_YE)	
			{
				ADX345_Test();
			}
			if(TouchData.lcdx>ICON12_XS&&TouchData.lcdx<ICON12_XE&&TouchData.lcdy>ICON12_YS&&TouchData.lcdy<ICON12_YE)	
			{
				SD_Test();
			}
			if(TouchData.lcdx>ICON13_XS&&TouchData.lcdx<ICON13_XE&&TouchData.lcdy>ICON13_YS&&TouchData.lcdy<ICON13_YE)	
			{
				USART3_Test();
			}
			if(TouchData.lcdx>ICON14_XS&&TouchData.lcdx<ICON14_XE&&TouchData.lcdy>ICON14_YS&&TouchData.lcdy<ICON14_YE)	
			{
				RS485_Test();
			}
			if(TouchData.lcdx>ICON15_XS&&TouchData.lcdx<ICON15_XE&&TouchData.lcdy>ICON15_YS&&TouchData.lcdy<ICON15_YE)	
			{
				CAN_Test();
			}
			if(TouchData.lcdx>ICON16_XS&&TouchData.lcdx<ICON16_XE&&TouchData.lcdy>ICON16_YS&&TouchData.lcdy<ICON16_YE)	
			{
				Internet_Test();
			}
			start=0;
		}			
	}
	
}
