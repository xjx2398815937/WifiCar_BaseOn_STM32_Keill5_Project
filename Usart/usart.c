#include "usart.h"
/************************
*描述:对板子上USART1的初始化
*链接引脚：
*	USART1_TX-->PA9
*	USART1_RX-->PA10
*作者：徐佳兴
*************************/
/*对串口进行初始化*/
void USART_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//打开串口时钟
	DEBUG_USART_APBxCLKCMD(DEBUG_USART_CLK,ENABLE);
	//打开GPIO的时钟
	DEBUG_USART_GPIO_APBxCLKCMD(DEBUG_USART_GPIO_CLK, ENABLE);
	//配置对用的TX引脚
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN	;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//发送配置成复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	//配置对应的RX引脚
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN	;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//配置成复用浮空输入
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode =	USART_Mode_Rx| USART_Mode_Tx;//两个模式一起用“|”或
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	USART_Cmd(DEBUG_USARTx,ENABLE);
	
}
/*串口发送一个字节*/
void UsartSentByte(USART_TypeDef* pUSARTx,uint8_t data)
{
	USART_SendData(pUSARTx,data);
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);
}
/*发送两个字节*/
void UsartSentHarfWord(USART_TypeDef* pUSARTx,uint16_t data)
{
	uint8_t  temp_h,temp_l;
	temp_h = (data&0xff00)>>8;
	temp_l = (data&0xff);
	USART_SendData(pUSARTx,temp_h);
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);
	USART_SendData(pUSARTx,temp_l);
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);
}
/*发送一个数组*/
void UsartSentArray(USART_TypeDef* pUSARTx,uint8_t *data,uint8_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	{
		UsartSentByte(pUSARTx,data[i]);
	}
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}
/*发送一个字符串*/
void UsartSentStr(USART_TypeDef* pUSARTx,uint8_t *str)
{
	while(*str != '\0')
	{
		UsartSentByte(pUSARTx,*str);
		str ++;
	}
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}
/*要是用printf()必须重写这个函数*/
int fputc(int ch,FILE *f)
{
	USART_SendData(DEBUG_USARTx,(uint8_t) ch);
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE)==RESET);
	return ch;
}
/*要是用getchar()必须重写这个函数*/
int fgetc(FILE *f)
{
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_RXNE)==RESET);
	return (int) USART_ReceiveData(DEBUG_USARTx);
}
