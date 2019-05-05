#include "spi.h"
#include "SysTick.h"
#include "time.h"
#include <stdio.h>
#include "enc28j60.h"	  

#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"				   
#include "math.h" 	
#include "string.h"

#include "tftlcd.h" 
#include "touch.h"
#include "font_show.h"
#include "gui.h"

#include "rtc.h"
#include "adc.h"
#include "adc_temp.h"
#include "led.h"

static u8 ENC28J60BANK;
static u32 NextPacketPtr;


//��λENC28J60
//����SPI��ʼ��/IO��ʼ����

static void ENC28J60_SPI2_Init(void)
{
   	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ�� 	
   	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE );//PORTB,D,Gʱ��ʹ�� 
			  
	//����PG13��PG14����,��Ϊ�˷�ֹFLASH��SDӰ��.
	//��Ϊ���ǹ���һ��SPI��.  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz				 
 	GPIO_Init(GPIOG, &GPIO_InitStructure);					 
 	GPIO_SetBits(GPIOG,GPIO_Pin_13|GPIO_Pin_14);					

	//NRF24L01_CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 
 	GPIO_Init(GPIOF, &GPIO_InitStructure);					
 	GPIO_SetBits(GPIOF,GPIO_Pin_9);					

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_12;			 
 	GPIO_Init(GPIOB, &GPIO_InitStructure);					 
 	GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_12);	
	
	//SPI2 		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //PB13/14/15����


	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//����ͬ��ʱ�ӵĵ�һ�������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
	
	SPI2_ReadWriteByte(0xff);//��������	 

}


void ENC28J60_Reset(void)
{
 	 
	ENC28J60_SPI2_Init();//SPI2��ʼ��
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);	//SPI2 SCKƵ��Ϊ36M/4=9Mhz
 	TIM6_Init(1000,719);//100Khz����Ƶ�ʣ�������1000Ϊ10ms
	ENC28J60_RST=0;			//��λENC28J60
	delay_ms(10);	 
	ENC28J60_RST=1;			//��λ����				    
	delay_ms(10);	 
}
//��ȡENC28J60�Ĵ���(��������) 
//op��������
//addr:�Ĵ�����ַ/����
//����ֵ:����������
u8 ENC28J60_Read_Op(u8 op,u8 addr)
{
	u8 dat=0;	 
	ENC28J60_CS=0;	 
	dat=op|(addr&ADDR_MASK);
	SPI2_ReadWriteByte(dat);
	dat=SPI2_ReadWriteByte(0xFF);
	//����Ƕ�ȡMAC/MII�Ĵ���,��ڶ��ζ��������ݲ�����ȷ��,���ֲ�29ҳ
 	if(addr&0x80)dat=SPI2_ReadWriteByte(0xFF);
	ENC28J60_CS=1;
	return dat;
}
//��ȡENC28J60�Ĵ���(��������) 
//op��������
//addr:�Ĵ�����ַ
//data:����
void ENC28J60_Write_Op(u8 op,u8 addr,u8 data)
{
	u8 dat = 0;	    
	ENC28J60_CS=0;			   
	dat=op|(addr&ADDR_MASK);
	SPI2_ReadWriteByte(dat);	  
	SPI2_ReadWriteByte(data);
	ENC28J60_CS=1;
}
//��ȡENC28J60���ջ�������
//len:Ҫ��ȡ�����ݳ���
//data:������ݻ�����(ĩβ�Զ���ӽ�����)
void ENC28J60_Read_Buf(u32 len,u8* data)
{
	ENC28J60_CS=0;			 
	SPI2_ReadWriteByte(ENC28J60_READ_BUF_MEM);
	while(len)
	{
		len--;			  
		*data=(u8)SPI2_ReadWriteByte(0);
		data++;
	}
	*data='\0';
	ENC28J60_CS=1;
}
//��ENC28J60д���ͻ�������
//len:Ҫд������ݳ���
//data:���ݻ����� 
void ENC28J60_Write_Buf(u32 len,u8* data)
{
	ENC28J60_CS=0;			   
	SPI2_ReadWriteByte(ENC28J60_WRITE_BUF_MEM);		 
	while(len)
	{
		len--;
		SPI2_ReadWriteByte(*data);
		data++;
	}
	ENC28J60_CS=1;
}
//����ENC28J60�Ĵ���Bank
//ban:Ҫ���õ�bank
void ENC28J60_Set_Bank(u8 bank)
{								    
	if((bank&BANK_MASK)!=ENC28J60BANK)//�͵�ǰbank��һ�µ�ʱ��,������
	{				  
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,ECON1,(ECON1_BSEL1|ECON1_BSEL0));
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON1,(bank&BANK_MASK)>>5);
		ENC28J60BANK=(bank&BANK_MASK);
	}
}
//��ȡENC28J60ָ���Ĵ��� 
//addr:�Ĵ�����ַ
//����ֵ:����������
u8 ENC28J60_Read(u8 addr)
{						  
	ENC28J60_Set_Bank(addr);//����BANK		 
	return ENC28J60_Read_Op(ENC28J60_READ_CTRL_REG,addr);
}
//��ENC28J60ָ���Ĵ���д����
//addr:�Ĵ�����ַ
//data:Ҫд�������		 
void ENC28J60_Write(u8 addr,u8 data)
{					  
	ENC28J60_Set_Bank(addr);		 
	ENC28J60_Write_Op(ENC28J60_WRITE_CTRL_REG,addr,data);
}
//��ENC28J60��PHY�Ĵ���д������
//addr:�Ĵ�����ַ
//data:Ҫд�������		 
void ENC28J60_PHY_Write(u8 addr,u32 data)
{
	u16 retry=0;
	ENC28J60_Write(MIREGADR,addr);	//����PHY�Ĵ�����ַ
	ENC28J60_Write(MIWRL,data);		//д������
	ENC28J60_Write(MIWRH,data>>8);		   
	while((ENC28J60_Read(MISTAT)&MISTAT_BUSY)&&retry<0XFFF)retry++;//�ȴ�д��PHY����		  
}
//��ʼ��ENC28J60
//macaddr:MAC��ַ
//����ֵ:0,��ʼ���ɹ�;
//       1,��ʼ��ʧ��;
u8 ENC28J60_Init(u8* macaddr)
{		
	u16 retry=0;		  
	ENC28J60_Reset();
	ENC28J60_Write_Op(ENC28J60_SOFT_RESET,0,ENC28J60_SOFT_RESET);//�����λ
	while(!(ENC28J60_Read(ESTAT)&ESTAT_CLKRDY)&&retry<500)//�ȴ�ʱ���ȶ�
	{
		retry++;
		delay_ms(1);
	};
	if(retry>=500)return 1;//ENC28J60��ʼ��ʧ��
	// do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers,must write low byte first
	// set receive buffer start address	   ���ý��ջ�������ַ  8K�ֽ�����
	NextPacketPtr=RXSTART_INIT;
	// Rx start
	//���ջ�������һ��Ӳ�������ѭ��FIFO ���������ɡ�
	//�Ĵ�����ERXSTH:ERXSTL ��ERXNDH:ERXNDL ��
	//Ϊָ�룬���建���������������ڴ洢���е�λ�á�
	//ERXST��ERXNDָ����ֽھ�������FIFO�������ڡ�
	//������̫���ӿڽ��������ֽ�ʱ����Щ�ֽڱ�˳��д��
	//���ջ������� ���ǵ�д����ERXND ָ��Ĵ洢��Ԫ
	//��Ӳ�����Զ������յ���һ�ֽ�д����ERXST ָ��
	//�Ĵ洢��Ԫ�� ��˽���Ӳ��������д��FIFO ����ĵ�
	//Ԫ��
	//���ý�����ʼ�ֽ�
	ENC28J60_Write(ERXSTL,RXSTART_INIT&0xFF);	
	ENC28J60_Write(ERXSTH,RXSTART_INIT>>8);	  
	//ERXWRPTH:ERXWRPTL �Ĵ�������Ӳ����FIFO ��
	//���ĸ�λ��д������յ����ֽڡ� ָ����ֻ���ģ��ڳ�
	//�����յ�һ�����ݰ���Ӳ�����Զ�����ָ�롣 ָ���
	//�����ж�FIFO ��ʣ��ռ�Ĵ�С  8K-1500�� 
	//���ý��ն�ָ���ֽ�
	ENC28J60_Write(ERXRDPTL,RXSTART_INIT&0xFF);
	ENC28J60_Write(ERXRDPTH,RXSTART_INIT>>8);
	//���ý��ս����ֽ�
	ENC28J60_Write(ERXNDL,RXSTOP_INIT&0xFF);
	ENC28J60_Write(ERXNDH,RXSTOP_INIT>>8);
	//���÷�����ʼ�ֽ�
	ENC28J60_Write(ETXSTL,TXSTART_INIT&0xFF);
	ENC28J60_Write(ETXSTH,TXSTART_INIT>>8);
	//���÷��ͽ����ֽ�
	ENC28J60_Write(ETXNDL,TXSTOP_INIT&0xFF);
	ENC28J60_Write(ETXNDH,TXSTOP_INIT>>8);
	// do bank 1 stuff,packet filter:
	// For broadcast packets we allow only ARP packtets
	// All other packets should be unicast only for our mac (MAADR)
	//
	// The pattern to match on is therefore
	// Type     ETH.DST
	// ARP      BROADCAST
	// 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
	// in binary these poitions are:11 0000 0011 1111
	// This is hex 303F->EPMM0=0x3f,EPMM1=0x30
	//���չ�����
	//UCEN������������ʹ��λ
	//��ANDOR = 1 ʱ��
	//1 = Ŀ���ַ�뱾��MAC ��ַ��ƥ������ݰ���������
	//0 = ��ֹ������
	//��ANDOR = 0 ʱ��
	//1 = Ŀ���ַ�뱾��MAC ��ַƥ������ݰ��ᱻ����
	//0 = ��ֹ������
	//CRCEN���������CRC У��ʹ��λ
	//1 = ����CRC ��Ч�����ݰ�����������
	//0 = ������CRC �Ƿ���Ч
	//PMEN����ʽƥ�������ʹ��λ
	//��ANDOR = 1 ʱ��
	//1 = ���ݰ�������ϸ�ʽƥ�����������򽫱�����
	//0 = ��ֹ������
	//��ANDOR = 0 ʱ��
	//1 = ���ϸ�ʽƥ�����������ݰ���������
	//0 = ��ֹ������
	ENC28J60_Write(ERXFCON,ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	ENC28J60_Write(EPMM0,0x3f);
	ENC28J60_Write(EPMM1,0x30);
	ENC28J60_Write(EPMCSL,0xf9);
	ENC28J60_Write(EPMCSH,0xf7);
	// do bank 2 stuff
	// enable MAC receive
	//bit 0 MARXEN��MAC ����ʹ��λ
	//1 = ����MAC �������ݰ�
	//0 = ��ֹ���ݰ�����
	//bit 3 TXPAUS����ͣ����֡����ʹ��λ
	//1 = ����MAC ������ͣ����֡������ȫ˫��ģʽ�µ��������ƣ�
	//0 = ��ֹ��ͣ֡����
	//bit 2 RXPAUS����ͣ����֡����ʹ��λ
	//1 = �����յ���ͣ����֡ʱ����ֹ���ͣ�����������
	//0 = ���Խ��յ�����ͣ����֡
	ENC28J60_Write(MACON1,MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
	// bring MAC out of reset
	//��MACON2 �е�MARST λ���㣬ʹMAC �˳���λ״̬��
	ENC28J60_Write(MACON2,0x00);
	// enable automatic padding to 60bytes and CRC operations
	//bit 7-5 PADCFG2:PACDFG0���Զ�����CRC ����λ
	//111 = ��0 ������ж�֡��64 �ֽڳ�����׷��һ����Ч��CRC
	//110 = ���Զ�����֡
	//101 = MAC �Զ�������8100h �����ֶε�VLAN Э��֡�����Զ���䵽64 �ֽڳ��������
	//��VLAN ֡���������60 �ֽڳ�������Ҫ׷��һ����Ч��CRC
	//100 = ���Զ�����֡
	//011 = ��0 ������ж�֡��64 �ֽڳ�����׷��һ����Ч��CRC
	//010 = ���Զ�����֡
	//001 = ��0 ������ж�֡��60 �ֽڳ�����׷��һ����Ч��CRC
	//000 = ���Զ�����֡
	//bit 4 TXCRCEN������CRC ʹ��λ
	//1 = ����PADCFG��Σ�MAC�����ڷ���֡��ĩβ׷��һ����Ч��CRC�� ���PADCFG�涨Ҫ
	//׷����Ч��CRC������뽫TXCRCEN ��1��
	//0 = MAC����׷��CRC�� ������4 ���ֽڣ����������Ч��CRC �򱨸������״̬������
	//bit 0 FULDPX��MAC ȫ˫��ʹ��λ
	//1 = MAC������ȫ˫��ģʽ�¡� PHCON1.PDPXMD λ������1��
	//0 = MAC�����ڰ�˫��ģʽ�¡� PHCON1.PDPXMD λ�������㡣
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,MACON3,MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);
	// set inter-frame gap (non-back-to-back)
	//���÷Ǳ��Ա��������Ĵ����ĵ��ֽ�
	//MAIPGL�� �����Ӧ��ʹ��12h ��̸üĴ�����
	//���ʹ�ð�˫��ģʽ��Ӧ��̷Ǳ��Ա�������
	//�Ĵ����ĸ��ֽ�MAIPGH�� �����Ӧ��ʹ��0Ch
	//��̸üĴ�����
	ENC28J60_Write(MAIPGL,0x12);
	ENC28J60_Write(MAIPGH,0x0C);
	// set inter-frame gap (back-to-back)
	//���ñ��Ա��������Ĵ���MABBIPG����ʹ��
	//ȫ˫��ģʽʱ�������Ӧ��ʹ��15h ��̸üĴ�
	//������ʹ�ð�˫��ģʽʱ��ʹ��12h ���б�̡�
	ENC28J60_Write(MABBIPG,0x15);
	// Set the maximum packet size which the controller will accept
	// Do not send packets longer than MAX_FRAMELEN:
	// ���֡����  1500
	ENC28J60_Write(MAMXFLL,MAX_FRAMELEN&0xFF);	
	ENC28J60_Write(MAMXFLH,MAX_FRAMELEN>>8);
	// do bank 3 stuff
	// write MAC address
	// NOTE: MAC address in ENC28J60 is byte-backward
	//����MAC��ַ
	ENC28J60_Write(MAADR5,macaddr[0]);	
	ENC28J60_Write(MAADR4,macaddr[1]);
	ENC28J60_Write(MAADR3,macaddr[2]);
	ENC28J60_Write(MAADR2,macaddr[3]);
	ENC28J60_Write(MAADR1,macaddr[4]);
	ENC28J60_Write(MAADR0,macaddr[5]);
	//����PHYΪȫ˫��  LEDBΪ������
	ENC28J60_PHY_Write(PHCON1,PHCON1_PDPXMD);	 
	// no loopback of transmitted frames	 ��ֹ����
	//HDLDIS��PHY ��˫�����ؽ�ֹλ
	//��PHCON1.PDPXMD = 1 ��PHCON1.PLOOPBK = 1 ʱ��
	//��λ�ɱ����ԡ�
	//��PHCON1.PDPXMD = 0 ��PHCON1.PLOOPBK = 0 ʱ��
	//1 = Ҫ���͵����ݽ�ͨ��˫���߽ӿڷ���
	//0 = Ҫ���͵����ݻỷ�ص�MAC ��ͨ��˫���߽ӿڷ���
	ENC28J60_PHY_Write(PHCON2,PHCON2_HDLDIS);
	// switch to bank 0
	//ECON1 �Ĵ���
	//�Ĵ���3-1 ��ʾΪECON1 �Ĵ����������ڿ���
	//ENC28J60 ����Ҫ���ܡ� ECON1 �а�������ʹ�ܡ���
	//������DMA ���ƺʹ洢��ѡ��λ��	   
	ENC28J60_Set_Bank(ECON1);
	// enable interrutps
	//EIE�� ��̫���ж�����Ĵ���
	//bit 7 INTIE�� ȫ��INT �ж�����λ
	//1 = �����ж��¼�����INT ����
	//0 = ��ֹ����INT ���ŵĻ������ʼ�ձ�����Ϊ�ߵ�ƽ��
	//bit 6 PKTIE�� �������ݰ��������ж�����λ
	//1 = ����������ݰ��������ж�
	//0 = ��ֹ�������ݰ��������ж�
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,EIE,EIE_INTIE|EIE_PKTIE);
	// enable packet reception
	//bit 2 RXEN������ʹ��λ
	//1 = ͨ����ǰ�����������ݰ�����д����ջ�����
	//0 = �������н��յ����ݰ�
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON1,ECON1_RXEN);
	if(ENC28J60_Read(MAADR5)== macaddr[0])return 0;//��ʼ���ɹ�
	else return 1; 	  

}
//��ȡEREVID
u8 ENC28J60_Get_EREVID(void)
{
	//��EREVID ��Ҳ�洢�˰汾��Ϣ�� EREVID ��һ��ֻ����
	//�ƼĴ���������һ��5 λ��ʶ����������ʶ�����ض���Ƭ
	//�İ汾��
	return ENC28J60_Read(EREVID);
}
#include "uip.h"
//ͨ��ENC28J60�������ݰ�������
//len:���ݰ���С
//packet:���ݰ�
void ENC28J60_Packet_Send(u32 len,u8* packet)
{
	//���÷��ͻ�������ַдָ�����
	ENC28J60_Write(EWRPTL,TXSTART_INIT&0xFF);
	ENC28J60_Write(EWRPTH,TXSTART_INIT>>8);
	//����TXNDָ�룬�Զ�Ӧ���������ݰ���С	   
	ENC28J60_Write(ETXNDL,(TXSTART_INIT+len)&0xFF);
	ENC28J60_Write(ETXNDH,(TXSTART_INIT+len)>>8);
	//дÿ�������ֽڣ�0x00��ʾʹ��macon3�����ã� 
	ENC28J60_Write_Op(ENC28J60_WRITE_BUF_MEM,0,0x00);
	//�������ݰ������ͻ�����
	//printf("len:%d\r\n",len);	//���ӷ������ݳ���
 	ENC28J60_Write_Buf(len,packet);
 	//�������ݵ�����
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON1,ECON1_TXRTS);
	//��λ�����߼������⡣�μ�Rev. B4 Silicon Errata point 12.
	if((ENC28J60_Read(EIR)&EIR_TXERIF))ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,ECON1,ECON1_TXRTS);
}
//�������ȡһ�����ݰ�����
//maxlen:���ݰ����������ճ���
//packet:���ݰ�������
//����ֵ:�յ������ݰ�����(�ֽ�)									  
u32 ENC28J60_Packet_Receive(u32 maxlen,u8* packet)
{
	u32 rxstat;
	u32 len;    													 
	if(ENC28J60_Read(EPKTCNT)==0)return 0;  //�Ƿ��յ����ݰ�?	   
	//���ý��ջ�������ָ��
	ENC28J60_Write(ERDPTL,(NextPacketPtr));
	ENC28J60_Write(ERDPTH,(NextPacketPtr)>>8);	   
	// ����һ������ָ��
	NextPacketPtr=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0);
	NextPacketPtr|=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0)<<8;
	//�����ĳ���
	len=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0);
	len|=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0)<<8;
 	len-=4; //ȥ��CRC����
	//��ȡ����״̬
	rxstat=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0);
	rxstat|=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0)<<8;
	//���ƽ��ճ���	
	if (len>maxlen-1)len=maxlen-1;	
	//���CRC�ͷ��Ŵ���
	// ERXFCON.CRCENΪĬ������,һ�����ǲ���Ҫ���.
	if((rxstat&0x80)==0)len=0;//��Ч
	else ENC28J60_Read_Buf(len,packet);//�ӽ��ջ������и������ݰ�	    
	//RX��ָ���ƶ�����һ�����յ������ݰ��Ŀ�ʼλ�� 
	//���ͷ����ǸղŶ��������ڴ�
	ENC28J60_Write(ERXRDPTL,(NextPacketPtr));
	ENC28J60_Write(ERXRDPTH,(NextPacketPtr)>>8);
	//�ݼ����ݰ���������־�����Ѿ��õ�������� 
 	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON2,ECON2_PKTDEC);
	return(len);
}








#if 1

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	 		  


//����->�ַ���ת������
//��num����(λ��Ϊlen)תΪ�ַ���,�����buf����
//num:����,����
//buf:�ַ�������
//len:����
void num2str(u16 num,u8 *buf,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		buf[i]=(num/LCD_Pow(10,len-i-1))%10+'0';
	}
}
//��ȡSTM32�ڲ��¶ȴ��������¶�
//temp:����¶��ַ������׵�ַ.��"28.3";
//temp,���ٵ���5���ֽڵĿռ�!
void get_temperature(u8 *temp)
{			  
	u16 t;
	float temperate;		   
	temperate=Get_ADC_Temp_Value(ADC_Channel_16,10);			 
	temperate=temperate*(3.3/4096);			    											    
	temperate=(1.43-temperate)/0.0043+25;	//�������ǰ�¶�ֵ
	t=temperate*10;//�õ��¶�
	num2str(t/10,temp,2);							   
	temp[2]='.';temp[3]=t%10+'0';temp[4]=0;	//�����ӽ�����
}
//��ȡRTCʱ��
//time:���ʱ���ַ���,����:"2012-09-27 12:33"
//time,���ٵ���17���ֽڵĿռ�!
void get_time(u8 *time)
{	
	RTC_Get();
	time[4]='-';time[7]='-';time[10]=' ';
	time[13]=':';time[16]=0;			//�����ӽ�����
	num2str(calendar.w_year,time,4);	//���->�ַ���
	num2str(calendar.w_month,time+5,2); //�·�->�ַ���	 
	num2str(calendar.w_date,time+8,2); 	//����->�ַ���
	num2str(calendar.hour,time+11,2); 	//Сʱ->�ַ���
	num2str(calendar.min,time+14,2); 	//����->�ַ���								   	  									  
}

//uip�¼�������
//���뽫�ú��������û���ѭ��,ѭ������.

void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;	 
	if(timer_ok==0)//����ʼ��һ��
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //����1��0.5��Ķ�ʱ�� 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//����1��10��Ķ�ʱ�� 
	}				 
	uip_len=tapdev_read();	//�������豸��ȡһ��IP��,�õ����ݳ���.uip_len��uip.c�ж���
	if(uip_len>0) 			//������
	{   
		//����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//�Ƿ���IP��? 
		{
			uip_arp_ipin();	//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   	//IP������
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)		    
			if(uip_len>0)//��Ҫ��Ӧ����
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//����arp����,�Ƿ���ARP�����?
		{
			uip_arp_arpin();
 			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
 			if(uip_len>0)tapdev_send();//��Ҫ��������,��ͨ��tapdev_send����	 
		}
	}else if(timer_expired(&periodic_timer))	//0.5�붨ʱ����ʱ
	{
		timer_reset(&periodic_timer);		//��λ0.5�붨ʱ�� 
		//��������ÿ��TCP����, UIP_CONNSȱʡ��40��  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//����TCPͨ���¼�  
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
	 		if(uip_len>0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#if UIP_UDP	//UIP_UDP 
		//��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10��
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//����UDPͨ���¼�
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#endif 
		//ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}




#define ICON16_XS   ICON_XFRAM+3*ICON_XLEN+3*ICON_XSPACE
#define ICON16_YS   ICON_YFRAM+3*ICON_YSPACE+3*ICON_YLEN
#define ICON16_XE   ICON_XFRAM+4*ICON_XLEN+3*ICON_XSPACE
#define ICON16_YE   ICON_YFRAM+3*ICON_YSPACE+4*ICON_YLEN

void LCD_DisplayWindows(void);

void Internet_Test()		   //��̫������
{
	uip_ipaddr_t ipaddr;
	
	RTC_ITConfig(RTC_IT_SEC, DISABLE);//�ر�RTC���ж�

	LCD_Clear(BLACK);
	
	FRONT_COLOR=YELLOW;
	LCD_ShowFont12Char(10,10,"EN28J60��̫������");
	LCD_ShowFont12Char(10,40,"ע�⣺ʹ�����߽����������ں�·�������ڻ�����������ӣ���֤����ͬһ������");
	LCD_ShowFont12Char(10,90,"��IE�����������IP��ַ��192.168.1.16���ɵ�����ҳ���粻���븴λ���ڽ���");
	LCD_ShowFont12Char(10,140,"����ʹ����ҳ�ϵİ������ƿ�����С�Ƶı仯��Ҳ���Ե�����ǹ�˾��̳��ַ���뽻����̳��ҳ");
	LCD_ShowFont12Char(10,300,"�밴����λ��������������");
	LCD_ShowPicture(tftlcd_data.width-RETURN_X,tftlcd_data.height-RETURN_Y,RETURN_X,RETURN_Y,(u8 *)gImage_return);
	
	//RTC_Init();
	ADCx_Init();	
	ADC_Temp_Init();

	while(tapdev_init())	//��ʼ��ENC28J60����
	{								   
		LCD_ShowString(10,200,tftlcd_data.width,tftlcd_data.height,16,"ENC28J60 Init Error!");	 
		//printf("ENC28J60 Init Error!\r\n");
		
		if(TouchData.lcdx>tftlcd_data.width-RETURN_X&&TouchData.lcdx<tftlcd_data.width&&TouchData.lcdy>tftlcd_data.height-RETURN_Y&&TouchData.lcdy<tftlcd_data.height)	
		{
			LCD_DisplayWindows();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//����RTC���ж�
			return;				
		}
		TOUCH_Scan();
	}
	led1=0;
	led2=0;
	//printf("ENC28J60 Init OK!\r\n");
	LCD_ShowString(10,200,tftlcd_data.width,tftlcd_data.height,16,"                    ");
	uip_init();				//uIP��ʼ��
	  	   						  	 
	LCD_ShowString(10,230,tftlcd_data.width,tftlcd_data.height,16,"IP:192.168.1.16");
	LCD_ShowString(10,250,tftlcd_data.width,tftlcd_data.height,16,"MASK:255.255.255.0");
	LCD_ShowString(10,270,tftlcd_data.width,tftlcd_data.height,16,"GATEWAY:192.168.1.1");
	
	uip_ipaddr(ipaddr, 192,168,1,16);	//���ñ�������IP��ַ
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, 192,168,1,1); 	//��������IP��ַ(��ʵ������·������IP��ַ)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, 255,255,255,0);	//������������
	uip_setnetmask(ipaddr);

	uip_listen(HTONS(1200));			//����1200�˿�,����TCP Server
	uip_listen(HTONS(80));				//����80�˿�,����Web Server
  	tcp_client_reconnect();	   		    //�������ӵ�TCP Server��,����TCP Client
	
	while (1)
	{
		uip_polling();	//����uip�¼���������뵽�û������ѭ������  
		delay_ms(1); 
			
		if(TouchData.lcdx>tftlcd_data.width-RETURN_X&&TouchData.lcdx<tftlcd_data.width&&TouchData.lcdy>tftlcd_data.height-RETURN_Y&&TouchData.lcdy<tftlcd_data.height)	
		{
			led1=1;
			led2=1;
			LCD_DisplayWindows();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);//����RTC���ж�
			return;				
		}
		TOUCH_Scan();
	}  
}
#endif

