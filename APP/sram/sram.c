#include "sram.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "key.h"
#include "gui.h"

//��ʼ���ⲿSRAM
void FSMC_SRAM_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  FSMC_ReadWriteNORSRAMTiming; 

	//PD,PE-FSMC_DATA 	  PF,PG-FSMC_A
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE  
	                      | RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;	  //FSMC_NE3 PG10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1;	  //FSMC_NBL0-1 PE0	PE1
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4|GPIO_Pin_5;	  //FSMC_NOE,FSMC_NWE PD4 PD5
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8  
								  | GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_11
								  | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
								  | GPIO_Pin_15 );
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9
	                               | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12
								   | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2  
								  | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5
								  | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
								  | GPIO_Pin_15 );
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2  
								  | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5);
	GPIO_Init(GPIOG, &GPIO_InitStructure);
 	
 	  
 	FSMC_ReadWriteNORSRAMTiming.FSMC_AddressSetupTime = 0x00;	 //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 
	FSMC_ReadWriteNORSRAMTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�	
	FSMC_ReadWriteNORSRAMTiming.FSMC_DataSetupTime = 0x08;		 ////���ݱ���ʱ�䣨DATAST��Ϊ9��HCLK 	 	 
	FSMC_ReadWriteNORSRAMTiming.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_ReadWriteNORSRAMTiming.FSMC_CLKDivision = 0x00;
	FSMC_ReadWriteNORSRAMTiming.FSMC_DataLatency = 0x00;
	FSMC_ReadWriteNORSRAMTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 
    

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;//  ��������ʹ��NE3 ��Ҳ�Ͷ�ӦBTCR[4],[5]��
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//�洢�����ݿ��Ϊ16bit  
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//�洢��дʹ�� 
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;  
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; // ��дʹ����ͬ��ʱ��
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_ReadWriteNORSRAMTiming;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_ReadWriteNORSRAMTiming; //��дͬ��ʱ��

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC����

 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // ʹ��BANK1����3										  
											
}

//��ָ����ַ(WriteAddr+Bank1_SRAM3_ADDR)��ʼ,����д��n���ֽ�.
//pBuffer:�ֽ�ָ��
//WriteAddr:Ҫд��ĵ�ַ
//n:Ҫд����ֽ���
void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)  
	{										    
		*(u8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer++;	 		
		WriteAddr++;
	}   
}		

//��ָ����ַ((WriteAddr+Bank1_SRAM3_ADDR))��ʼ,��������n���ֽ�.
//pBuffer:�ֽ�ָ��
//ReadAddr:Ҫ��������ʼ��ַ
//n:Ҫд����ֽ���
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)  
	{											    
		*pBuffer++=*(u8*)(Bank1_SRAM3_ADDR+ReadAddr);    
		ReadAddr++;
	}  
} 
 


#define ICON10_XS   ICON_XFRAM+ICON_XLEN+ICON_XSPACE
#define ICON10_YS   ICON_YFRAM+2*ICON_YSPACE+2*ICON_YLEN
#define ICON10_XE   ICON_XFRAM+2*ICON_XLEN+ICON_XSPACE
#define ICON10_YE   ICON_YFRAM+2*ICON_YSPACE+3*ICON_YLEN

void LCD_DisplayWindows(void);


//�ⲿ�ڴ����(���֧��1M�ֽ��ڴ����)	    
void ExSRAM_Cap_Test(u16 x,u16 y)
{
    u8 writeData = 0xf0, readData;
	u16 cap=0;
    u32 addr;
	
	addr = 1024; //��1KBλ�ÿ�ʼ����
	
	LCD_ShowString(x,y,239,y+16,16,"ExSRAM Cap:   0KB"); 
	
	while(1)
	{
		FSMC_SRAM_WriteBuffer(&writeData, addr, 1);
		FSMC_SRAM_ReadBuffer(&readData,addr,1);
		
		/* �鿴��ȡ���������Ƿ��д������һ�� */
        if(readData == writeData)
        {
            cap++;
            addr += 1024;
            readData = 0;
            if(addr > 1024 * 1024) //SRAM�������Ϊ1MB
            {
                break;
            }    
        }
        else
        {
            break;
        }     
	}
	LCD_ShowxNum(x+11*8,y,cap,4,16,0);//��ʾ�ڴ����� 
}

void SRAM_Test(void)
{
	u8 sbuf[15]="www.prechin.cn";
	u8 key;
	
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//�ر�RTC���ж�
	
	LCD_Clear(BLACK);
	FSMC_SRAM_Init();
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"�ⲿSRAM����");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	LCD_ShowFont12Char(10, 38, "����K_up д����");
	LCD_ShowFont12Char(10, 56, "����K_down ������");
	LCD_ShowFont12Char(10, 74, "SRAM Cap:     KB");
	ExSRAM_Cap_Test(10, 74);
	
	while(1)
	{
	
		key=KEY_Scan(0);
		if(key==KEY_UP)
		{
			FSMC_SRAM_WriteBuffer(sbuf, 2, 15);
			LCD_ShowFont12Char(10,98,"www.prechin.cn");
		}
		if(key==KEY_DOWN)
		{
			FSMC_SRAM_ReadBuffer(sbuf, 2, 15);
			LCD_ShowFont12Char(10, 158, sbuf);	
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
