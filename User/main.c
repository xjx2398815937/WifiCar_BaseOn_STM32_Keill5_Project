#include "stm32f10x.h"
#include "led.h"
#include "usart.h"
#include "ultrasonic.h"
#include "motor.h"
#include "wifi.h"
#include "ov7725.h"
#include "sccb.h"

uint8_t retry = 0;
extern uint8_t Ov7725_vsync;
extern OV7725_MODE_PARAM cam_mode;

void Init()//初始化函数
{
	//串口的初始化
	USART_Config();
	
	printf("\r\n欢迎使用徐佳兴的毕业设计-WIFI智能小车\r\n");
	printf("\r\n进行系统初始化\r\n");
	
	printf("\r\n串口初始化完成\r\n");
	
	//LED的初始化
	LED_Init();
	printf("\r\nLED初始化完成\r\n");
	
	//超声波传感器的初始化
	Hcsr04Init1(); 
	printf("\r\n超声波初始化完成\r\n");
	
	//电机驱动的初始化
	Motor_Init();
	Motor_Enable();
	TIM3_PWM_Init(10);
	printf("\r\n电机驱动初始化完成\r\n");
	
	//WIFI的初始化
	printf("\r\n进行WIFI配置\r\n");
	WifiConfig();
	
	OV7725_GPIO_Config();
	while(OV7725_Init() != SUCCESS)
	{
		if(retry==0)
		{
			printf("\r\n没有检测到OV7725摄像头，请接通摄像头\r\n");
			retry = 1;
		}
	}
	/*根据摄像头参数组配置模式*/
	OV7725_Special_Effect(cam_mode.effect);
	/*光照模式*/
	OV7725_Light_Mode(cam_mode.light_mode);
	/*饱和度*/
	OV7725_Color_Saturation(cam_mode.saturation);
	/*光照度*/
	OV7725_Brightness(cam_mode.brightness);
	/*对比度*/
	OV7725_Contrast(cam_mode.contrast);
	/*特殊效果*/
	OV7725_Special_Effect(cam_mode.effect);
	/*设置图像采样及模式大小*/
	OV7725_Window_Set(	cam_mode.cam_sx,
											cam_mode.cam_sy,
											cam_mode.cam_width,
											cam_mode.cam_height,
											cam_mode.QVGA_VGA);
	printf("\r\nOV7725摄像头初始化完成\r\n");
	Ov7725_vsync = 0;
	
	
}
int main()
{
	Init();
	uint8_t data[1];//用来接收收到的数据，之后进行分析
	uint8_t length[4];
	
	while(1)
	{
		
			
			Length(length);
			M8266WIFI_SPI_Send_Data(length,4,0,NULL);

			if(M8266WIFI_SPI_Has_DataReceived())
			{
				M8266WIFI_SPI_RecvData(data,1,1,0,NULL);
				switch(data[0])
				{
					//自动驾驶
					case '0':
						while(!M8266WIFI_SPI_Has_DataReceived())
						{
							Length(length);
							M8266WIFI_SPI_Send_Data(length,4,0,NULL);
							
							if(Hcsr04GetLength() > 45)
							{
								Advance();
							}
							else
							{
								Stop();
								Left();
							}
						}
						break;
					//手动操作
					case '1':
						Stop();
						break;
					//前进
					case '2':
						Advance();
						break;
					//后退
					case '3':
						Back();
						break;
					//左转
					case '4':
						Left();
						break;
					//右转
					case '5':
						Right();
						break;
					//速度快
					case '6':
						TIM3_PWM_Init(10);
						Stop();
						break;
					//速度慢
					case '7':
						TIM3_PWM_Init(8);
						Stop();					
						break;
					//停止
					case '8':
						Stop();
						break;
					case '9':
						while(1)
						{
								
								/*接收到新图像进行发送Phone*/
								if(Ov7725_vsync == 2 )
								{
									FIFO_PREPARE;  			/*FIFO准备*/					
									ImagDataSentToPhone(	cam_mode.cam_width,
																				cam_mode.cam_height);	/*采集并发送至手机*/
									Ov7725_vsync = 0;
								}
								M8266WIFI_SPI_RecvData(data,1,1,0,NULL);
								if(data[0]=='5')
								{
									break;
								}
						}
						break;
						
					default:
						break;
				}

			}


		}
		
	
}

