/**
  ******************************************************************************
  * @file    bsp_usart2.c
  * @author  lhq
  * @version V1.0
  * @date    2017-03-16
  * @brief   初始化uart2，该串口为调试串口
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
  */

#include "global.h"


// 配置USART2接收中断
static void NVIC_USART2_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = macUSART_IRQ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


 /**
  * @brief  USART1 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART2_Config(u32 band)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	macUSART_xClock_FUN(macUSART_CLK, ENABLE);
	macUSART_GPIO_xClock_FUN(macUSART_GPIO_CLK, ENABLE);
	
  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(macUSART_TX_PORT, macUSART_TX_SOURCE, macUSART_TX_AF);

  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(macUSART_RX_PORT, macUSART_RX_SOURCE, macUSART_RX_AF);	
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = macUSART_TX_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(macUSART_TX_PORT, &GPIO_InitStructure);

	/* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = macUSART_RX_PIN;
  GPIO_Init(macUSART_RX_PORT, &GPIO_InitStructure);	

	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate =  band;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(macUSARTx, &USART_InitStructure);
	
	NVIC_USART2_Configuration();
	
	/* 使能串口中断*/
	USART_ITConfig(macUSARTx, USART_IT_RXNE, ENABLE); //开启接收中断
	USART_ITConfig(macUSARTx, USART_IT_IDLE, ENABLE); //开启空闲中断
	
	USART_Cmd(macUSARTx, ENABLE);
}

/// 重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART2, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

/// 重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART2);
}
/*********************************************END OF FILE**********************/


