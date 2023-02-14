#include "stm32f10x.h"
#include "main.h"
#include "delay.h"
#include "OLED_I2C.h"
short temperature;//�¶�
u16 wd=300;//����ֵ
void KEY_GPIO_Config(void)//�������������ʼ��IO��
{
	GPIO_InitTypeDef GPIO_InitStructure;//���ð���I0
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);/*����LED��ص�GPIO����ʱ��*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_Scan2(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)//����ɨ��
{
	if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin)==0)
	{
		delay_us(2);
		return 0;		
	}
	else return 1;	
		
}
int main(void)
{
	delay_init();	    	 //��ʱ������ʼ��	
	KEY_GPIO_Config();
	IIC_Init();	 									//��ʼ��IIC	 
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ� 
 	OLED_Init();									//��ʼ��OLED  
	OLED_Clear();									//��������
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==Bit_SET)
		{
			OLED_ShowString(0,0,"hello world xxxx",16);
		}
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)==Bit_SET)
		{
			OLED_ShowString(0,0,"hello world xxxx",16);
		}
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)==Bit_SET)
		{
			OLED_ShowString(0,0,"hello world xxxx",16);
		}
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)==Bit_SET)
		{
			OLED_ShowString(0,0,"hello world xxxx",16);
		}
			///��������
//		 if(wd<temperature) GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_RESET);//0��ʼ����
//		 else GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_SET);//1ֹͣ����
	} 
}
