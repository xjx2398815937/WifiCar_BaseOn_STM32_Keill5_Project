#include "ultrasonic.h"
/************************
*描述：超声波传感器的初始化
*链接引脚：
* 传感器			STM32
* VCC		-->	3.3V
* GND		-->	GND
*	TRIG	-->	PD6
*	ECHO	-->	PD7
*作者：徐佳兴
*************************/
u16 msHcCount = 0;//ms计数
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Delay_Ms_Ms
** 功能描述: 延时1MS (可通过仿真来判断他的准确度)            
** 参数描述：time (ms) 注意time<65535
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Delay_Ms(uint16_t time)  //延时函数
{ 
    uint16_t i,j;
    for(i=0;i<time;i++)
          for(j=0;j<10260;j++);
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Delay_Ms_Us
** 功能描述: 延时1us (可通过仿真来判断他的准确度)
** 参数描述：time (us) 注意time<65535                 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Delay_Us(uint16_t time)  //延时函数
{ 
    uint16_t i,j;
    for(i=0;i<time;i++)
          for(j=0;j<9;j++);
}

 //NVIC配置
void hcsr04_NVIC()
{
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

		NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;             //选择串口1中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //抢占式中断优先级设置为1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //响应式中断优先级设置为1
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //使能中断
		NVIC_Init(&NVIC_InitStructure);
}

void Hcsr04Init()
{  
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;     //生成用于定时器设置的结构体
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
		 
			//IO初始化
		GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG;       //发送电平引脚
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
		GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
		GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
		 
		GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO;     //返回电平引脚
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO);    
     
		//定时器初始化 使用基本定时器TIM6
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);   //使能对应RCC时钟
		//配置定时器基础结构体
		TIM_DeInit(TIM6);
		TIM_TimeBaseStructure.TIM_Period = (1000-1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值         计数到1000为1ms
		TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //设置用来作为TIMx时钟频率除数的预分频值  1M的计数频率 1US计数
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//不分频
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位         
		
		TIM_ClearFlag(TIM6, TIM_FLAG_Update);   //清除更新中断，免得一打开中断立即产生中断
		TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);    //打开定时器更新中断
		hcsr04_NVIC();
		TIM_Cmd(TIM6,DISABLE);     
}


//tips：static函数的作用域仅限于定义它的源文件内，所以不需要在头文件里声明
static void OpenTimerForHc()        //打开定时器
{
        TIM_SetCounter(TIM6,0);//清除计数
        msHcCount = 0;
        TIM_Cmd(TIM6, ENABLE);  //使能TIMx外设
}
 
static void CloseTimerForHc()        //关闭定时器
{
        TIM_Cmd(TIM6, DISABLE);  //使能TIMx外设
}

u32 GetEchoTimer(void)
{
        u32 t = 0;
        t = msHcCount*1000;//得到MS
        t += TIM_GetCounter(TIM6);//得到US
        TIM6->CNT = 0;  //将TIM2计数寄存器的计数值清零
        Delay_Ms(50);
        return t;
}

//一次获取超声波测距数据 两次测距之间需要相隔一段时间，隔断回响信号
//为了消除余震的影响，取五次数据的平均值进行加权滤波。
int Hcsr04GetLength(void )
{
		u32 t = 0;  
		int i = 0;
		int lengthTemp = 0;
		int sum = 0;
		while(i!=5)
		{
				TRIG_Send1;      //发送口高电平输出
				Delay_Us(20);
				TRIG_Send0;
				while(ECHO_Reci == 0);      //等待接收口高电平输出
				OpenTimerForHc();        //打开定时器
				i = i + 1;
				while(ECHO_Reci == 1);
				CloseTimerForHc();        //关闭定时器
				t = GetEchoTimer();        //获取时间,分辨率为1US
				lengthTemp = ((int)t/58.0);//cm
				sum = lengthTemp + sum ;
		}
		lengthTemp = sum/5.0;
		return lengthTemp;
}
void  Length(uint8_t * p)
{
		
	
		int length1 = Hcsr04GetLength();
			
			p[0] = (char)('0' + length1/1000);
			if(p[0]=='0')
			{
				p[0] = '0';
			}
			p[1] = (char)('0' + length1/100%10);
			if(p[0]=='0'&&p[1]=='0')
			{
				p[1] = '0';
			}
			p[2] = (char)('0' + length1/10%10);
			if(p[0]=='0'&&p[1]=='0'&&p[2] == '0')
			{
				p[2] = '0';
			}
			
			p[3] = (char)('0' + length1%10);
			
			
}

void Hcsr04Init1(void)
{
	hcsr04_NVIC();
	Hcsr04Init();
}

 
 
