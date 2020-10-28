#ifndef 	__USART_H
#define  	__USART_H
#include "stm32f10x.h"
#include <stdio.h>
/*对时钟和引脚的宏定义*/
#define DEBUG_USARTx 							USART1
#define DEBUG_USART_CLK 						RCC_APB2Periph_USART1
#define DEBUG_USART_APBxCLKCMD 					RCC_APB2PeriphClockCmd
#define DEBUG_USART_BAUDRATE					115200

#define DEBUG_USART_GPIO_CLK 					RCC_APB2Periph_GPIOA
#define DEBUG_USART_GPIO_APBxCLKCMD 			RCC_APB2PeriphClockCmd

#define DEBUG_USART_TX_GPIO_PORT 		GPIOA
#define DEBUG_USART_TX_GPIO_PIN			GPIO_Pin_9
#define DEBUG_USART_RX_GPIO_PORT 		GPIOA
#define DEBUG_USART_RX_GPIO_PIN			GPIO_Pin_10
/*相关函数的声明*/
void USART_Config(void);
void UsartSentByte(USART_TypeDef* pUSARTx,uint8_t data);
void UsartSentHarfWord(USART_TypeDef* pUSARTx,uint16_t data);
void UsartSentArray(USART_TypeDef* pUSARTx,uint8_t *data,uint8_t num);
void UsartSentStr(USART_TypeDef* pUSARTx,uint8_t *str);
#endif /*__USART_H*/

