#include "touch_key.h"
#include "SysTick.h"
#include "usart.h"

#define Touch_ARR_MAX_VAL 0xffff  //����ARRֵ	
u16 touch_default_val=0;  //Ϊ���´�������ʱ��ֵ

/*******************************************************************************
* �� �� ��         : TIM5_CH2_Input_Init
* ��������		   : TIM5_CH2���벶���ʼ������
* ��    ��         : arr���Զ���װ��ֵ
					 psc��Ԥ��Ƶϵ��
* ��    ��         : ��
*******************************************************************************/
void TIM5_CH2_Input_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);//ʹ��TIM5ʱ��
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	 //��������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // PA0
	
	
	TIM_TimeBaseInitStructure.TIM_Period=arr;   //�Զ�װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //�������ϼ���ģʽ
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);	
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_2; //ͨ��2
	TIM_ICInitStructure.TIM_ICFilter=0x00;  //�˲�
	TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//������
	TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1; //��Ƶϵ��
	TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;//ֱ��ӳ�䵽TI1
	TIM_ICInit(TIM5,&TIM_ICInitStructure);
		
	TIM_Cmd(TIM5,ENABLE); //ʹ�ܶ�ʱ��
}

/*******************************************************************************
* �� �� ��         : Touch_Reset
* ��������		   : ����������λ �ȷŵ�Ȼ���粢�ͷż�ʱ���ڵ�ֵ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Touch_Reset(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //�������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);//���0,�ŵ�

	delay_ms(5);
	TIM_ClearFlag(TIM5, TIM_FLAG_CC2|TIM_FLAG_Update); //�����־
	TIM_SetCounter(TIM5,0);		//��0
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	 //��������ģʽ	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* �� �� ��         : Touch_Get_Val
* ��������		   : ���ز���ߵ�ƽֵ
* ��    ��         : ��
* ��    ��         : ����ߵ�ƽֵ
*******************************************************************************/
u16 Touch_Get_Val(void)
{
	Touch_Reset();
	while(TIM_GetFlagStatus(TIM5,TIM_FLAG_CC2)==0) //�ȴ����񵽸ߵ�ƽ��־
	{
		if(TIM_GetCounter(TIM5)>Touch_ARR_MAX_VAL-500)  //��ʱ��ֱ�ӷ���CNTֵ
		{
			return TIM_GetCounter(TIM5);
		}
	}
	return TIM_GetCapture2(TIM5); //���ز���ߵ�ƽֵ
}

/*******************************************************************************
* �� �� ��         : Touch_Key_Init
* ��������		   : ����������ʼ��
* ��    ��         : ��
* ��    ��         : 0������
					 1��������
*******************************************************************************/
u8 Touch_Key_Init(u8 psc)  
{
	u8 i;
	u16 buf[10];
	u8 j;
	u16 temp;
	TIM5_CH2_Input_Init(Touch_ARR_MAX_VAL,psc);
	
	for(i=0;i<10;i++) //��ȡ10��Ϊ����ʱ��Ĵ���ֵ
	{
		buf[i]=Touch_Get_Val();
		delay_ms(10);
	}
	
	for(i=0;i<9;i++)   //��С��������
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])
			{
				temp=buf[i];
				buf[j]=buf[j];
				buf[j]=temp;
			}
		}
	}
	
	temp=0;
	for(i=2;i<8;i++)  //ȡ�м�6����ֵ��� ȡ��ƽ����
	{
		temp+=buf[i];
	}
	touch_default_val=temp/6;
	printf("touch_default_val=%d \r\n",touch_default_val);
	if(touch_default_val>Touch_ARR_MAX_VAL/2)
	{
		return 1;//��ʼ����������Touch_ARR_MAX_VAL/2����ֵ,������!
	}
	return 0;
}

/*******************************************************************************
* �� �� ��         : Touch_Get_MaxVal
* ��������		   : ��ȡn��,ȡ���ֵ
* ��    ��         : n��������ȡ�Ĵ���
* ��    ��         : n�ζ������������������ֵ
*******************************************************************************/
u16 Touch_Get_MaxVal(u8 n)
{
	u16 temp=0;
	u16 res=0; 
	while(n--)
	{
		temp=Touch_Get_Val();//�õ�һ��ֵ
		if(temp>res)res=temp;
	}
	return res;
}  

/*******************************************************************************
* �� �� ��         : Touch_Key_Scan
* ��������		   : ��������ɨ��
* ��    ��         : 0����֧����������(����һ�α����ɿ����ܰ���һ��)
					 1��֧����������(����һֱ����)
* ��    ��         : 0��û�а���
					 1���а���
*******************************************************************************/										  
#define TOUCH_GATE_VAL 	100	//����������ֵ,Ҳ���Ǳ������tpad_default_val+TOUCH_GATE_VAL,����Ϊ����Ч����.
u8 Touch_Key_Scan(u8 mode)
{
	static u8 keyen=0;	//0,���Կ�ʼ���;>0,�����ܿ�ʼ���	 
	u8 res=0;
	u8 sample=3;		//Ĭ�ϲ�������Ϊ3��	 
	u16 rval;
	if(mode)
	{
		sample=6;	//֧��������ʱ�����ò�������Ϊ6��
		keyen=0;	//֧������	  
	}
	rval=Touch_Get_MaxVal(sample); 
	if(rval>(touch_default_val+TOUCH_GATE_VAL)&&rval<(10*touch_default_val))//����touch_default_val+TPAD_GATE_VAL,��С��10��touch_default_val,����Ч
	{							 
		if((keyen==0)&&(rval>(touch_default_val+TOUCH_GATE_VAL)))	//����touch_default_val+TOUCH_GATE_VAL,��Ч
		{
			res=1;
		}	   
		printf("�����󲶻�ߵ�ƽֵΪ��%d\r\n",rval);		     	    					   
		keyen=3;				//����Ҫ�ٹ�3��֮����ܰ�����Ч   
	} 
	if(keyen)keyen--;		   							   		     	    					   
	return res;
}	 
