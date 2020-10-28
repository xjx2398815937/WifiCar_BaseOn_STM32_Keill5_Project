#include "motor.h"
/************************************************
连线：
黄线 GND
绿线 5V
棕线 ENA	-->PB0(PWM波输出控制电机的转速)
橙线 ENB	-->PB1(PWM波输出控制电机的转速)
(这四个引脚控制电机的转动方向)
黄线 IN1	-->PB12
绿线 IN2	-->PB13 
（左侧轱辘）
红线 IN3	-->PB14
紫线 IN4	-->PB15
（右侧轱辘）
作者：徐佳兴
*************************************************/

/*************************************************
函数名称：void Motor_Init(void)
函数功能：小车初始化
入口参数：无
出口参数：无
*************************************************/
void Motor_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //

	GPIO_Init(GPIOB,&GPIO_InitStructure); 
}
/*************************************************
函数名称：void Motor_Enable(void)
函数功能：小车运动使能
入口参数：无
出口参数：无
*************************************************/
void Motor_Enable(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
}
/*************************************************
函数名称： void Advance(void)
函数功能：小车前进
入口参数：无
出口参数：无
*************************************************/
void Advance(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);	
}

/*************************************************
函数名称：void Back(void)
函数功能：小车后退
入口参数：无
出口参数：无
*************************************************/
void Back(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);		
}

/*************************************************
函数名称：void Left(void)
函数功能：小车左转
入口参数：无
出口参数：无
*************************************************/
void Left(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12);	//左轮
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);  //右轮
	GPIO_SetBits(GPIOB,GPIO_Pin_15);					
}

/*************************************************
函数名称：void Right(void)
函数功能：小车右转
入口参数：无
出口参数：无
*************************************************/
void Right(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);							
}

/*************************************************
函数名称：void Stop(void)
函数功能：小车停止
入口参数：无
出口参数：无
*************************************************/
void Stop(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);					
}
/*************************************************
函数名称：void TIM3_PWM_Init(void)
函数功能：通过更改TIM_TimeBaseStructure.TIM_Period的值改变PWM周期，
					通过更改TIM_OCInitStructure.TIM_Pulse的值改变PWM占空比，实现调速
					使用的是通用定时器TIM3的通道3和4
入口参数：int speed： 1-10 	最大占空比为100%
出口参数：无
*************************************************/
void TIM3_PWM_Init(int speed)
{
	
	//C语言的标准 C89：必须在大括号后面定义变量	C99：变量在任意位置都可以定义等；在魔法棒中进行设置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/************************时基初始化结构体************************************/
	//PWM的周期 f=72M/(7199+1)*(9+1) = 10K 一般的直流电机驱动
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Period = 9; 
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; 

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	/*************************GPIO引脚的初始化*************************************/	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Output Compare Active Mode configuration: Channel3 */
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = speed; //占空比为100%
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx,PB0
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIMx在CCR2上的预装载寄存器
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = speed;//占空比为100%
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);			//PB1
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIMx在CCR2上的预装载寄存器
	
	//上面两句中的OCx确定了是channle几，要是OC3则是channel 3  
	
	TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIMx在ARR上的预装载寄存器 
	
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
		
}

