#ifndef _font_show_H
#define _font_show_H


#include "system.h"


/* �ֿ��ļ���ַ */
#define GUI_ASCII_FILE  "ϵͳ/FONT/ASCII��8X16��.DZK"
#define GUI_12CHAR_FILE "ϵͳ/FONT/12�����壨16X16��.DZK"
#define GUI_16CHAR_FILE "ϵͳ/FONT/16�����壨24X21��.DZK"


/* �����ֿ��ַ */
#define GUI_FLASH_ASCII_ADDR     6112846  //ASCII�ֿ��׵�ַ��6114304 - 1456 - 2��
#define GUI_FLASH_12CHAR_ADDR    6114304  //12���ֿ��׵�ַ��6880386 - 766080 - 2��
#define GUI_FLASH_16CHAR_ADDR    6880386  //16���ֿ��׵�ַ��8388608 - 1508220 - 2��

/* �����ֿ�ѡ���� */
#define GUI_UPDATE_ASCII         0x01     
#define GUI_UPDATE_12CHAR        0x02
#define GUI_UPDATE_16CHAR        0x04
#define GUI_UPDATE_ALL           0x07


void FontUpdate(uint8_t updateState);
void LCD_ShowFont12Char(uint16_t x, uint16_t y, uint8_t *ch);
void LCD_ShowFont16Char(uint16_t x, uint16_t y, uint8_t *ch);




#endif
