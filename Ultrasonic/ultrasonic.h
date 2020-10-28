#ifndef __ULTRASONIC
#define  __ULTRASONIC

#include "stm32f10x.h"

#define HCSR04_PORT     GPIOD
#define HCSR04_CLK      RCC_APB2Periph_GPIOD
#define HCSR04_TRIG     GPIO_Pin_6
#define HCSR04_ECHO     GPIO_Pin_7

#define TRIG_Send1  GPIO_SetBits(HCSR04_PORT,GPIO_Pin_6);
#define TRIG_Send0  GPIO_ResetBits(HCSR04_PORT,GPIO_Pin_6);


#define ECHO_Reci  GPIO_ReadInputDataBit(HCSR04_PORT,HCSR04_ECHO)
 
int Hcsr04GetLength(void );
void Hcsr04Init1(void);
void  Length(uint8_t * p);





#endif /**__ULTRASONIC**/


