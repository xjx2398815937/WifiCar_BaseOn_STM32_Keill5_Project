#include "led.h"
/************************
*描述：对板子上LED的初始化
*函数名称：
*	LED0（ON） ：开灯
*	LED0（OFF）：关灯
*链接引脚：
*	LED0-->PB5
*	LED1-->PE5
*作者：徐佳兴
*************************/
/*灯的相关引脚的初始化*/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*初始化LED0*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = LED_0_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(LED_0_GPIO_PORT,&GPIO_InitStruct);
	/*初始化LED1*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStruct.GPIO_Pin = LED_1_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(LED_1_GPIO_PORT,&GPIO_InitStruct);	
}
void LED_set(u8 led_no, u8 on)
{
	if(led_no==0)
	{
		if(on)
		   GPIO_ResetBits(GPIOB, GPIO_Pin_5);
		else
			 GPIO_SetBits(GPIOB, GPIO_Pin_5);
	}
	else
	{
		if(on)
		   GPIO_ResetBits(GPIOE, GPIO_Pin_5);
		else
			 GPIO_SetBits(GPIOE, GPIO_Pin_5);
	}
}
