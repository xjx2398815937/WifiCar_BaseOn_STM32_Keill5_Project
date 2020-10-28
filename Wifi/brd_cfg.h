/********************************************************************
 * brd_cfg.h
 * .Description
 *     header file of main board configuration macros 
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
#ifndef _BRG_CFG_H_
#define _BRG_CFG_H_

#define MCU_IS_STM32F1XX

#define M8266WIFI_CONFIG_VIA_SPI


///////////////////////////////////////////////////////////////
// M8266WIFI Module Interface Board Usages Macros
//      -- nRESET, nCS
///////////////////////////////////////////////////////////////
#if   defined(MCU_IS_STM32F1XX)  // STM's STM32F1XX, using GPIOC4 as nRESET
#define  M8266WIFI_nRESET_GPIO_RCC_CLOCK_EN				  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)
#define  M8266WIFI_nRESET_GPIO											GPIOC
#define  M8266WIFI_nRESET_PIN												GPIO_Pin_4
#endif

#if   defined(MCU_IS_STM32F1XX)  // STM's STM32F1XX, using GPIOA4 as nCS
#define  M8266WIFI_SPI_nCS_GPIO_RCC_CLOCK_EN				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)
#define  M8266WIFI_SPI_nCS_GPIO											GPIOA
#define  M8266WIFI_SPI_nCS_PIN											GPIO_Pin_4
#endif

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// M8266WIFI Module Interface Board Usages Macros
//     - MOSI - MISO - CLK
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
//      PIN   GND - nRST - nCS - MOSI - MISO - CLK   
//For STM32
//- If uses SPI1, SPI1_MOSI=GPIOA7,  SPI1_MISO=GPIOA6,  SPI1_SCK=GPIOA5   -- use this currently
//            or  SPI1_MOSI=GPIOB5,  SPI1_MISO=GPIOB4,  SPI1_SCK=GPIOB3                 
//- If uses SPI3, SPI3_MOSI=GPIOC12, SPI3_MISO=GPIOC11, SPI3_SCK=GPIOC10  -- use this currently
//            or  SPI1_MOSI=GPIOB5,  SPI1_MISO=GPIOB4,  SPI1_SCK=GPIOB3,                  


#define  M8266WIFI_SPI_INTERFACE_NO							1

//- If uses USART1, USART1_TXD=PA9, USART1_RXD=PA10
//- If uses USART2, USART2_TXD=PA2, USART2_RXD=PA3   
#define  M8266WIFI_USART_INTERFACE_NO						1 // 1 for USART1, 2 for USART2
#define  M8266WIFI_USART_RX_BUF_SIZE						512

#define  USART_FOR_PRINTF												USART1

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// M8266WIFI Module Interface Board Usages Macros
//     - SPI TX DMA   if DMA used 
//     - SPI RX DMA   if DMA used
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// PLEASE DO NOT MODIFY BELOW UNLESS NECESSITY
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#if defined(MCU_IS_STM32F1XX) || defined(MCU_IS_STM32F4XX) || defined(MCU_IS_STM32F7XX)
#define MCU_IS_STM32
#endif

//////////////////////////////////////////////////////////////////////////////////
#if   defined(MCU_IS_STM32F1XX)  // STM's STM32F1XX, 
#define GPIO_InitStructure_AS_GPIO_OUTPUT(gpio,pin)						\
	GPIO_InitStructure.GPIO_Pin 	= pin;												\
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;						\
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						\
	GPIO_Init(gpio, &GPIO_InitStructure)	
#endif

#if   defined(MCU_IS_STM32F1XX)  // STM's STM32F1XX, 
#define GPIO_OUTPUT_HIGH(gpio,pin)	(gpio)->BSRR=pin
#define GPIO_OUTPUT_LOW(gpio,pin)		(gpio)->BRR=pin
#endif

#if   defined(MCU_IS_STM32F1XX)
#define GPIO_InitStructure_AS_AF(gpio,pins)										\
	GPIO_InitStructure.GPIO_Pin 	= pins;												\
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;						\
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						\
	GPIO_Init(gpio, &GPIO_InitStructure)		
#endif

//To define GPIO_PinRemapSPI0
#if   defined(MCU_IS_STM32F1XX)
#elif defined(MCU_IS_STM32F4XX)
#elif defined(MCU_IS_STM32F7XX)
#elif defined(MCU_IS_NXP_LPC17XX)
#define GPIO_PinRemapSPI0																																										\
						LPC_PINCON->PINSEL0 &= ~(0x03UL << 30); LPC_PINCON->PINSEL0 |=  (0x02UL << 30);									\
						LPC_PINCON->PINSEL1 &= ~(0x03UL <<  2); LPC_PINCON->PINSEL1 |=  (0x02UL <<  2);									\
						LPC_PINCON->PINSEL1 &= ~(0x03UL <<  4); LPC_PINCON->PINSEL1 |=  (0x02UL <<  4);									\
						LPC_PINCON->PINSEL1 &= ~(0x03UL << 0);	
#elif defined(MCU_IS_HT32F16XX)  //Holtek's HT32F16XX
#else
#error YOU SHOULD DEFINED MCU_IS_XXX near line 17 in brd_cfg.h
#endif

//To define GPIO_PinRemapSPI1
#if   defined(MCU_IS_STM32F1XX)
#define GPIO_PinRemapSPI1																																	\
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
#endif

//To define GPIO_PinRemapSPI3
#if   defined(MCU_IS_STM32F1XX)
#define GPIO_PinRemapSPI3																				\
	GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);
#endif

#if   defined(MCU_IS_STM32F1XX)
#define GPIO_PinRemapUSART1																				\
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
#endif

#if   defined(MCU_IS_STM32F1XX)
#define GPIO_PinRemapUSART2																				\
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
#endif


#if   defined(MCU_IS_STM32)&&(M8266WIFI_SPI_INTERFACE_NO == 1)  // for STM32
  #define  M8266WIFI_INTERFACE_SPI								SPI1
#elif (M8266WIFI_SPI_INTERFACE_NO == 2)
  #define  M8266WIFI_INTERFACE_SPI								SPI2
#elif (M8266WIFI_SPI_INTERFACE_NO == 3)
  #define  M8266WIFI_INTERFACE_SPI								SPI3
#elif (M8266WIFI_SPI_INTERFACE_NO == 4)
  #define  M8266WIFI_INTERFACE_SPI								SPI4
#else
#error Wrong M8266WIFI_SPI_INTERFACE_NO defined near line 104 for STM32 in brd_cfg.h
#endif



#if   defined(MCU_IS_STM32)&&(M8266WIFI_USART_INTERFACE_NO == 1) // for STM32
 
#define  M8266WIFI_INTERFACE_USART							USART1
#define  M8266WIFI_INTERFACE_USART_IRQn					USART1_IRQn
#elif (M8266WIFI_USART_INTERFACE_NO == 2)
#define  M8266WIFI_INTERFACE_USART							USART2
#define  M8266WIFI_INTERFACE_USART_IRQn					USART2_IRQn
#else
#error Wrong M8266WIFI_SPI_INTERFACE_NO defined near line 90 for STM32 in brd_cfg.h
#endif


//Other board related Macros

#if defined(MCU_IS_STM32F4XX)
#if 1  // for DISCOVERY STM32F4 Board
#define RCC_AHB1Periph_GPIO_LEDS								RCC_AHB1Periph_GPIOF
#define LED_GPIOS															  GPIOF
#define LED0_GPIO_PIN													  GPIO_Pin_9
#define LED1_GPIO_PIN													  GPIO_Pin_10
#else // for a customer's Board
#define RCC_AHB1Periph_GPIO_LEDS								RCC_AHB1Periph_GPIOC
#define LED_GPIOS															  GPIOC
#define LED0_GPIO_PIN													  GPIO_Pin_0
#define LED1_GPIO_PIN													  GPIO_Pin_1
//#define LED2_GPIO_PIN													  GPIO_Pin_2
//#define LED3_GPIO_PIN													  GPIO_Pin_3
#endif
#if 1 // for DISCOVERY STM32F4 Board
#define RCC_AHB1Periph_GPIO_KEYS								RCC_AHB1Periph_GPIOE
#define KEY_GPIOS															  GPIOE
#define KEY0_GPIO_PIN														GPIO_Pin_4
#define KEY1_GPIO_PIN 													GPIO_Pin_3
#define KEY2_GPIO_PIN														GPIO_Pin_2
#else // for a customer's Board
#define RCC_AHB1Periph_GPIO_KEYS								RCC_AHB1Periph_GPIOB
#define KEY_GPIOS																GPIOB
#define KEY0_GPIO_PIN														GPIO_Pin_9
#define KEY1_GPIO_PIN														GPIO_Pin_9
#define KEY2_GPIO_PIN														GPIO_Pin_9
#endif
#endif


