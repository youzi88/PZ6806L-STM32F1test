#ifndef _gui_H
#define _gui_H


#include "system.h"


#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


//根据TFT触摸屏分辨率大小来选择ICON图标的尺寸
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
