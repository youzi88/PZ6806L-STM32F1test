#include "adxl345.h"
#include "iic.h"
#include "math.h"
#include "SysTick.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"
#include "key.h"

//初始化ADXL345.
//返回值:0,初始化成功;1,初始化失败.
u8 ADXL345_Init(void)
{				  
	IIC_Init();							//初始化IIC总线	
	if(ADXL345_RD_Reg(DEVICE_ID)==0XE5)	//读取器件ID
	{  
		ADXL345_WR_Reg(DATA_FORMAT,0X2B);	//低电平中断输出,13位全分辨率,输出数据右对齐,16g量程 
		ADXL345_WR_Reg(BW_RATE,0x0A);		//数据输出速度为100Hz
		ADXL345_WR_Reg(POWER_CTL,0x28);	   	//链接使能,测量模式
		ADXL345_WR_Reg(INT_ENABLE,0x00);	//不使用中断		 
	 	ADXL345_WR_Reg(OFSX,0x00);
		ADXL345_WR_Reg(OFSY,0x00);
		ADXL345_WR_Reg(OFSZ,0x00);	
		return 0;
	}			
	return 1;	   								  
} 

//写ADXL345寄存器
//addr:寄存器地址
//val:要写入的值
//返回值:无
void ADXL345_WR_Reg(u8 addr,u8 val) 
{
	IIC_Start();  				 
	IIC_Send_Byte(ADXL_WRITE);     	//发送写器件指令	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   			//发送寄存器地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(val);     		//发送值					   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();						//产生一个停止条件 	   
} 

//读ADXL345寄存器
//addr:寄存器地址
//返回值:读到的值
u8 ADXL345_RD_Reg(u8 addr) 		
{
	u8 temp=0;		 
	IIC_Start();  				 
	IIC_Send_Byte(ADXL_WRITE);	//发送写器件指令	 
	temp=IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   		//发送寄存器地址
	temp=IIC_Wait_Ack(); 	 										  		   
	IIC_Start();  	 	   		//重新启动
	IIC_Send_Byte(ADXL_READ);	//发送读器件指令	 
	temp=IIC_Wait_Ack();	   
    temp=IIC_Read_Byte(0);		//读取一个字节,不继续再读,发送NAK 	    	   
    IIC_Stop();					//产生一个停止条件 	    
	return temp;				//返回读到的值
} 

//读取ADXL的平均值
//x,y,z:读取10次后取平均值
void ADXL345_RD_Avval(short *x,short *y,short *z)
{
	short tx=0,ty=0,tz=0;	   
	u8 i;  
	for(i=0;i<10;i++)
	{
		ADXL345_RD_XYZ(x,y,z);
		delay_ms(10);
		tx+=(short)*x;
		ty+=(short)*y;
		tz+=(short)*z;	   
	}
	*x=tx/10;
	*y=ty/10;
	*z=tz/10;
}  

//读取3个轴的数据
//x,y,z:读取到的数据
void ADXL345_RD_XYZ(short *x,short *y,short *z)
{
	u8 buf[6];
	u8 i;
	IIC_Start();  				 
	IIC_Send_Byte(ADXL_WRITE);	//发送写器件指令	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(0x32);   		//发送寄存器地址(数据缓存的起始地址为0X32)
	IIC_Wait_Ack(); 	 										  		   
 
 	IIC_Start();  	 	   		//重新启动
	IIC_Send_Byte(ADXL_READ);	//发送读器件指令
	IIC_Wait_Ack();
	for(i=0;i<6;i++)
	{
		if(i==5)buf[i]=IIC_Read_Byte(0);//读取一个字节,不继续再读,发送NACK  
		else buf[i]=IIC_Read_Byte(1);	//读取一个字节,继续读,发送ACK 
 	}	        	   
    IIC_Stop();					//产生一个停止条件
	*x=(short)(((u16)buf[1]<<8)+buf[0]); 	    
	*y=(short)(((u16)buf[3]<<8)+buf[2]); 	    
	*z=(short)(((u16)buf[5]<<8)+buf[4]); 	   
}

//自动校准
//xval,yval,zval:x,y,z轴的校准值
void ADXL345_AUTO_Adjust(char *xval,char *yval,char *zval)
{
	short tx,ty,tz;
	u8 i;
	short offx=0,offy=0,offz=0;
	ADXL345_WR_Reg(POWER_CTL,0x00);	   	//先进入休眠模式.
	delay_ms(100);
	ADXL345_WR_Reg(DATA_FORMAT,0X2B);	//低电平中断输出,13位全分辨率,输出数据右对齐,16g量程 
	ADXL345_WR_Reg(BW_RATE,0x0A);		//数据输出速度为100Hz
	ADXL345_WR_Reg(POWER_CTL,0x28);	   	//链接使能,测量模式
	ADXL345_WR_Reg(INT_ENABLE,0x00);	//不使用中断		 

	ADXL345_WR_Reg(OFSX,0x00);
	ADXL345_WR_Reg(OFSY,0x00);
	ADXL345_WR_Reg(OFSZ,0x00);
	delay_ms(12);
	for(i=0;i<10;i++)
	{
		ADXL345_RD_Avval(&tx,&ty,&tz);
		offx+=tx;
		offy+=ty;
		offz+=tz;
	}	 		
	offx/=10;
	offy/=10;
	offz/=10;
	*xval=-offx/4;
	*yval=-offy/4;
	*zval=-(offz-256)/4;	  
 	ADXL345_WR_Reg(OFSX,*xval);
	ADXL345_WR_Reg(OFSY,*yval);
	ADXL345_WR_Reg(OFSZ,*zval);	
}

//读取ADXL345的数据times次,再取平均
//x,y,z:读到的数据
//times:读取多少次
void ADXL345_Read_Average(short *x,short *y,short *z,u8 times)
{
	u8 i;
	short tx,ty,tz;
	*x=0;
	*y=0;
	*z=0;
	if(times)//读取次数不为0
	{
		for(i=0;i<times;i++)//连续读取times次
		{
			ADXL345_RD_XYZ(&tx,&ty,&tz);
			*x+=tx;
			*y+=ty;
			*z+=tz;
			delay_ms(5);
		}
		*x/=times;
		*y/=times;
		*z/=times;
	}
} 

//得到角度
//x,y,z:x,y,z方向的重力加速度分量(不需要单位,直接数值即可)
//dir:要获得的角度.0,与Z轴的角度;1,与X轴的角度;2,与Y轴的角度.
//返回值:角度值.单位0.1°.
//res得到的是弧度值，需要将其转换为角度值也就是*180/3.14
short ADXL345_Get_Angle(float x,float y,float z,u8 dir)
{
	float temp;
 	float res=0;
	switch(dir)
	{
		case 0://与自然Z轴的角度
 			temp=sqrt((x*x+y*y))/z;
 			res=atan(temp);
 			break;
		case 1://与自然X轴的角度
 			temp=x/sqrt((y*y+z*z));
 			res=atan(temp);
 			break;
 		case 2://与自然Y轴的角度
 			temp=y/sqrt((x*x+z*z));
 			res=atan(temp);
 			break;
 	}
	return res*180/3.14;
}

 

#define ICON11_XS   ICON_XFRAM+2*ICON_XLEN+2*ICON_XSPACE
#define ICON11_YS   ICON_YFRAM+2*ICON_YSPACE+2*ICON_YLEN
#define ICON11_XE   ICON_XFRAM+3*ICON_XLEN+2*ICON_XSPACE
#define ICON11_YE   ICON_YFRAM+2*ICON_YSPACE+3*ICON_YLEN

void LCD_DisplayWindows(void);

//x,y:开始显示的坐标位置
//num:要显示的数据
//mode:0,显示加速度值;1,显示角度值;
void ADXL_Show_num(u16 x,u16 y,short num,u8 mode)   //ADXL345显示
{
	u8 valbuf[3];
	if(mode==0)	  //显示加速度值
	{
		if(num<0)
		{
			num=-num;
			LCD_ShowString(x,y,tftlcd_data.width,tftlcd_data.height,16,"-");
		}
		else
		{
			LCD_ShowString(x,y,tftlcd_data.width,tftlcd_data.height,16," ");
		}
		valbuf[0]=num/100+0x30;
		valbuf[1]=num%100/10+0x30;
		valbuf[2]=num%100%10+0x30;	
		LCD_ShowString(x+10,y,tftlcd_data.width,tftlcd_data.height,16,valbuf);	
	}
	else		 //显示角度值
	{
		if(num<0)
		{
			num=-num;
			LCD_ShowString(x,y,tftlcd_data.width,tftlcd_data.height,16,"-");
		}
		else
		{
			LCD_ShowString(x,y,tftlcd_data.width,tftlcd_data.height,16," ");
		}
		valbuf[0]=num/10+0x30;
		valbuf[1]='.';
		valbuf[2]=num%10+0x30;	
		LCD_ShowString(x+10,y,tftlcd_data.width,tftlcd_data.height,16,valbuf);		
	}		
}


void ADX345_Test(void)
{
	short x,y,z;
	short xang,yang,zang;	
	u8 key;
	
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//关闭RTC秒中断
	
	LCD_Clear(BLACK);
	ADXL345_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"ADX345L加速度传感器测试");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	
	LCD_ShowString(10,90,tftlcd_data.width,tftlcd_data.height,16,"K_UP:ADXL345 Adjust");
	LCD_ShowString(10,120,tftlcd_data.width,tftlcd_data.height,16,"X Val:");
	LCD_ShowString(10,140,tftlcd_data.width,tftlcd_data.height,16,"Y Val:");
	LCD_ShowString(10,160,tftlcd_data.width,tftlcd_data.height,16,"Z Val:");
	LCD_ShowString(10,180,tftlcd_data.width,tftlcd_data.height,16,"X Ang:");
	LCD_ShowString(10,200,tftlcd_data.width,tftlcd_data.height,16,"Y Ang:");
	LCD_ShowString(10,220,tftlcd_data.width,tftlcd_data.height,16,"Z Ang:");
	
	while(1)
	{
		ADXL345_Read_Average(&x,&y,&z,10);  //读取x,y,z 3个方向的加速度值 总共10次
		ADXL_Show_num(60,120,x,0);
		ADXL_Show_num(60,140,y,0);
		ADXL_Show_num(60,160,z,0);

		xang=ADXL345_Get_Angle(x,y,z,1);
		yang=ADXL345_Get_Angle(x,y,z,2);
		zang=ADXL345_Get_Angle(x,y,z,0);
		ADXL_Show_num(60,180,xang,1);
		ADXL_Show_num(60,200,yang,1);
		ADXL_Show_num(60,220,zang,1);

		key=KEY_Scan(0);
		if(key==KEY_UP)		  //按下K_UP校准
		{
			ADXL345_AUTO_Adjust((char*)&x,(char*)&y,(char*)&z);
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
