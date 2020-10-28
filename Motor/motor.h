#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm32f10x.h"

void Motor_Init(void);
void Motor_Enable(void);
void Advance(void);
void Back(void);
void Left(void);
void Right(void);
void Stop(void);
void TIM3_PWM_Init(int speed);


#endif /*_MOTOR_H*/

