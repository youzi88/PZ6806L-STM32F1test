#ifndef _can_H
#define _can_H

#include "system.h"
							    

#define CAN_RX0_INT_ENABLE 0   //��ʹ���ж�

void CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��
 
u8 CAN_Send_Msg(u8* msg,u8 len);						//��������

u8 CAN_Receive_Msg(u8 *buf);							//��������

void CAN_Test(void);
#endif
