#include "stm32f10x.h"
#include "main.h"
#include "delay.h"
#include "OLED_I2C.h"
short temperature;//温度
u16 wd=300;//报警值
void KEY_GPIO_Config(void)//按键与蜂鸣器初始化IO口
{
	GPIO_InitTypeDef GPIO_InitStructure;//配置按键I0
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);/*开启LED相关的GPIO外设时钟*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_Scan2(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)//按键扫描
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
	delay_init();	    	 //延时函数初始化	
	KEY_GPIO_Config();
	IIC_Init();	 									//初始化IIC	 
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级 
 	OLED_Init();									//初始化OLED  
	OLED_Clear();									//清屏函数
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
			///报警函数
//		 if(wd<temperature) GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_RESET);//0开始报警
//		 else GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_SET);//1停止报警
	} 
}
