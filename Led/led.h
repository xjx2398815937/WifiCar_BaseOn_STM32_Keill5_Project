#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include "sys.h"
/*LED时钟和引脚的声明*/
#define LED_0_GPIO_PIN 		GPIO_Pin_5
#define LED_0_GPIO_PORT		GPIOB
#define LED_0_GPOI_CLK		RCC_APB2Periph_GPIOB

#define LED_1_GPIO_PIN 		GPIO_Pin_5
#define LED_1_GPIO_PORT		GPIOE
#define LED_1_GPOI_CLK		RCC_APB2Periph_GPIOE

#define LED0 PCout(1)// PC1
#define LED1 PCout(3)// PC3	

void LED_Init(void);
void LED_set(u8 led_no, u8 on);


#endif /*LED_H*/
