#ifndef _rs485_H
#define _rs485_H

#include "system.h"


//ģʽ����
#define RS485_TX_EN		PGout(3)	//485ģʽ����.0,����;1,����.
														 
void RS485_Init(u32 bound);

void RS485_Test(void);
#endif
