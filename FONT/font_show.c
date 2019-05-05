#include "font_show.h"
#include "ff.h" 
#include "flash.h"
#include "tftlcd.h" 
#include "malloc.h"




void FontUpdate(uint8_t updateState)
{
    FRESULT res;
	FIL fsrc;
	UINT  br;
    uint32_t wordAddr, i, j;

#ifdef _malloc_H	
    uint8_t *p;
    p = mymalloc(0,4096);                  //����һ���ڴ��
    if(p == 0)
    {
        return;
    }
#else
	uint8_t buffer[512];
#endif    	

    /* ����ASCII�ֿ� */
    if((updateState & GUI_UPDATE_ASCII) == GUI_UPDATE_ASCII)
    {
        /* ����д����ʼ��ַ */
        wordAddr = GUI_FLASH_ASCII_ADDR;
        j = 0;

        /* �򿪶�ȡ�ļ� */
        res = f_open(&fsrc, GUI_ASCII_FILE, FA_READ);	
    	if(res == FR_OK)  //�򿪳ɹ�
        { 
         	for (;;)      //��ʼ��ȡ����
         	{       
#ifdef _malloc_H	
                res = f_read(&fsrc, p, 4096, &br);
    
                /* ����ȡ��������д��FLASH */
                EN25QXX_Write(p, wordAddr, br);
          	    wordAddr += br;   //д���ַ����

#else
                res = f_read(&fsrc, buffer, sizeof(buffer), &br);
    
                /* ����ȡ��������д��FLASH */
                EN25QXX_Write(buffer, wordAddr, br);
          	    wordAddr += br;   //д���ַ����
#endif
                j += br;
                i = j * 100 / 1456;
                LCD_Fill(0, 80, i, 90, RED);    

                if (res || br == 0)
    			{
    				break;    // error or eof 
    			}
            } 
        }
    	 
        f_close(&fsrc);  //�����Ǵ򿪣������½��ļ���һ���ǵùر�
    }

    /* ����12�ź��ֿ� */
    if((updateState & GUI_UPDATE_12CHAR) == GUI_UPDATE_12CHAR)
    {
        wordAddr = GUI_FLASH_12CHAR_ADDR;
        j = 0;

        res = f_open(&fsrc, GUI_12CHAR_FILE, FA_READ);
    	
    	if(res == FR_OK) 
        {  
         	for (;;)  
         	{      
#ifdef _malloc_H	
                res = f_read(&fsrc, p, 4096, &br);
    
                /* ����ȡ��������д��FLASH */
                EN25QXX_Write(p, wordAddr, br);
          	    wordAddr += br;   //д���ַ����
#else
                res = f_read(&fsrc, buffer, sizeof(buffer), &br);
    
                EN25QXX_Write(buffer, wordAddr, br);
          	    wordAddr += br;
#endif
                j += br;
                i = j * 100 / 766080;
                LCD_Fill(0, 95, i, 105, RED);    

    
                if (res || br == 0)
    			{
    				break;    // error or eof 
    			}
            } 
        }
        f_close(&fsrc);  //�����Ǵ򿪣������½��ļ���һ���ǵùر�
    }
    
    /* ����16�ź��ֿ� */
    if((updateState & GUI_UPDATE_16CHAR) == GUI_UPDATE_16CHAR)
    {
        
        wordAddr = GUI_FLASH_16CHAR_ADDR;
        j = 0;

        res = f_open(&fsrc, GUI_16CHAR_FILE, FA_READ);	
    	if(res == FR_OK) 
        { 
         	for (;;)  
         	{       
#ifdef _malloc_H	
                res = f_read(&fsrc, p, 4096, &br);
    
                /* ����ȡ��������д��FLASH */
                EN25QXX_Write(p, wordAddr, br);
          	    wordAddr += br;   //д���ַ����
#else
                res = f_read(&fsrc, buffer, sizeof(buffer), &br);
    
                EN25QXX_Write(buffer, wordAddr, br);
          	    wordAddr += br;
#endif
                j += br;
                i = j * 100 / 1508220;
                LCD_Fill(0, 110, i, 120, RED);    

    
                if (res || br == 0)
    			{
    				break;    // error or eof 
    			}
            } 
        }
    	 
        f_close(&fsrc);  //�����Ǵ򿪣������½��ļ���һ���ǵùر�
    }
#ifdef _malloc_H	
    myfree(0,p);
#endif
} 


void LCD_ShowFont12Char(uint16_t x, uint16_t y, uint8_t *ch)
{
    uint8_t i, j, color, buf[32];
    uint16_t asc;
    uint32_t wordAddr = 0;

    while(*ch != '\0')
    {
        /*��ʾ��ĸ��ASCII���� */
        if(*ch < 0x80)  //ASCII���0~127
        {
            /* ���ֿ��е�ASCII���Ǵӿո�ʼ��Ҳ����32��ʼ�ģ����Լ�ȥ32 */
    		wordAddr = *ch - 32;
            wordAddr *= 16;
            wordAddr += GUI_FLASH_ASCII_ADDR;
            
            /* ��ȡFLASH�и��ֵ���ģ */
            EN25QXX_Read(buf, wordAddr, 16);
            
            /* ��ʾ������ */		
            LCD_Set_Window(x, y, x+7, y+15);           //�ֿ�*��Ϊ��8*16
    		for (j=0; j<16; j++) //ÿ����ģһ����16���ֽ�
    		{
    			color = buf[j];
    			for (i=0; i<8; i++) 
    			{
    				if ((color&0x80) == 0x80)
    				{
    					LCD_WriteData_Color(FRONT_COLOR);
    				} 						
    				else
    				{
    					LCD_WriteData_Color(BACK_COLOR);
    				} 	
    				color <<= 1;
    			}
    		}
    
    		ch++;    //ָ��ָ����һ����
    		
            /* ��Ļ���괦�� */
            x += 8;
            if(x > tftlcd_data.width -8)   //TFT_XMAX -8
            {
                x = 0;
                y += 16;    
            }            
        }
        /* ��ʾ���֣�GBK���� */
        else
        {
            /* �����ֱ���ת������FLASH�еĵ�ַ */
            asc = *ch - 0x81;     //���ֽ��Ǳ�ʾ�����������Ǵ�0x81��0xFE,����ת���ɵ�ַ-0x80
            wordAddr = asc * 190; //ÿ������һ����190����
    
            asc = *(ch + 1); //���ֽڴ���ÿ������ÿ��������λ�ã����Ǵ�0x40��0xFF
            if(asc < 0x7F)   //��0x7Fλ���и���λ����������ȡģ�����գ����Դ���0x7F֮����һ
            {
                asc -= 0x40;
            }
            else
            {
                asc -= 0x41;
            }
            
            wordAddr += asc; //�����GBK���ǵڼ�����
            wordAddr *= 32;  //����λ��ת��λFLASH��ַ
            wordAddr += GUI_FLASH_12CHAR_ADDR; //�����׵�ַ
    
            /* ��ȡFLASH�и��ֵ���ģ */
            EN25QXX_Read(buf, wordAddr, 32);
    
            /* �ڲ���������ʾ */
            LCD_Set_Window(x, y, x+15, y+15);
            for(i=0; i<32; i++)
            {
                 
                color = buf[i];            
                for(j=0; j<8; j++) 
        		{
        			if((color & 0x80) == 0x80)
        			{
        				LCD_WriteData_Color(FRONT_COLOR);
        			} 						
        			else
        			{
        				LCD_WriteData_Color(BACK_COLOR);
        			} 
        			color <<= 1;
        		}//for(j=0;j<8;j++)����
            }
    
            /* ��Ļ���괦�� */
            x += 16;
            if(x > tftlcd_data.width -15)   //TFT_XMAX -15
            {
                x = 0;
                y += 16;    
            }
    
            /* д��һ���֣�ÿ������ռ�����ֽ�����+2 */
            ch += 2;             
        }
    }    
}


void LCD_ShowFont16Char(uint16_t x, uint16_t y, uint8_t *cn )
{   
    uint8_t i, j, color, buf[63];
    uint16_t asc;
    uint32_t wordAddr = 0;    
    while(*cn != '\0')
    {  
        /* �����ֱ���ת������FLASH�еĵ�ַ */
        asc = *cn - 0x81;     //���ֽ��Ǳ�ʾ�����������Ǵ�0x81��0xFE,����ת���ɵ�ַ-0x80
        wordAddr = asc * 190; //ÿ������һ����190����

        asc = *(cn + 1); //���ֽڴ���ÿ������ÿ��������λ�ã����Ǵ�0x40��0xFF
        if(asc < 0x7F)   //��0x7Fλ���и���λ����������ȡģ�����գ����Դ���0x7F֮����һ
        {
            asc -= 0x40;
        }
        else
        {
            asc -= 0x41;
        }
        
        wordAddr += asc; //�����GBK���ǵڼ�����
        wordAddr *= 63;  //����λ��ת��λFLASH��ַ
        wordAddr += GUI_FLASH_16CHAR_ADDR; //�����׵�ַ

        /* ��ȡFLASH�и��ֵ���ģ */
        EN25QXX_Read(buf, wordAddr, 63);

        /* �ڲ���������ʾ */
        LCD_Set_Window(x, y, x+23, y+20);
        for(i=0; i<63; i++)
        {
            color = buf[i];            
            for(j=0; j<8; j++) 
    		{
    			if((color & 0x80) == 0x80)
    			{
    				LCD_WriteData_Color(FRONT_COLOR);
    			} 						
    			else
    			{
    				LCD_WriteData_Color(BACK_COLOR);
    			} 
    			color <<= 1;
    		}//for(j=0;j<8;j++)����
        }

        /* ��Ļ���괦�� */
        x += 21;
        if(x > tftlcd_data.width -21)   //TFT_XMAX -21
        {
            x = 0;
            y += 21;    
        }

        /* д��һ���֣�ÿ������ռ�����ֽ�����+2 */
        cn += 2;      
    }    
}
