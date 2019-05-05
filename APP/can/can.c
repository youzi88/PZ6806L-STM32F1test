#include "can.h"
#include "usart.h"
#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "key.h"
#include "gui.h"


//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,�������CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//������Ϊ:42M/((6+7+1)*6)=500Kbps
void CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef  		NVIC_InitStructure;
#endif
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); //��CAN1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //PA�˿�ʱ�Ӵ�
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//PA11	   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	 //��������ģʽ
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		//PA12	   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	 //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//ʹ�ñ����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= mode;	 //ģʽ���� 
  	CAN_InitStructure.CAN_SJW=tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1
	
	//���ù�����
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	
#if CAN_RX0_INT_ENABLE 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				//FIFO0��Ϣ�Һ��ж�����.		    

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

#if CAN_RX0_INT_ENABLE	//ʹ��RX0�ж�
//�жϷ�����			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
	int i=0;
    CAN_Receive(CAN1, 0, &RxMessage);
	for(i=0;i<8;i++)
	printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN_Send_Msg(u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x12;	 // ��׼��ʶ��Ϊ0
	TxMessage.ExtId=0x12;	 // ������չ��ʾ����29λ��
	TxMessage.IDE=0;		  // ʹ����չ��ʶ��
	TxMessage.RTR=0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.DLC=len;							 // ������֡��Ϣ
	for(i=0;i<len;i++)
		TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	if(i>=0XFFF)return 1;
	return 0;		
}

//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
    for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}


#define ICON15_XS   ICON_XFRAM+2*ICON_XLEN+2*ICON_XSPACE
#define ICON15_YS   ICON_YFRAM+3*ICON_YSPACE+3*ICON_YLEN
#define ICON15_XE   ICON_XFRAM+3*ICON_XLEN+2*ICON_XSPACE
#define ICON15_YE   ICON_YFRAM+3*ICON_YSPACE+4*ICON_YLEN

void LCD_DisplayWindows(void);
void CAN_Test(void)
{
	u8 key,j=0;
	u8 mode=0;
	u8 res;
	u8 tbuf[8],char_buf[8];
	u8 rbuf[8];
	
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//�ر�RTC���ж�
	
	LCD_Clear(BLACK);
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"CAN����");
	LCD_ShowFont12Char(10,50,"ע�⣺��CANģ���P16���Ӷ̽ӵ�CAN��");
	LCD_ShowFont12Char(10,100,"K_UP��ģʽ  K_DOWN������");	
	LCD_ShowFont12Char(10,150,"ģʽ��");
	LCD_ShowFont12Char(10,180,"���ͣ�");
	LCD_ShowFont12Char(10,210,"���գ�");
	
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);//500Kbps������
	
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY_UP)  //ģʽ�л�
		{
			mode=!mode;
			CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,mode);
			if(mode==0)
			{
				LCD_ShowFont12Char(50,150,"����ģʽ");	
			}
			else
			{
				LCD_ShowFont12Char(50,150,"�ػ�ģʽ");
			}
			LCD_ShowFont12Char(50,180,"               ");
			LCD_ShowFont12Char(50,210,"               ");
			LCD_ShowFont12Char(200,180,"    ");
		}
		if(key==KEY_DOWN)  //��������
		{
			for(j=0;j<8;j++)
			{
				tbuf[j]=j;
				char_buf[j]=tbuf[j]+0x30;
			}
			res=CAN_Send_Msg(tbuf,8);
			if(res)
			{
				LCD_ShowFont12Char(200,180,"ʧ��");
			}
			else
			{
				LCD_ShowFont12Char(200,180,"    ");
				LCD_ShowFont12Char(50,180,char_buf);
			}
			
		}
		res=CAN_Receive_Msg(rbuf);
		if(res)
		{
			for(j=0;j<res;j++)
			{
				char_buf[j]=rbuf[j]+0x30;
			}
			LCD_ShowFont12Char(50,210,char_buf);
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



