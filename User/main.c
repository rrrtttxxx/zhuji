/************************************************************************************
*  Copyright (c), 2014, HelTec Automatic Technology co.,LTD.
*            All rights reserved.
*
* Http:    www.heltec.cn
* Email:   cn.heltec@gmail.com
* WebShop: heltec.taobao.com
*
* File name: main.c
* Project  : HelTec.uvprij
* Processor: STM32F103C8T6
* Compiler : MDK fo ARM
* 
* Author : 小林
* Version: 1.00
* Date   : 2014.4.8
* Email  : hello14blog@gmail.com
* Modification: none
* 
* Description:128*64点阵的OLED显示屏测试文件，仅适用于惠特自动化(heltec.taobao.com)的SD1306驱动IIC通信方式显示屏
*
* Others: none;
*
* Function List:
*	1. int main(void);//主函数
*
* History: none;
*
*************************************************************************************/
#include "stm32f10x.h"
#include "OLED_I2C.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "bsp_usart2.h"
#include "esp8266.h"
#include "dht11.h" 
#include "led_key.h"
#include "yanse.h"
#include "ds1302.h"
#include "ds18b20.h" 
#include "mfrc522.h"
#include "HX711.h"
#include "adxl345.h"

#define  LED_NUM 16
unsigned char WS2812_FLAG;							//WS2812开关信号
#define WS2812_FLAG_ENABLE  0x01  //置1安装
unsigned int  RGB_Timer = 0;
unsigned char RGB_Color = 0;
unsigned char TimeFlag = 0;
/*unsigned char colorful[8][3] =
{
    {0xFF, 0x00, 0x00   },  //红			
    {0x00, 0xFF, 0x00   },  //绿
		{0x00, 0x00, 0xFF   },  //蓝	
    {0x00, 0x00, 0x00   },  //灭	
    {0xFF, 0xFF, 0xFF   },  //白		
    {0xFF, 0x61, 0x00   },  //橙
    {0xFF, 0xFF, 0x00   },  //黄
    {0x00, 0xFF, 0xFF   },  //青
};*/

unsigned char colorful[8][3] =
{
    {255, 255, 255   }, 		
    {220, 220, 220    }, 
		{255, 255, 0  },  
};
//================================================================================

//BY SAGAHDS 20181128
void WS2812_Configuration(unsigned char Enable)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO, ENABLE); // 使能PC端口时钟  
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				//选择对应的引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出   //GPIO_Mode_AF_PP;   //复用推挽输出 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);  					//初始化PC端口
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);	 							//关闭所有IO口	
	
	WS2812_FLAG  |=		WS2812_FLAG_ENABLE; 
	
}

//程序名称:WS_Set_Up(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
//参数:  GPIO_TypeDef *GPIOx :  指定的IO类型
//       GPIO_Pin :  指定IO的类型的相应管脚
//说明: 1码 这里直接使用寄存器操作，避免IO口速度不够,具体计算如下：
//        TIMCLK = 72ZHZ  WSTIM=WSH+WSL = 1.25us ± 600ns 		
//		    1码： H1(0.8us) + L1(0.45us) = 1.25us  误差 ±150ns 
void WS_Set_Up(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
{
	unsigned char j=0;
	GPIOx->BSRR = GPIO_Pin;
	j = 5;
	while(j--) __NOP();
	GPIOx->BRR = GPIO_Pin;
}

//程序名称:WS_Set_Down(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
//参数: GPIO_TypeDef *GPIOx :  指定的IO类型
//      GPIO_Pin :  指定IO的类型的相应管脚
//说明: 发送0码	具体计算如下
//		  TIMCLK = 72ZHZ  WSTIM=WSH+WSL = 1.25us ± 600ns 	
//		  0码： H0(0.4us) + L0(0.85us) = 1.25us  误差 ±150ns 
void WS_Set_Down(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
{
		unsigned char j=0;
	
	  GPIOx->BSRR = GPIO_Pin;
		j= 2;
		while(j--) __NOP();
		
    GPIOx->BRR = GPIO_Pin;
		j= 2;
		while(j--) __NOP();
}

//程序名称:WS_Reset(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
//参数:  GPIO_TypeDef *GPIOx :  指定的IO类型
//       GPIO_Pin :  指定IO的类型的相应管脚
//说明: 将指定的IO设置为0后，延时
void WS_Reset(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
{
		uint8_t j;
	
	  GPIOx->BRR = GPIO_Pin;
		j=80;
		while(j--) __NOP();
}

//程序名称:WS_Set_OneFrame(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin,unsigned long dat)
//功能: 发送一个像素点的24bit数据
//参数: GPIO_TypeDef *GPIOx :  指定的IO类型
//      GPIO_Pin :  指定IO的类型的相应管脚
//			dat  :  要发送的一个像素点的数据(即一个24bit的数据) 
//说明: 发送一个数据
void WS_Set_OneFrame(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin,uint32_t dat)
{
	unsigned char i;
	uint8_t byte = 0;
	
	for(i=0; i<24; i++)    //一个24Bit数据
	{
		byte = ( (dat>>i) & 0x1);
		if( byte == 1 )	 WS_Set_Up(GPIOx,GPIO_Pin);    	// 发送1码	( H(0.8us) + L(0.45us))
		else  WS_Set_Down(GPIOx,GPIO_Pin);	            // 发送0码	  ( H(0.4us) + L(0.85us))
	}
}
 
uint32_t WS_rgb_colour(uint8_t (*color)[3])
{
	uint32_t rgbColor = 0;
	
	rgbColor = (color[0][1]<<16)|(color[0][0]<<8)|(color[0][2]);
  return rgbColor;
}

void WS_Set_AllDate(uint8_t len,uint32_t dat,u8 open,u8 pwm) 
{
  unsigned char i;
  for(i = 0; i <len; i++)	{
		if(open==0){//关灯
			WS_Set_OneFrame(GPIOB,GPIO_Pin_9,0);
		}else{
			WS_Set_OneFrame(GPIOB,GPIO_Pin_9,dat);	
			if(pwm==2){
				WS_Set_OneFrame(GPIOB,GPIO_Pin_9,0X000000);
				WS_Set_OneFrame(GPIOB,GPIO_Pin_9,0X000000);
				WS_Set_OneFrame(GPIOB,GPIO_Pin_9,0X000000);
			}else if(pwm==1){
				WS_Set_OneFrame(GPIOB,GPIO_Pin_9,0X000000);
			}
		}
	}
  WS_Reset(GPIOB,GPIO_Pin_9);
}

uint8_t  WS_LED_Flicker(uint8_t flickCnt,u8 open,u8 pwm)
{
	WS_Set_AllDate(LED_NUM, WS_rgb_colour(&colorful[flickCnt]),open,pwm); 		
	return 1;
}



char start_flag=0,flag,mode_flag,SETM=15,MQ_FLAG1=0,MQ_FLAG2=0,MQ_FLAG3=0,MQ_FLAG4=0,MQ_FLAGR=0;
u8 crc,SETP=20;
int help_flag,err_flag,temp=0,humi=0,ch2o,pm25,huo,sound,yan,huo,kong,guang,shui,co,jiujin,ren,time1_cntr,time2_cntr,num,MONEY,SETY=500,SETG=500,SETC=200,SETJ=250,SETT=35,SETH=50,SETS=500,num1;
int mq1_value =0,mq2_value=0,mq3_value=0,mq4_value=0;
int SET_MQ1 =2000,SET_MQ2=2000,SET_MQ3=2000,SET_MQ4=2000;
extern u8	open_flag,open_flag3,open_flag2,open_flag1,congji,open_flag4,id;
u8 congji=1,num4;

int SEND_FLAG,SET_H1=19,SET_M1=30,SET_H2=9,SET_M2=30,SET1,SET2,SETN;
extern uchar time_data1[7],time_data[7],ACK_flag;
int main(void)
{
	u8 dis[10];
	unsigned char i=0;
	DHT11_Data_TypeDef DHT11_Data;
	//Stm32_Clock_Init(9);	//系统时钟设置
	
	uart_init1(9600);	 	//蓝牙串口初始化为9600
	delay_init(72);	   	 	//延时初始化 
	
	I2C_Configuration();
	OLED_Init();


	OLED_CLS();		//屏幕清除
	OLED_ShowStr(0,0,"connect...",2);				//测试8*16字符
	OLED_ShowStr(0,2,"TP-LINK_FC65",2);
	OLED_ShowStr(0,4,"gg12345678",2);
	USART2_Init();//ESP8266的 115200  wifi  AT
	ESP8266_Init();    //ESP8266初始化
	ESP8266_DevLink(DEVICEID,APIKEY,20);//wifi连接到ONENET

	OLED_Fill(0x00);//全屏灭
	delay_ms(2);
	OLED_ShowStr(0,0,"MAIN:",2);
	
	OLED_ShowStr(0,2,"Set:",2);

	OLED_ShowStr(0,4,"G:",2);

	OLED_ShowStr(0,6,"Mode:",2);
	
	OLED_ShowStr(64,6,"Open:",2);

	uart_init1(115200);	 	//蓝牙串口初始化为9600

	KEY_Init();
	SEND_FLAG=1;
	while(1)
	{	
///////////////////////////////////////////////////////////////////	
		if(KEY1==1){//模式切换按键
			while(KEY1==1);
			if(mode_flag==0){mode_flag=1;}
			else if(mode_flag==1){mode_flag=2;}
			else if(mode_flag==2){mode_flag=3;}
			else if(mode_flag==3){mode_flag=4;}
			else if(mode_flag==4){mode_flag=0;}
		}
		if(KEY2==1){//按模式设置按键1
			while(KEY2==1);
			if(mode_flag==0){
				if(open_flag==1)open_flag=0;
				else open_flag=1;
			}
			if(mode_flag==1)SET_H1++;
			if(mode_flag==2)SET_M1++;
			if(mode_flag==3)SET_H2++;
			if(mode_flag==4)SET_M2++;

		}
		if(KEY3==1){//按模式设置按键2
			while(KEY3==1);
			if(mode_flag==0){
				if(open_flag==1)open_flag=0;
				else open_flag=1;
			}
			if(mode_flag==1)SET_H1--;
			if(mode_flag==2)SET_M1--;
			if(mode_flag==3)SET_H2--;
			if(mode_flag==4)SET_M2--;
		}
		
		dis[0]=mode_flag%10+0x30;
		dis[1]='\0';	
		OLED_ShowStr(40,6,dis,2);				//测试8*16字符
		
		dis[0]=open_flag%10+0x30;
		dis[1]=' ';	
		dis[2]=open_flag1%10+0x30;
		dis[3]='\0';	
		OLED_ShowStr(104,6,dis,2);				//测试8*16字符
		
		dis[0]=SET_H1/10+0x30;
		dis[1]=SET_H1%10+0x30;
		dis[2]='-';
		dis[3]=SET_M1/10+0x30;
		dis[4]=SET_M1%10+0x30;
		dis[5]='\0';	
		OLED_ShowStr(30,2,dis,2);				//测试8*16字符
		
		dis[0]=SET_H2/10+0x30;
		dis[1]=SET_H2%10+0x30;
		dis[2]='-';
		dis[3]=SET_M2/10+0x30;
		dis[4]=SET_M2%10+0x30;
		dis[5]='\0';	
		OLED_ShowStr(80,2,dis,2);				//测试8*16字符
		
		
		dis[0]=guang/1000+0x30;
		dis[1]=guang%1000/100+0x30;
		dis[2]=guang%100/10+0x30;
		dis[3]=guang%10+0x30;
		dis[4]='\0';	
		OLED_ShowStr(30,4,dis,2);				//测试8*16字符
		

		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
		ACK_flag=0;
		usart1_write(USART1, 0xFF ,1);delay_ms(40);//fa给从机
		usart1_write(USART1, SET_H1,1);delay_ms(40);
		usart1_write(USART1, SET_M1,1);delay_ms(40);
		usart1_write(USART1, SET_H2,1);delay_ms(40);
		usart1_write(USART1, SET_M2,1);delay_ms(40);
		usart1_write(USART1, open_flag,1);delay_ms(40);
		crc= 0xFF+SET_H1+SET_H2+SET_M1+SET_M2+open_flag;
		usart1_write(USART1, crc,1);delay_ms(40);
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

		

///////////////////////////////////////////////////////////////////	蓝牙请看UART.H     WIFI请看UART2.c+STM32F10X_IT.c
		num1++;
		if(mode_flag==0&&num1>250){//心跳
			num1=0;
	///////////////////////////////////////////////wifi 数据上送
			if(!(ESP8266_CheckStatus(3))){
				ESP8266_SendDat();
			}else{
				//break;
			}
		}
	}
}
