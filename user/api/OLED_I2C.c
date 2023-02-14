#include "OLED_I2C.h"
#include "delay.h"
#include "codetab.h"
//初始化IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7); 	//PB6,PB7 输出高
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}
/*********************坐通过I2C总线写一个字节************************************/
void Write_IIC_Byte(u8 IIC_Byte)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(IIC_Byte & 0x80)
		IIC_SDA=1;
		else
		IIC_SDA=0;
		IIC_SCL=1;
		delay_us(4);  
		IIC_SCL=0;
		IIC_Byte<<=1;
	}
	IIC_SDA = 1; //原程序这里有一个拉高SDA，根据OLED的DATASHEET，此句必须去掉。
	IIC_SCL=1;
	delay_us(4);
	IIC_SCL=0;
}
/*********************OLED写数据************************************/ 
void OLED_WrDat(u8 IIC_Data)
{
	IIC_Start();
	IIC_Send_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
	IIC_Send_Byte(0x40);			//write data
	IIC_Wait_Ack();	
	IIC_Send_Byte(IIC_Data);
	IIC_Wait_Ack();
	IIC_Stop();
}
/*********************OLED写命令************************************/
void OLED_WrCmd(u8 IIC_Command)
{
	IIC_Start();
	IIC_Send_Byte(0x78);            //Slave address,SA0=0
	if(IIC_Wait_Ack())
	{
		while(1);
	}
	IIC_Send_Byte(0x00);			//write command
	IIC_Wait_Ack();	
	IIC_Send_Byte(IIC_Command); 
	IIC_Wait_Ack();
	IIC_Stop();
}
/*********************OLED写一个字节************************************/
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
		OLED_WrDat(dat);
	}
	else
	{
		OLED_WrCmd(dat);	
	}
}
/*********************填满画面************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//0页—1页
		OLED_WR_Byte(0x00,0);		//低列起始地址
		OLED_WR_Byte(0x10,0);		//高列起始地址
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(fill_Data,1);
		}
	}
}
/*********************坐标设置************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	  
/*********************开启OLED显示************************************/   
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
/*********************关闭OLED显示************************************/      
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
/*********************清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!************************************/	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
			OLED_WR_Byte(0,OLED_DATA); 
	}//更新显示
}
void OLED_On(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x>Max_Column-1)
	{
		x=0;
		y=y+2;
	}
	if(Char_Size ==16)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
	}
	else
	{	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
			OLED_WR_Byte(F6x8[c][i],OLED_DATA);
	}
}
/*********************m^n函数************************************/
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}
			else
				enshow=1; 	 	 
		}
		OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
/*********************显示一个字符号串************************************/
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{
		OLED_ShowChar(x,y,chr[j],Char_Size);
		x+=8;
		if(x>120)
		{
			x=0;
			y+=2;
		}
		j++;
	}
}
/*********************显示汉字************************************/
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);	
	for(t=0;t<16;t++)
	{
		OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
		adder+=1;
	}	
	OLED_Set_Pos(x,y+1);	
	for(t=0;t<16;t++)
	{	
		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
		adder+=1;
	}					
}
void OLED_Init(void)
{
	delay_ms(100); //这里的延时很重要
    OLED_WR_Byte(0xAE,OLED_CMD);//显示关闭
	OLED_WR_Byte(0x00,OLED_CMD);//设置低列地址
	OLED_WR_Byte(0x10,OLED_CMD);//设置高列地址
	OLED_WR_Byte(0x40,OLED_CMD);//设置起始行地址 
	OLED_WR_Byte(0xB0,OLED_CMD);//页面地址设置
	OLED_WR_Byte(0x81,OLED_CMD);//设置对比度控制寄存器
	OLED_WR_Byte(0xFF,OLED_CMD);//设置SEG输出电流亮度（128）
	OLED_WR_Byte(0xA1,OLED_CMD);//设置部分重新映射	0xa0左右反置 0xa1正常 
	OLED_WR_Byte(0xA6,OLED_CMD);//设置正常显示	正常/反向
	OLED_WR_Byte(0xA8,OLED_CMD);//设置多路比(1 ~ 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//设置行扫描方向	0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xD3,OLED_CMD);//设置显示抵消
	OLED_WR_Byte(0x00,OLED_CMD);//设置低列地址	
	OLED_WR_Byte(0xD5,OLED_CMD);//设置显示时钟分比/振荡器频率
	OLED_WR_Byte(0x80,OLED_CMD);//设定分频比，设定时钟为100帧/秒
	OLED_WR_Byte(0xD8,OLED_CMD);//设置区域颜色模式关闭
	OLED_WR_Byte(0x05,OLED_CMD);//	
	OLED_WR_Byte(0xD9,OLED_CMD);//设置pre-charge时期
	OLED_WR_Byte(0xF1,OLED_CMD);//设定预充15个时钟，放电1个时钟	
	OLED_WR_Byte(0xDA,OLED_CMD);//设置com引脚硬件配置
	OLED_WR_Byte(0x12,OLED_CMD);//	
	OLED_WR_Byte(0xDB,OLED_CMD);//设置Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//设置充电泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//设置(0 x10)禁用	
	OLED_WR_Byte(0xAF,OLED_CMD);//打开oled面板
}
