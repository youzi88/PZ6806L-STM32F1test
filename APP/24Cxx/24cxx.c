#include "24cxx.h"
#include "SysTick.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"
#include "key.h"

/*******************************************************************************
* �� �� ��         : AT24CXX_Init
* ��������		   : AT24CXX��ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void AT24CXX_Init(void)
{
	IIC_Init();//IIC��ʼ��
}

/*******************************************************************************
* �� �� ��         : AT24CXX_ReadOneByte
* ��������		   : ��AT24CXXָ����ַ����һ������
* ��    ��         : ReadAddr:��ʼ�����ĵ�ַ 
* ��    ��         : ����������
*******************************************************************************/
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ	    
	}
	else 
	{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д����
	} 	   
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}

/*******************************************************************************
* �� �� ��         : AT24CXX_WriteOneByte
* ��������		   : ��AT24CXXָ����ַд��һ������
* ��    ��         : WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ 
					 DataToWrite:Ҫд�������
* ��    ��         : ��
*******************************************************************************/
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}
	else 
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д����
	} 	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

/*******************************************************************************
* �� �� ��         : AT24CXX_WriteLenByte
* ��������		   : ��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
					 ����д��16bit����32bit������
* ��    ��         : WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ 
					 DataToWrite:Ҫд�������
					 Len        :Ҫд�����ݵĳ���2,4
* ��    ��         : ��
*******************************************************************************/
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/*******************************************************************************
* �� �� ��         : AT24CXX_ReadLenByte
* ��������		   : ��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
					 ���ڶ���16bit����32bit������
* ��    ��         : ReadAddr   :��ʼ�����ĵ�ַ 
					 Len        :Ҫ�������ݵĳ���2,4
* ��    ��         : ��ȡ������
*******************************************************************************/
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/*******************************************************************************
* �� �� ��         : AT24CXX_Check
* ��������		   : ���AT24CXX�Ƿ�����
* ��    ��         : ��
* ��    ��         : 1:���ʧ�ܣ�0:���ɹ�
*******************************************************************************/
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0x36)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X36);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X36)return 0;
	}
	return 1;											  
}

/*******************************************************************************
* �� �� ��         : AT24CXX_Read
* ��������		   : ��AT24CXX�����ָ����ַ��ʼ����ָ������������
* ��    ��         : ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
					 pBuffer  :���������׵�ַ
					 NumToRead:Ҫ�������ݵĸ���
* ��    ��         : ��
*******************************************************************************/
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
} 

/*******************************************************************************
* �� �� ��         : AT24CXX_Write
* ��������		   : ��AT24CXX�����ָ����ַ��ʼд��ָ������������
* ��    ��         : WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
					 pBuffer  :���������׵�ַ
					 NumToRead:Ҫ�������ݵĸ���
* ��    ��         : ��
*******************************************************************************/
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

#define ICON7_XS   ICON_XFRAM+2*ICON_XLEN+2*ICON_XSPACE
#define ICON7_YS   ICON_YFRAM+ICON_YSPACE+ICON_YLEN
#define ICON7_XE   ICON_XFRAM+3*ICON_XLEN+2*ICON_XSPACE
#define ICON7_YE   ICON_YFRAM+ICON_YSPACE+2*ICON_YLEN

void LCD_DisplayWindows(void);
void EEPROM_Test(void)
{
	u8 key;
	u8 wdata=0,value;
	u8 dat[6],dat1[6];
	
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//�ر�RTC���ж�
	
	LCD_Clear(BLACK);
	AT24CXX_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"EEPROM����");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	LCD_ShowFont12Char(10,44,"��K_UP��:��AT24C02д����");
	LCD_ShowFont12Char(10,61,"��K_DOWM��:��AT24C02������");
	LCD_ShowFont12Char(10,100,"д���ݵķ�Χ��: 0-255");
	
	LCD_ShowFont12Char(10,150,"Write data is:");
	LCD_ShowFont12Char(10,167,"Read data is:");
	while(1)
	{

		key=KEY_Scan(0);
		if(key==KEY_UP)
		{
			AT24CXX_WriteOneByte(0,++wdata);//д����ǵ��ֽڣ�0-255ȡֵ��Χ
			dat[0]=wdata/100+0x30;
			dat[1]=wdata%100/10+0x30;
			dat[2]=wdata%100%10+0x30;
			dat[3]='\0';
			LCD_ShowFont12Char(160,150,dat);
		}
		if(key==KEY_DOWN)
		{
			value=AT24CXX_ReadOneByte(0);
			dat1[0]=value/100+0x30;
			dat1[1]=value%100/10+0x30;
			dat1[2]=value%100%10+0x30;
			dat1[3]='\0';
			LCD_ShowFont12Char(160,167,dat1);
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



