/********************************************************************
 * M8266HostIf.c
 * .Description
 *     Source file of M8266WIFI Host Interface 
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"	
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"

/***********************************************************************************
 * M8266HostIf_GPIO_SPInCS_nRESET_Pin_Init                                         *
 * Description                                                                     *
 *    To initialise the GPIOs for SPI nCS and nRESET output for M8266WIFI module   *
 *    You may update the macros of GPIO PINs usages for nRESET from brd_cfg.h      *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    None                                                                         *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_GPIO_CS_RESET_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

	//Initial STM32's GPIO for M8266WIFI_SPI_nCS
  M8266WIFI_SPI_nCS_GPIO_RCC_CLOCK_EN;  														// enable nCS GPIO clock
	GPIO_OUTPUT_HIGH(M8266WIFI_SPI_nCS_GPIO,M8266WIFI_SPI_nCS_PIN);		// nCS output high initially	
	GPIO_InitStructure_AS_GPIO_OUTPUT(M8266WIFI_SPI_nCS_GPIO,M8266WIFI_SPI_nCS_PIN);
	
	//Initial STM32's GPIO for M8266WIFI_nRESET
  M8266WIFI_nRESET_GPIO_RCC_CLOCK_EN;  															// enable nRESET GPIO clock
	GPIO_OUTPUT_HIGH(M8266WIFI_nRESET_GPIO,M8266WIFI_nRESET_PIN);		  // nRESET output high initially	
	GPIO_InitStructure_AS_GPIO_OUTPUT(M8266WIFI_nRESET_GPIO,M8266WIFI_nRESET_PIN);
}
/***********************************************************************************
 * M8266HostIf_SPI_Init                                                            *
 * Description                                                                     *
 *    To initialise the SPI Interface for M8266WIFI module                         *
 *    You may update the macros of SPI usages for nRESET from brd_cfg.h            *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    None                                                                         *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
#if defined(MCU_IS_STM32F7XX) 
	SPI_HandleTypeDef hspi;
#endif
void M8266HostIf_SPI_Init(void)
{
////////////////////////////////////////////////////////////////////////////////////
///////////////  STM32F1  //////////////////////////////////////////////////////////
#if 	defined(MCU_IS_STM32F1XX)
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;
#if (M8266WIFI_SPI_INTERFACE_NO == 1)  // if use SPI1 GPIOA5/6/7 for SCK/MISO/MOSI
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 						// enable the SPI1 GPIO CLOCK
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;									// SET GPIOs for SPI1
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);											// Enable GPIO Alternate for SPI1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);						  //  Enable SPI1 CLOCK
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);							//  Start to Reset SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);							//  Stop reseting SPI1	
#else
#error Invalid M8266WIFI_SPI_INTERFACE_NO defined in brd_cfg.h 
#endif

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;    
	SPI_InitStructure.SPI_Mode 					= SPI_Mode_Master;									  
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;									 
	SPI_InitStructure.SPI_CPOL 					= SPI_CPOL_Low;		                   //  !!!! M8266WIFI SPI Works at CPOL=LOW, CPHA=1Edge
	SPI_InitStructure.SPI_CPHA 					= SPI_CPHA_1Edge;	
	SPI_InitStructure.SPI_NSS 					= SPI_NSS_Soft;									
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;									 //  !!!! M8266WIFI SPI Works with MSB First							
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(M8266WIFI_INTERFACE_SPI, &SPI_InitStructure);
	SPI_Cmd(M8266WIFI_INTERFACE_SPI, ENABLE); 															//	Enable SPI Interface
	//M8266HostIf_SPI_ReadWriteByte(0xff);																	//  Start Trasmission	
#ifdef M8266WIFI_SPI_ACCESS_USE_DMA	
	SPI_I2S_DMACmd(M8266WIFI_INTERFACE_SPI, SPI_I2S_DMAReq_Rx, ENABLE);			//  Enable SPI Rx DMA
	SPI_I2S_DMACmd(M8266WIFI_INTERFACE_SPI, SPI_I2S_DMAReq_Tx, ENABLE);			//  Enable SPI Tx DMA
#endif
		
////////////////////////////////////////////////////////////////////////////////////
///////////////  STM32F4  //////////////////////////////////////////////////////////
#elif defined(MCU_IS_STM32F4XX)
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;
#if (M8266WIFI_SPI_INTERFACE_NO == 1)  // if use SPI1 GPIOA5/6/7 for SCK/MISO/MOSI
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 						// enable the SPI1 GPIO CLOCK
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;											// SET GPIOs for SPI1
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);							// Enable GPIO Alternate for SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);						  //  Enable SPI1 CLOCK
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);							//  Start to Reset SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);							//  Stop reseting SPI1	
		
#elif (M8266WIFI_SPI_INTERFACE_NO == 3)    // if use SPI3 GPIOC10/11/12 for SCK/MISO/MOSI
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 						// enable the SPI3 GPIO CLOCK
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;											// SET GPIOs for SPI3
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3);							// Enable GPIO Alternate for SPI3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);						  //  Enable SPI3 CLOCK
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE);							//  Start to Reset SPI3
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, DISABLE);							//  Stop reseting SPI3	
#else
#error Invalid M8266WIFI_SPI_INTERFACE_NO defined in brd_cfg.h 
#endif

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;    
	SPI_InitStructure.SPI_Mode 					= SPI_Mode_Master;									  
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;									 
	SPI_InitStructure.SPI_CPOL 					= SPI_CPOL_Low;		                   //  !!!! M8266WIFI SPI Works at CPOL=LOW, CPHA=1Edge
	SPI_InitStructure.SPI_CPHA 					= SPI_CPHA_1Edge;	
	SPI_InitStructure.SPI_NSS 					= SPI_NSS_Soft;									
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;									 //  !!!! M8266WIFI SPI Works with MSB First							
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(M8266WIFI_INTERFACE_SPI, &SPI_InitStructure);
	SPI_Cmd(M8266WIFI_INTERFACE_SPI, ENABLE); 															//	Enable SPI Interface
	//M8266HostIf_SPI_ReadWriteByte(0xff);																	//  Start Trasmission	
#ifdef M8266WIFI_SPI_ACCESS_USE_DMA	
	SPI_I2S_DMACmd(M8266WIFI_INTERFACE_SPI, SPI_I2S_DMAReq_Rx, ENABLE);			//  Enable SPI Rx DMA
	SPI_I2S_DMACmd(M8266WIFI_INTERFACE_SPI, SPI_I2S_DMAReq_Tx, ENABLE);			//  Enable SPI Tx DMA
#endif

////////////////////////////////////////////////////////////////////////////////////
///////////////  STM32F7  //////////////////////////////////////////////////////////
#elif defined(MCU_IS_STM32F7XX)
  GPIO_InitTypeDef  GPIO_InitStructure;
#if (M8266WIFI_SPI_INTERFACE_NO == 1)    // if use SPI1 GPIOA5/6/7 for SCK/MISO/MOSI
#elif (M8266WIFI_SPI_INTERFACE_NO == 4)  // if use SPI1 GPIOE2/5/6 for SCK/MISO/MOSI
	__HAL_RCC_GPIOE_CLK_ENABLE();					 // Enable the GPIOE Clock
	GPIO_InitStructure.Pin       = GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6;
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull      = GPIO_PULLUP;
  GPIO_InitStructure.Alternate = GPIO_AF5_SPI4;	
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);  
	__HAL_RCC_SPI4_CLK_ENABLE();
	
	hspi.Instance = SPI4;
#else
#error Invalid M8266WIFI_SPI_INTERFACE_NO defined in brd_cfg.h 
#endif

  hspi.Init.Mode 									= SPI_MODE_MASTER;
  hspi.Init.Direction 						= SPI_DIRECTION_2LINES;
  hspi.Init.DataSize 							= SPI_DATASIZE_8BIT;
  hspi.Init.CLKPolarity 					= SPI_POLARITY_LOW;
  hspi.Init.CLKPhase 							= SPI_PHASE_1EDGE;
  hspi.Init.NSS 									= SPI_NSS_SOFT;
  hspi.Init.BaudRatePrescaler 		= SPI_BAUDRATEPRESCALER_8;
  hspi.Init.FirstBit 							= SPI_FIRSTBIT_MSB;
  hspi.Init.TIMode 								= SPI_TIMODE_DISABLE;
  hspi.Init.CRCCalculation 				= SPI_CRCCALCULATION_DISABLE;
  hspi.Init.CRCPolynomial 				= 7;
  hspi.Init.CRCLength 						= SPI_CRC_LENGTH_DATASIZE;
  hspi.Init.NSSPMode 							= SPI_NSS_PULSE_DISABLE; //SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi) != HAL_OK)
  {
    while(1);
  }
  __HAL_SPI_ENABLE(&hspi);                        //使能SPI2

#else
#error YOU SHOULD DEFINED MCU_IS_STM32FX?XX in brd_cfg.h
#endif

}
/***********************************************************************************
 * M8266HostIf_SPI_SetSpeed                                                        *
 * Description                                                                     *
 *    To setup the SPI Clock Speed for M8266WIFI module                            *
 * Parameter(s):                                                                   *
 *    SPI_BaudRatePrescaler: SPI BaudRate Prescaler                                *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_SPI_SetSpeed(u8 SPI_BaudRatePrescaler)
{
#if  defined(MCU_IS_STM32F1XX) || defined(MCU_IS_STM32F4XX)	
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));		// 判断有效性
	M8266WIFI_INTERFACE_SPI->CR1&=0XFFC7;															// 位3-5清零，用来设置时钟
	M8266WIFI_INTERFACE_SPI->CR1|=SPI_BaudRatePrescaler;							// 设置SPI速度 
	SPI_Cmd(M8266WIFI_INTERFACE_SPI,ENABLE); 													// 使能SPIx
#elif defined(MCU_IS_STM32F7XX)
	__HAL_SPI_DISABLE(&hspi);            															//关闭SPI
  hspi.Instance->CR1&=0XFFC7;          															//位3-5清零，用来设置波特率
  hspi.Instance->CR1|=SPI_BaudRatePrescaler;											//设置SPI速度
  __HAL_SPI_ENABLE(&hspi);
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif	
} 

#ifdef M8266WIFI_SPI_ACCESS_USE_DMA
void M8266HostIf_SPI_DMA_Init(void)
{
   DMA_InitTypeDef DMA_InitStructure;    

	 //Initialization of SPI TX DMA
   RCC_AHB1PeriphClockCmd(M8266WIFI_INTERFACE_SPI_TX_RCC_Periph_DMA, ENABLE);  
	 DMA_DeInit(M8266WIFI_INTERFACE_SPI_TX_DMA_STREAM); 
   DMA_InitStructure.DMA_Channel 							= M8266WIFI_INTERFACE_SPI_TX_DMA_CHANNEL;
   DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)(&(M8266WIFI_INTERFACE_SPI->DR));  
   DMA_InitStructure.DMA_Memory0BaseAddr 			= (uint32_t)0;  // use 0 for the moment during initialization
   DMA_InitStructure.DMA_DIR 									= DMA_DIR_MemoryToPeripheral;  
   DMA_InitStructure.DMA_BufferSize 					= 1;            // use 1 for the moment during initialization
   DMA_InitStructure.DMA_PeripheralInc 				= DMA_PeripheralInc_Disable;  
   DMA_InitStructure.DMA_MemoryInc 						= DMA_MemoryInc_Enable;  
   DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;  
   DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte;  
   DMA_InitStructure.DMA_Mode 								= DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority 						= DMA_Priority_Low;  
   DMA_InitStructure.DMA_FIFOMode 						= DMA_FIFOMode_Disable;      
   DMA_InitStructure.DMA_FIFOThreshold 				= DMA_FIFOThreshold_HalfFull;          
   DMA_InitStructure.DMA_MemoryBurst 					= DMA_MemoryBurst_Single;         
   DMA_InitStructure.DMA_PeripheralBurst 			= DMA_PeripheralBurst_Single;   
   DMA_Init(M8266WIFI_INTERFACE_SPI_TX_DMA_STREAM, &DMA_InitStructure);
   //DMA_Cmd(M8266WIFI_INTERFACE_SPI_TX_DMA_STREAM, ENABLE);   // Please DO NOT ENABLE the STREAM during initialization

   //Initialization of SPI RX DMA
	 
//SPI1_RX uses DMA2 Stream2, channel 3
   RCC_AHB1PeriphClockCmd(M8266WIFI_INTERFACE_SPI_RX_RCC_Periph_DMA, ENABLE);  
	 DMA_DeInit(M8266WIFI_INTERFACE_SPI_RX_DMA_STREAM); 
   DMA_InitStructure.DMA_Channel 							= M8266WIFI_INTERFACE_SPI_RX_DMA_CHANNEL;
   DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)(&(M8266WIFI_INTERFACE_SPI->DR));  
   DMA_InitStructure.DMA_Memory0BaseAddr 			= (uint32_t)0;  // use 0 for the moment during initialization
   DMA_InitStructure.DMA_DIR 									= DMA_DIR_PeripheralToMemory;  
   DMA_InitStructure.DMA_BufferSize 					= 1;            // use 1 for the moment during initialization
   DMA_InitStructure.DMA_PeripheralInc 				= DMA_PeripheralInc_Disable;  
   DMA_InitStructure.DMA_MemoryInc 						= DMA_MemoryInc_Enable;  
   DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;  
   DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte;  
   DMA_InitStructure.DMA_Mode 								= DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority 						= DMA_Priority_High;  
   DMA_InitStructure.DMA_FIFOMode 						= DMA_FIFOMode_Disable;      
   DMA_InitStructure.DMA_FIFOThreshold 				= DMA_FIFOThreshold_HalfFull;          
   DMA_InitStructure.DMA_MemoryBurst 					= DMA_MemoryBurst_Single;         
   DMA_InitStructure.DMA_PeripheralBurst 			= DMA_PeripheralBurst_Single;   
   DMA_Init(M8266WIFI_INTERFACE_SPI_RX_DMA_STREAM, &DMA_InitStructure);
	 //DMA_Cmd(M8266WIFI_INTERFACE_SPI_RX_DMA_STREAM, ENABLE);  // Please DO NOT ENABLE the STREAM during initialization
}
#endif


#if defined(M8266WIFI_CONFIG_VIA_USART)
////////////////////////////////////////////////////////////////////////////
// For printf support
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#pragma import(__use_no_semihosting)             
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x;
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 
#if defined(MCU_IS_STM32F1XX) || defined(MCU_IS_STM32F4XX)	
	while((USART_FOR_PRINTF->SR&0X40)==0);//循环发送,直到发送完毕   
	USART_FOR_PRINTF->DR = (u8) ch;      
#elif defined(MCU_IS_STM32F7XX)
#else
#endif
	return ch;
}

/***********************************************************************************
 * M8266HostIf_USART_Init                                                          *
 * Description                                                                     *
 *    To initialise the Host USART interface for M8266WIFI module                  *
 *    e.g. STM32F4 Host Interface -- USART                                         *
 *      - If uses USART1, USART1_TXD=PA9, USART1_RXD=PA10                          *
 *      - If uses USART2, USART2_TXD=PA2, USART2_RXD=PA3                           *
 * Parameter(s):                                                                   *
 *    baud: baud rate to set                                                       *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/ 
void M8266HostIf_USART_Init(u32 baud)
{
  GPIO_InitTypeDef 		GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;
	NVIC_InitTypeDef 		NVIC_InitStructure;

////////////////////////////////////////////////////////////////////////////////////
///////////////  STM32F1  //////////////////////////////////////////////////////////
#if 	defined(MCU_IS_STM32F1XX)
#if (M8266WIFI_USART_INTERFACE_NO==1)	 	// if use USART1 GPIOA9/10
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 						// enable the USART1 GPIO CLOCK
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_9|GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;									// SET GPIOs for USART1
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);											// Enable GPIO Alternate for USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);						//  Enable USART1 CLOCK
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE);						//  Start to Reset USART1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);						//  Stop reseting USART1
#elif (M8266WIFI_USART_INTERFACE_NO==2)	 	// if use USART1 GPIOA2/3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 						// enable the USART2 GPIO CLOCK
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;									// SET GPIOs for USART2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);									  //  Enable GPIO Alternate for USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);						//  Enable USART2CLOCK
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE);						//  Start to Reset USART2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE);						//  Stop reseting USART2
#else
#error Invalid M8266WIFI_SPI_INTERFACE_NO defined in brd_cfg.h 
#endif
	USART_InitStructure.USART_BaudRate 						= baud;
	USART_InitStructure.USART_WordLength 					= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 						= USART_StopBits_1;
	USART_InitStructure.USART_Parity 							= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 								= USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(M8266WIFI_INTERFACE_USART, &USART_InitStructure); 
	
  USART_Cmd(M8266WIFI_INTERFACE_USART, ENABLE);
//USART_ClearFlag(M8266WIFI_INTERFACE_USART, USART_FLAG_TC);
	
	USART_ITConfig(M8266WIFI_INTERFACE_USART, USART_IT_RXNE, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel 										= M8266WIFI_INTERFACE_USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 3;	
	NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

////////////////////////////////////////////////////////////////////////////////////
///////////////  STM32F4  //////////////////////////////////////////////////////////
#elif defined(MCU_IS_STM32F4XX)

#if (M8266WIFI_USART_INTERFACE_NO==1)	 // if use USART1 GPIOA9/10
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  								// Enable GPIOA  CLOCK
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);   		          // Enbale USART1 CLOCK
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;								// SET GPIOs for USART1
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;											
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 											           
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);						// Enable GPIO Alternate for USART1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

#elif (M8266WIFI_USART_INTERFACE_NO==2) 	 // if use USART2 GPIOA2/3
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  								// Enable GPIOA  CLOCK
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);   		          // Enbale USART2 CLOCK
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;											
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 											           
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);						// Enable GPIO Alternate for USART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART3);

#else
#error Invalid M8266WIFI_USART_INTERFACE_NO defined in brd_cfg.h 
#endif

	USART_InitStructure.USART_BaudRate 						= baud;
	USART_InitStructure.USART_WordLength 					= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 						= USART_StopBits_1;
	USART_InitStructure.USART_Parity 							= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 								= USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(M8266WIFI_INTERFACE_USART, &USART_InitStructure); 
	
  USART_Cmd(M8266WIFI_INTERFACE_USART, ENABLE);
//USART_ClearFlag(M8266WIFI_INTERFACE_USART, USART_FLAG_TC);
	
	USART_ITConfig(M8266WIFI_INTERFACE_USART, USART_IT_RXNE, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel 										= M8266WIFI_INTERFACE_USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 3;	
	NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
////////////////////////////////////////////////////////////////////////////////////
///////////////  STM32F7  //////////////////////////////////////////////////////////
#elif defined(MCU_IS_STM32F7XX)

#else
#error YOU SHOULD DEFINED MCU_IS_STM32FX?XX in brd_cfg.h
#endif
}

u8  M8266WIFI_USART_RX_BUF[M8266WIFI_USART_RX_BUF_SIZE/2*3];
u16 uart_rx_head=0; 
u16 uart_rx_tail=0;

#if (M8266WIFI_USART_INTERFACE_NO==1)	 // if use USART1
void USART1_IRQHandler(void)
#else
#if (M8266WIFI_USART_INTERFACE_NO==2) 	 // if use USART2
void USART2_IRQHandler(void)
#else
#error Wrong M8266WIFI_USART_INTERFACE_NO defined in brd_cfg.h 
#endif
#endif	
{
	 u8 Res;
	 if(USART_GetITStatus(M8266WIFI_INTERFACE_USART, USART_IT_RXNE) != RESET) 
	 {
		  Res =USART_ReceiveData(M8266WIFI_INTERFACE_USART); //(USART1->DR);
			M8266WIFI_USART_RX_BUF[uart_rx_head] = Res;
		  if( (++uart_rx_head)>=M8266WIFI_USART_RX_BUF_SIZE) 
			 uart_rx_head = 0;
   } 
} 
u8 M8266HostIf_USART_Has_data_In_RX_BUFFER(void)
{
	if(uart_rx_head==uart_rx_tail)
		return 0;
	return 1;
}
u16 M8266HostIf_USART_RX_BUFFER_Data_count(void)
{
	if(uart_rx_head>=uart_rx_tail)
		return uart_rx_head-uart_rx_tail;
	return uart_rx_head+M8266WIFI_USART_RX_BUF_SIZE-uart_rx_tail;
}
u8 M8266HostIf_USART_RX_BUFFER_FULL(void)
{
	if( (M8266HostIf_USART_RX_BUFFER_Data_count()) == (M8266WIFI_USART_RX_BUF_SIZE-1) )
		return 1;
	return 0;
}
void M8266HostIf_USART_Reset_RX_BUFFER(void)
{
	uart_rx_tail = 0;
	uart_rx_head = 0;
}

u8 M8266HostIf_USART_get_char(void)
{
	u8 ch = M8266WIFI_USART_RX_BUF[uart_rx_tail];
	if( (++uart_rx_tail)>=M8266WIFI_USART_RX_BUF_SIZE) 
			uart_rx_tail = 0;
	return ch;
}
u16 M8266HostIf_USART_get_data(u8* data, u16 len)
{
	u16 i;
	
	if( (i=M8266HostIf_USART_RX_BUFFER_Data_count())<len)
    return 0;
	
	for(i=0; i<len; i++)
	  data[i]=M8266HostIf_USART_get_char();
	
	return i;
}
void M8266HostIf_usart_txd_data(char* data, u16 len)
{
	u16 j;
	for(j=0; j<len; j++)
	{
	   while(USART_GetFlagStatus(M8266WIFI_INTERFACE_USART, USART_FLAG_TC)==RESET);  // wait uart txd buffer free 
		 USART_SendData(M8266WIFI_INTERFACE_USART, (uint8_t)(*(data++)) );
	}
}
void M8266HostIf_usart_txd_string(char* str)
{
	M8266HostIf_usart_txd_data(str, strlen(str));
}

#endif // end to #if defined(M8266WIFI_CONFIG_VIA_USART)

/***********************************************************************************
 * M8266HostIf_Init                                                                *
 * Description                                                                     *
 *    To initialise the Host interface for M8266WIFI module                        *
 * Parameter(s):                                                                   *
 *    baud: baud rate to set                                                       *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/ 
void M8266HostIf_Init(void)
{
	 M8266HostIf_GPIO_CS_RESET_Init();
#ifdef M8266WIFI_CONFIG_VIA_USART	
	 M8266HostIf_USART_Init(115200);
#endif
	 M8266HostIf_SPI_Init();
//	 M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_8);
	
#ifdef M8266WIFI_SPI_ACCESS_USE_DMA
	 M8266HostIf_SPI_DMA_Init();
#endif
	
}

//////////////////////////////////////////////////////////////////////////////////////
// BELOW FUNCTIONS ARE REQUIRED BY M8266WIFIDRV.LIB. 
// PLEASE IMPLEMENTE THEM ACCORDING TO YOUR HARDWARE
//////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
 * M8266HostIf_Set_nRESET_Pin                                                      *
 * Description                                                                     *
 *    To Outpout HIGH or LOW onto the GPIO pin for M8266WIFI nRESET                *
 *    You may update the macros of GPIO PIN usages for nRESET from brd_cfg.h       *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    1. level: LEVEL output to nRESET pin                                         *
 *              0 = output LOW  onto nRESET                                        *
 *              1 = output HIGH onto nRESET                                        *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_Set_nRESET_Pin(u8 level)
{
	  if(level!=0)
		//GPIO_SetBits(M8266WIFI_nRESET_GPIO, M8266WIFI_nRESET_PIN);
#if   defined(MCU_IS_STM32F1XX)
		  M8266WIFI_nRESET_GPIO->BSRR  = M8266WIFI_nRESET_PIN;
#elif defined(MCU_IS_STM32F4XX)
		  M8266WIFI_nRESET_GPIO->BSRRL = M8266WIFI_nRESET_PIN;
#elif defined(MCU_IS_STM32F7XX)
		  M8266WIFI_nRESET_GPIO->BSRR  = M8266WIFI_nRESET_PIN;
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif

		else
		//GPIO_ResetBits(M8266WIFI_nRESET_GPIO, M8266WIFI_nRESET_PIN);
#if   defined(MCU_IS_STM32F1XX)
		  M8266WIFI_nRESET_GPIO->BRR   = M8266WIFI_nRESET_PIN;
#elif defined(MCU_IS_STM32F4XX)
		  M8266WIFI_nRESET_GPIO->BSRRH = M8266WIFI_nRESET_PIN;
#elif defined(MCU_IS_STM32F7XX)
		  M8266WIFI_nRESET_GPIO->BSRR  = M8266WIFI_nRESET_PIN<<16;
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif
}
/***********************************************************************************
 * M8266HostIf_Set_SPI_nCS_PIN                                                     *
 * Description                                                                     *
 *    To Outpout HIGH or LOW onto the GPIO pin for M8266WIFI SPI nCS               *
 *    You may update the macros of GPIO PIN usages for SPI nCS from brd_cfg.h      *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    1. level: LEVEL output to SPI nCS pin                                        *
 *              0 = output LOW  onto SPI nCS                                       *
 *              1 = output HIGH onto SPI nCS                                       *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_Set_SPI_nCS_Pin(u8 level)
{
	  if(level!=0)
		//GPIO_SetBits(M8266WIFI_SPI_nCS_GPIO, M8266WIFI_SPI_nCS_PIN);
#if   defined(MCU_IS_STM32F1XX)
		  M8266WIFI_SPI_nCS_GPIO->BSRR  = M8266WIFI_SPI_nCS_PIN;
#elif defined(MCU_IS_STM32F4XX) 
		  M8266WIFI_SPI_nCS_GPIO->BSRRL = M8266WIFI_SPI_nCS_PIN;
#elif defined(MCU_IS_STM32F7XX) 
		  M8266WIFI_SPI_nCS_GPIO->BSRR  = M8266WIFI_SPI_nCS_PIN;
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif

		else
		//GPIO_ResetBits(M8266WIFI_SPI_nCS_GPIO, M8266WIFI_SPI_nCS_PIN);
#if   defined(MCU_IS_STM32F1XX)
		  M8266WIFI_SPI_nCS_GPIO->BRR   = M8266WIFI_SPI_nCS_PIN;
#elif defined(MCU_IS_STM32F4XX)
		  M8266WIFI_SPI_nCS_GPIO->BSRRH = M8266WIFI_SPI_nCS_PIN;
#elif defined(MCU_IS_STM32F7XX) 
		  M8266WIFI_SPI_nCS_GPIO->BSRR  = M8266WIFI_SPI_nCS_PIN<<16;
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif
}

/***********************************************************************************
 * M8266WIFIHostIf_delay_us                                                        *
 * Description                                                                     *
 *    To loop delay some micro seconds.                                            *
 * Parameter(s):                                                                   *
 *    1. nus: the micro seconds to delay                                           *
 * Return:                                                                         *
 *    none                                                                         *
 ***********************************************************************************/
void M8266HostIf_delay_us(u8 nus)
{
   delay_us(nus);
}

/***********************************************************************************
 * M8266HostIf_SPI_ReadWriteByte                                                   *
 * Description                                                                     *
 *    To write a byte onto the SPI bus from MCU MOSI to the M8266WIFI module       *
 *    and read back a byte from the SPI bus MISO meanwhile                         *
 *    You may update the macros of SPI usage from brd_cfg.h                        *
 * Parameter(s):                                                                   *
 *    1. TxdByte: the byte to be sent over MOSI                                    *
 * Return:                                                                         *
 *    1. The byte read back from MOSI meanwhile                                    *                                                                         *
 ***********************************************************************************/
u8 M8266HostIf_SPI_ReadWriteByte(u8 TxdByte)
{
#if  defined(MCU_IS_STM32F1XX) || defined(MCU_IS_STM32F4XX)	
	while(SPI_I2S_GetFlagStatus(M8266WIFI_INTERFACE_SPI, SPI_I2S_FLAG_TXE) == RESET){} 	//wait SPI TXD Buffer Empty
	SPI_I2S_SendData(M8266WIFI_INTERFACE_SPI, TxdByte); 			//Write the byte to the TXD buffer and then shift out via MOSI
	while(SPI_I2S_GetFlagStatus(M8266WIFI_INTERFACE_SPI, SPI_I2S_FLAG_RXNE) == RESET){} 	//wait the SPI RXD buffer has received the data from MISO
	return SPI_I2S_ReceiveData(M8266WIFI_INTERFACE_SPI);  		//return the data received from MISO
#elif defined(MCU_IS_STM32F7XX)
  u8 Rxdata;
  HAL_SPI_TransmitReceive(&hspi,&TxdByte,&Rxdata,1, 1000);       
 	return Rxdata; 
#else
#error YOU SHOULD DEFINED MCU_IS_STM32F?XX in brd_cfg.h
#endif	
}
