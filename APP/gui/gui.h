#ifndef _gui_H
#define _gui_H


#include "system.h"


#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
//#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)


//����TFT�������ֱ��ʴ�С��ѡ��ICONͼ��ĳߴ�
//#define ICON_SIZE_58X58     	//320*480 
#define ICON_SIZE_48X48    	//240*400 
//#define ICON_SIZE_38X38		//240*320 


extern u8 ICON_XLEN;
extern u8 ICON_YLEN;
extern u8 ICON_XFRAM;
extern u8 ICON_YFRAM;
extern u8 ICON_XSPACE;
extern u8 ICON_YSPACE;
extern u8 ICON_SIZE;


#define RETURN_X 40
#define RETURN_Y 30
extern const unsigned char gImage_return[2400];


void LCD_ShowStringTrans(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);


#endif
