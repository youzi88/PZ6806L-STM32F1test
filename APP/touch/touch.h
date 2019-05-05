#ifndef _touch_H
#define _touch_H

#include "system.h"
#include "tftlcd.h"


/* ������������ */
typedef struct 
{
	uint16_t x;
	uint16_t y;
    uint16_t lcdx;
    uint16_t lcdy;
} TouchTypeDef;

extern TouchTypeDef TouchData;

typedef struct{
    uint8_t posState;   
    int16_t xOffset;
    int16_t yOffset; 
    float xFactor;
    float yFactor;
} PosTypeDef;


#define TOUCH_ADJ_OK          'Y'              //��ʾ����У������׼����
#define TOUCH_ADJ_ADDR        200          //У��������24C02�е��׵�ַ200
/* ����У���������� */
#define LCD_ADJX_MIN (10)                      //��ȡ�ĸ������СXֵ
#define LCD_ADJX_MAX (tftlcd_data.width - LCD_ADJX_MIN) //��ȡ�ĸ�������Xֵ
#define LCD_ADJY_MIN (10)                      //��ȡ�ĸ������СYֵ
#define LCD_ADJY_MAX (tftlcd_data.height - LCD_ADJY_MIN) //��ȡ�ĸ�������Yֵ

#define LCD_ADJ_X (LCD_ADJX_MAX - LCD_ADJY_MIN)//��ȡ����Ŀ��
#define LCD_ADJ_Y (LCD_ADJY_MAX - LCD_ADJY_MIN)//��ȡ����ĸ߶�

#define TOUCH_READ_TIMES 40     //һ�ζ�ȡ����ֵ�Ĵ���

#define TOUCH_X_CMD      0xD0  //��ȡX������
#define TOUCH_Y_CMD      0x90  //��ȡY������
#define TOUCH_MAX        20    //Ԥ�ڲ�ֵ
#define TOUCH_X_MAX      4000  //X�����ֵ
#define TOUCH_X_MIN      100   //X����Сֵ
#define TOUCH_Y_MAX      4000  //Y�����ֵ
#define TOUCH_Y_MIN      100   //Y����Сֵ


//������оƬ��������	   
#define PEN  		PDin(7)  	//T_PEN
#define TCS  		PDout(6)  	//T_CS 



void TOUCH_Init(void);

void TOUCH_Adjust(void);
uint8_t TOUCH_Scan(void);

#endif
