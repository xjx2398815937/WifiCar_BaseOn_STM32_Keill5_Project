/************************
*描述:外接的ESP8266WIFI芯片的初始化
*			{}
*链接引脚：
*ESP8266			STM32
*GND			->	GND
*3V3			->	3V3
*SCLK 		-> 	PA5
*MISO			->	PA6
*MOSI			->	PA7
*nCS			->	PA4
*nREST		->	PC4
*作者：徐佳兴
*************************/
#include "wifi.h"
#include "usart.h"

//WIFI的相关配置
u8 ssid[32] = "208";//WIFI的名字
u8 password[64] = "qiaoqiupup";//密码
u16 remote_port = 8899; //目的端口号为8899
//char* remote_addr = "192.168.43.1"; //目的IP，手机端与路由器相连，路由器分配的IP地址已经与手机的MAC
																			//地址进行绑定，这是我用路由器的原因，如果用小车直接连接手机热点的话
//																			//每次分配的IP地址不确定，导致小车连接不上手机
//char* remote_addr = "192.168.1.103"; //目的IP，手机端与路由器相连，路由器分配的IP地址已经与手机的MAC
//																			//地址进行绑定，这是我用路由器的原因，如果用小车直接连接手机热点的话
////																			//每次分配的IP地址不确定，导致小车连接不上手机
char* remote_addr = "192.168.0.100"; //目的IP，手机端与路由器相连，路由器分配的IP地址已经与手机的MAC
																		//地址进行绑定，这是我用路由器的原因，如果用小车直接连接手机热点的话
																		//每次分配的IP地址不确定，导致小车连接不上手机
void WifiConfig(void)
{
	u8 success=0;
       
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
  LED_Init();
	M8266HostIf_Init();
#ifdef M8266WIFI_CONFIG_VIA_SPI
	success = M8266WIFI_Module_Init_Via_SPI();
#else
#error You should define M8266WIFI_CONFIG_VIA_SPI in brd_cfg.h
#endif
	 if(success)
	{
		for(int key=0; key<3; key++)  // Flash the Two LEDs in the Main Board
		{
			 LED_set(0, 0); LED_set(1, 0);  delay_ms(100);
			 LED_set(0, 1); LED_set(1, 0);  delay_ms(100);
			 LED_set(0, 0); LED_set(1, 1);  delay_ms(100);
			 LED_set(0, 1); LED_set(1, 1);  delay_ms(100);
		}
		
		if(1 == M8266WIFI_SPI_Set_Opmode(1,1,NULL))//设置wifi的工作在sta的模式下
		{
				printf("\r\n已配置WIFI工作在STA模式下\r\n");
		}
		
				printf("\r\n正在链接目的路由器，SSID：jiaxing，password：jiahewanshixing\r\n");
		while(!(M8266WIFI_SPI_STA_Connect_Ap(ssid,password,1,6,NULL)))//链接到的SSID，密码，是否保存链接，超时时间，返回值；
		{
				printf("\r\n请接通目的路由器电源\r\n");					
		}
				printf("\r\n已链接到目的路由器\r\n");
		
				printf("\r\n正在链接目的IP：192.168.43.1，目的端口号：8899\r\n");
		while((!(M8266WIFI_SPI_Setup_Connection(1,0,remote_addr,remote_port,0,5,NULL)))||(!(M8266WIFI_SPI_Setup_Connection(1,0,remote_addr,8898,1,5,NULL))))//TCP客户端，随机一个地址，目的IP，目的端口号，链接号，超时时间，返回值
		{ 
				printf("\r\n请打开手机端配置\r\n");
			
		}
				printf("\r\n已连接到目的服务器\r\n");
	}

}


