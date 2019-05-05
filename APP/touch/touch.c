#include "touch.h"
#include "24cxx.h"
#include "SysTick.h"
#include "tftlcd.h"
#include "spi.h"


#define TOUCH_AdjDelay500ms() delay_ms(500)

TouchTypeDef TouchData;         //���������洢��ȡ��������
static PosTypeDef TouchAdj;     //����һ��������������У������



void TOUCH_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    /* SPI��IO�ں�SPI�����ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    /* TOUCH-CS��IO������ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* TOUCH-PEN��IO������ */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(GPIOD, &GPIO_InitStructure);

    SPI1_Init();

    /* ʹ��EEPROM���洢У������������ע��֮ǰҪ��ʼ�� */
    /* ����Ƿ���У������ */
    AT24CXX_Init();		//��ʼ��24CXX
	AT24CXX_Read(TOUCH_ADJ_ADDR,&TouchAdj.posState,sizeof(TouchAdj));
	if(TouchAdj.posState != TOUCH_ADJ_OK)
    {
        TOUCH_Adjust(); //У��   
    }
}



uint16_t TOUCH_ReadData(uint8_t cmd)
{
    uint8_t i, j;
    uint16_t readValue[TOUCH_READ_TIMES], value;
    uint32_t totalValue;
	
	/* SPI���ٶȲ��˹��� */
    SPI1_SetSpeed(SPI_BaudRatePrescaler_16);
	
    /* ��ȡTOUCH_READ_TIMES�δ���ֵ */
    for(i=0; i<TOUCH_READ_TIMES; i++)
    {   /* ��Ƭѡ */
        TCS=0;
        /* �ڲ��ģʽ�£�XPT2046ת����Ҫ24��ʱ�ӣ�8��ʱ���������֮��1��ʱ��ȥ�� */
        /* æ�źţ��������12λת�������ʣ��3��ʱ���Ǻ���λ */    
        SPI1_ReadWriteByte(cmd); // �������ѡ��X�����Y�� 
        
        /* ��ȡ���� */
        readValue[i] = SPI1_ReadWriteByte(0xFF);
        readValue[i] <<= 8;
        readValue[i] |= SPI1_ReadWriteByte(0xFF);
        
        /* �����ݴ�����ȡ����ADֵ��ֻ��12λ�������λ���� */
        readValue[i] >>= 3;
        
        TCS=1;
    }

    /* �˲����� */
    /* ���ȴӴ�С���� */
    for(i=0; i<(TOUCH_READ_TIMES - 1); i++)
    {
        for(j=i+1; j<TOUCH_READ_TIMES; j++)
        {
            /* ����ֵ�Ӵ�С�������� */
            if(readValue[i] < readValue[j])
            {
                value = readValue[i];
				readValue[i] = readValue[j];
				readValue[j] = value;
            }   
        }       
    }
   
    /* ȥ�����ֵ��ȥ����Сֵ����ƽ��ֵ */
    j = TOUCH_READ_TIMES - 1;
    totalValue = 0;
    for(i=1; i<j; i++)     //��y��ȫ��ֵ
    {
        totalValue += readValue[i];
    }
    value = totalValue / (TOUCH_READ_TIMES - 2);
      
    return value;
}

uint8_t TOUCH_ReadXY(uint16_t *xValue, uint16_t *yValue)
{   
    uint16_t xValue1, yValue1, xValue2, yValue2;

    xValue1 = TOUCH_ReadData(TOUCH_X_CMD);
    yValue1 = TOUCH_ReadData(TOUCH_Y_CMD);
    xValue2 = TOUCH_ReadData(TOUCH_X_CMD);
    yValue2 = TOUCH_ReadData(TOUCH_Y_CMD);
    
    /* �鿴������֮���ֻ����ֵ��� */
    if(xValue1 > xValue2)
    {
        *xValue = xValue1 - xValue2;
    }
    else
    {
        *xValue = xValue2 - xValue1;
    }

    if(yValue1 > yValue2)
    {
        *yValue = yValue1 - yValue2;
    }
    else
    {
        *yValue = yValue2 - yValue1;
    }

    /* �жϲ�����ֵ�Ƿ��ڿɿط�Χ�� */
	if((*xValue > TOUCH_MAX+0) || (*yValue > TOUCH_MAX+0))  
	{
		return 0xFF;
	}

    /* ��ƽ��ֵ */
    *xValue = (xValue1 + xValue2) / 2;
    *yValue = (yValue1 + yValue2) / 2;

    /* �жϵõ���ֵ���Ƿ���ȡֵ��Χ֮�� */
    if((*xValue > TOUCH_X_MAX+0) || (*xValue < TOUCH_X_MIN) 
       || (*yValue > TOUCH_Y_MAX+0) || (*yValue < TOUCH_Y_MIN))
    {                   
        return 0xFF;
    }
 
    return 0; 
}

uint8_t TOUCH_ReadAdjust(uint16_t x, uint16_t y, uint16_t *xValue, uint16_t *yValue)
{
    uint8_t i;
    uint32_t timeCont;

    /* ��ȡУ��������� */
    LCD_Clear(BACK_COLOR);
    LCD_DrowSign(x, y, RED);
    i = 0;
    while(1)
    {
        if(!TOUCH_ReadXY(xValue, yValue))
        {
            i++;
            if(i > 10)         //��ʱһ�£��Զ�ȡ���ֵ
            {
                LCD_DrowSign(x, y, BACK_COLOR);
                return 0;
            }
               
        }
		timeCont++;
        /* ��ʱ�˳� */
        if(timeCont > 0xFFFFFFFE)
        {   
                LCD_DrowSign(x, y, BACK_COLOR); 
                return 0xFF;
        } 
    }       
}

void TOUCH_Adjust(void)
{
    uint16_t px[2], py[2], xPot[4], yPot[4];
    float xFactor, yFactor;

    /* ��ȡ��һ���� */
    if(TOUCH_ReadAdjust(LCD_ADJX_MIN, LCD_ADJY_MIN, &xPot[0], &yPot[0]))
    {
        return;
    }   
    TOUCH_AdjDelay500ms();

    /* ��ȡ�ڶ����� */
    if(TOUCH_ReadAdjust(LCD_ADJX_MIN, LCD_ADJY_MAX, &xPot[1], &yPot[1]))
    {
        return;
    }   
    TOUCH_AdjDelay500ms();

    /* ��ȡ�������� */
    if(TOUCH_ReadAdjust(LCD_ADJX_MAX, LCD_ADJY_MIN, &xPot[2], &yPot[2]))
    {
        return;
    }   
    TOUCH_AdjDelay500ms();

    /* ��ȡ���ĸ��� */
    if(TOUCH_ReadAdjust(LCD_ADJX_MAX, LCD_ADJY_MAX, &xPot[3], &yPot[3]))
    {
        return;
    }   
    TOUCH_AdjDelay500ms();
    
    /* �����ȡ�����ĸ�������ݣ����ϳɶԽǵ������� */
    px[0] = (xPot[0] + xPot[1]) / 2;
    py[0] = (yPot[0] + yPot[2]) / 2;
    px[1] = (xPot[3] + xPot[2]) / 2;
    py[1] = (yPot[3] + yPot[1]) / 2;

    /* ����������� */
    xFactor = (float)LCD_ADJ_X / (px[1] - px[0]);
    yFactor = (float)LCD_ADJ_Y / (py[1] - py[0]);  
    
    /* ���ƫ���� */
    TouchAdj.xOffset = (int16_t)LCD_ADJX_MAX - ((float)px[1] * xFactor);
    TouchAdj.yOffset = (int16_t)LCD_ADJY_MAX - ((float)py[1] * yFactor);

    /* �����������������ݴ���Ȼ�󱣴� */
    TouchAdj.xFactor = xFactor ;
    TouchAdj.yFactor = yFactor ;
    
    TouchAdj.posState = TOUCH_ADJ_OK;
    AT24CXX_Write(TOUCH_ADJ_ADDR, &TouchAdj.posState, sizeof(TouchAdj));            
}


uint8_t TOUCH_Scan(void)
{
    
//    if(PEN == 0)   //�鿴�Ƿ��д���
    {
        if(TOUCH_ReadXY(&TouchData.x, &TouchData.y)) //û�д���
        {
            return 0xFF;    
        }
        /* ������������ֵ���������������ֵ */
        TouchData.lcdx = TouchData.x * TouchAdj.xFactor + TouchAdj.xOffset;
        TouchData.lcdy = TouchData.y * TouchAdj.yFactor + TouchAdj.yOffset;
        
        /* �鿴��������ֵ�Ƿ񳬹�������С */
        if(TouchData.lcdx > tftlcd_data.width)
        {
            TouchData.lcdx = tftlcd_data.width;
        }
        if(TouchData.lcdy > tftlcd_data.height)
        {
            TouchData.lcdy = tftlcd_data.height;
        }
        return 0; 
    }
 //   return 0xFF;       
}







