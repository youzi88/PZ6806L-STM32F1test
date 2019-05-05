#include "ds18b20.h"
#include "SysTick.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"

/*******************************************************************************
* 函 数 名         : DS18B20_IO_IN
* 函数功能		   : DS18B20_IO输入配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void DS18B20_IO_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=DS18B20_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
}

/*******************************************************************************
* 函 数 名         : DS18B20_IO_OUT
* 函数功能		   : DS18B20_IO输出配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void DS18B20_IO_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=DS18B20_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
}

/*******************************************************************************
* 函 数 名         : DS18B20_Reset
* 函数功能		   : 复位DS18B20  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void DS18B20_Reset(void)	   
{                 
	DS18B20_IO_OUT(); //SET PG11 OUTPUT
	DS18B20_DQ_OUT=0; //拉低DQ
	delay_us(750);    //拉低750us
	DS18B20_DQ_OUT=1; //DQ=1 
	delay_us(15);     //15US
}

/*******************************************************************************
* 函 数 名         : DS18B20_Check
* 函数功能		   : 检测DS18B20是否存在
* 输    入         : 无
* 输    出         : 1:未检测到DS18B20的存在，0:存在
*******************************************************************************/
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PG11 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}

/*******************************************************************************
* 函 数 名         : DS18B20_Read_Bit
* 函数功能		   : 从DS18B20读取一个位
* 输    入         : 无
* 输    出         : 1/0
*******************************************************************************/
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
	u8 data;
	DS18B20_IO_OUT();//SET PG11 OUTPUT
	DS18B20_DQ_OUT=0; 
	delay_us(2);
	DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();//SET PG11 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
	else data=0;	 
	delay_us(50);           
	return data;
}

/*******************************************************************************
* 函 数 名         : DS18B20_Read_Byte
* 函数功能		   : 从DS18B20读取一个字节
* 输    入         : 无
* 输    出         : 一个字节数据
*******************************************************************************/
//
//返回值：读到的数据
u8 DS18B20_Read_Byte(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}

/*******************************************************************************
* 函 数 名         : DS18B20_Write_Byte
* 函数功能		   : 写一个字节到DS18B20
* 输    入         : dat：要写入的字节
* 输    出         : 无
*******************************************************************************/
void DS18B20_Write_Byte(u8 dat)     
{             
	u8 j;
    u8 testb;
	DS18B20_IO_OUT();//SET PG11 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT=0;// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT=1;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT=0;// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}

/*******************************************************************************
* 函 数 名         : DS18B20_Start
* 函数功能		   : 开始温度转换
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Reset();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 

/*******************************************************************************
* 函 数 名         : DS18B20_Init
* 函数功能		   : 初始化DS18B20的IO口 DQ 同时检测DS的存在
* 输    入         : 无
* 输    出         : 1:不存在，0:存在
*******************************************************************************/   	 
u8 DS18B20_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(DS18B20_PORT_RCC,ENABLE);

	GPIO_InitStructure.GPIO_Pin=DS18B20_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
 
 	DS18B20_Reset();
	return DS18B20_Check();
}  

/*******************************************************************************
* 函 数 名         : DS18B20_GetTemperture
* 函数功能		   : 从ds18b20得到温度值
* 输    入         : 无
* 输    出         : 温度数据
*******************************************************************************/ 
float DS18B20_GetTemperture(void)
{
    u16 temp;
	u8 a,b;
	float value;
    DS18B20_Start();                    // ds1820 start convert
    DS18B20_Reset();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert	    
    a=DS18B20_Read_Byte(); // LSB   
    b=DS18B20_Read_Byte(); // MSB   
	temp=b;
	temp=(temp<<8)+a;
    if((temp&0xf800)==0xf800)
	{
		temp=(~temp)+1;
		value=temp*(-0.0625);
	}
	else
	{
		value=temp*0.0625;	
	}
	return value;    
}





#define ICON6_XS   ICON_XFRAM+ICON_XLEN+ICON_XSPACE
#define ICON6_YS   ICON_YFRAM+ICON_YSPACE+ICON_YLEN
#define ICON6_XE   ICON_XFRAM+2*ICON_XLEN+ICON_XSPACE
#define ICON6_YE   ICON_YFRAM+ICON_YSPACE+2*ICON_YLEN

void LCD_DisplayWindows(void);

void DS18B20_Test(void)
{
	float temp;
	u16 tem_dat;
	u8 dat[8];
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//关闭RTC秒中断
	
	LCD_Clear(BLACK);
	DS18B20_Init();
	
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"DS18B20测试");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	LCD_ShowFont12Char(10,50,"检测温度值：");
	LCD_ShowFont12Char(10,100,"将温度传感器插上主板接口上，注意温度传感器的方向，板子上有对应的丝印指示");
		
	while(1)
	{
		
		temp=DS18B20_GetTemperture();
		tem_dat=temp*100;
		dat[0]=tem_dat/1000+0x30;
		dat[1]=tem_dat%1000/100+0x30;
		dat[2]='.';
		dat[3]=tem_dat%1000%100/10+0x30;
		dat[4]=tem_dat%1000%100%10+0x30;
		dat[5]='C';
		dat[6]='\0';
		LCD_ShowFont12Char(120,50,dat);
		
		if(TouchData.lcdx>tftlcd_data.width-RETURN_X&&TouchData.lcdx<tftlcd_data.width&&TouchData.lcdy>tftlcd_data.height-RETURN_Y&&TouchData.lcdy<tftlcd_data.height)	
		{
			LCD_DisplayWindows();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//开启RTC秒中断
			break;					
		}
		TOUCH_Scan();
	}
	
}

