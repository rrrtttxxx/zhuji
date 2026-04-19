/**
  ******************************************************************************
  * @file    USART/Printf/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "bsp_usart2.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s). 将此函数复制到it.c中                                         */
/******************************************************************************/
u8 flaglag,cmd;
u8	open_flag,open_flag3,open_flag2,open_flag1,open_flag4,id;
extern u8 congji;

extern int SET_H1,SET_M1,SET_H2,SET_M2;
void USART2_IRQHandler(void)
{
		unsigned char data;
#if 1
		 USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    if(USART2->SR & 0x0F)
    {
        // See if we have some kind of error
        // Clear interrupt (do nothing about it!)
       data = USART2->DR;
    }
    else 
		if(USART2->SR & USART_FLAG_RXNE)   //Receive Data Reg Full Flag
    {		//GPIO_SetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
        data = USART2->DR;
				usart2_rcv_buf[usart2_rcv_len++]=data;
			  data=USART_ReceiveData(USART2);//注释的这段用来收取ONENET数据格式为  5AA55AA5  01
				if(flaglag==0&&data=='C') flaglag=1;
				else if(flaglag==1&&data=='M')flaglag=2;
				else if(flaglag==2&&data=='D')flaglag=3;
				else if(flaglag==3&&data=='1'){flaglag=4;cmd=1;}//1
				else if(flaglag==3&&data=='2'){flaglag=4;cmd=2;}
				else if(flaglag==3&&data=='3'){flaglag=4;cmd=3;}
				else if(flaglag==3&&data=='4'){flaglag=4;cmd=4;}
				else if(flaglag==3&&data=='5'){flaglag=4;cmd=5;}
				else if(flaglag==3&&data=='6'){flaglag=4;cmd=6;}
				else if(flaglag==3&&data=='7'){flaglag=4;cmd=7;}
				
				else if(flaglag==4&&cmd==1){SET_H1=(data-0x30)*10;flaglag=5;}
				else if(flaglag==5&&cmd==1){SET_H1+=(data-0x30); flaglag=6;}
				else if(flaglag==6&&cmd==1){SET_M1=(data-0x30)*10;flaglag=7;}
				else if(flaglag==7&&cmd==1){SET_M1+=(data-0x30); flaglag=8;}
				else if(flaglag==8&&cmd==1){SET_H2=(data-0x30)*10;flaglag=9;}
				else if(flaglag==9&&cmd==1){SET_H2+=(data-0x30); flaglag=10;}
				else if(flaglag==10&&cmd==1){SET_M2=(data-0x30)*10;flaglag=11;}
				else if(flaglag==11&&cmd==1){SET_M2+=(data-0x30); flaglag=0;}
				
				else if(flaglag==4&&cmd==2){open_flag=1; flaglag=5;}
				else if(flaglag==5&&cmd==2){open_flag=1; flaglag=6;}
				
				else if(flaglag==4&&cmd==3){open_flag=0; flaglag=5;}
				else if(flaglag==5&&cmd==3){open_flag=0; flaglag=6;}
				
				
				else if(flaglag!=12){flaglag=0; }

    }
		else
		{
				;
		}
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		#endif
}
/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
