/********************************************************************
 * M8266WIFI_ops.c
 * .Description
 *     source file of M8266WIFI HAL operations
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
#include "stdio.h"
#include "string.h"
#include "brd_cfg.h"
#include "led.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"

void M8266WIFI_Module_delay_ms(u16 nms)
{
	 u16 i, j;
	 for(i=0; i<nms; i++)
	   for(j=0; j<4; j++)									// delay 1ms. Call 4 times of delay_us(250), since M8266HostIf_delay_us(u8 nus), nus max 256
	      M8266HostIf_delay_us(250);
}
////////////////////////////////////////////////////////////////////////////////////
// Functions as examples of M8266WIFI Module Operations
//    -- Fundamentals
////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************************
 * M8266WIFI_Module_Hardware_Reset                                                 *
 * Description                                                                     *
 *    1. To perform a hardware reset to M8266WIFI module via the nReset Pin        *
 *       and bring M8266WIFI module to boot up from external SPI flash             *
 *    2. In order to make sure the M8266WIFI module bootup from external           *
 *       SPI flash, nCS should be low during Reset out via nRESET pin              *
 * Parameter(s):                                                                   *
 *    none                                                                         *
 * Return:                                                                         *
 *    none                                                                         *
 ***********************************************************************************/
void M8266WIFI_Module_Hardware_Reset(void) // total 800ms
{
	 M8266HostIf_Set_SPI_nCS_Pin(0);   	// Module nCS==ESP8266 GPIO15 as well, should be low during reset in order for a normal reset 
	 M8266WIFI_Module_delay_ms(1); 	    // delay 1ms, adequate for nCS stable
	
	 M8266HostIf_Set_nRESET_Pin(0);			// Pull low the nReset Pin to bring the module into reset state
	 M8266WIFI_Module_delay_ms(1); 	    // delay 1ms, adequate for nRESET stable
	
	 M8266HostIf_Set_nRESET_Pin(1);			// Pull high again the nReset Pin to bring the module exiting reset state
	 M8266WIFI_Module_delay_ms(200); 	  // at least 18ms required for reset-out-boot sampling boottrap pin
                                      // Here, we use 200ms aadequate for stablility
 	 M8266HostIf_Set_SPI_nCS_Pin(1);
 //M8266WIFI_Module_delay_ms(1); 	    // delay 1ms, adequate for nCS stable
	
   M8266WIFI_Module_delay_ms(800-1-200-1); 	// Delay more than 500ms for M8266WIFI module bootup and initialization including bootup information print. 
	                                    // Wait time should be no less than 0.5s. 
                                      // Here, we wait 800ms
}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/////// BELOW FUNCTIONS ARE QUERYING OR CONFIGURATION VIA UART                    //
/////// IGNORE THEM IF YOU DON'T USER USART                                       //
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
#if 0
/***********************************************************************************
 * M8266WIFI_USART_wait_response_ok                                                      *
 * Description                                                                     *
 *    To wait response from the UART of M8266WIFI module and check                 *
 *    whether the response is what is expected                                     *
 * Parameter(s):                                                                   *
 *    1. ack: the string that should be contained in the expected response         *
 *    2. wait_time_in_10ms: max timeout to wait the response from UART             *
 * Return:                                                                         *
 *       0 = do not receive the expected response before timeout                   *
 *       1 = do     receive the expected response before timeout                   *
 ***********************************************************************************/
u8 M8266WIFI_USART_wait_response_ok(u8* ack, u16 wait_time_in_10ms)
{
	u16 count=0;
	u8  response[512]={0};
	while(wait_time_in_10ms--)	// wait max timeout
	{
		count = M8266HostIf_USART_RX_BUFFER_Data_count();
		if(count==0)
		{
			M8266WIFI_Module_delay_ms(10);
		}
		else
		{
			M8266WIFI_Module_delay_ms(10);
			if(count!=M8266HostIf_USART_RX_BUFFER_Data_count())
			continue;
 
			M8266HostIf_USART_get_data(response, count);
			
			if(strstr((const char*)response, (const char*)ack)==0)
				return 0;
			else 
				return 1; // response°üº¬ack,³É¹¦£¡
		}
	}
	return 0; 	
}
////////////////////////////////////////////////////////////////////////////////////
// Functions as examples of M8266WIFI Module Operations
//    -- Middle Level
////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
 * M8266WIFI_USART_reboot_module_and_check_if_reboot_successfully                        *
 * Description                                                                     *
 *    To hardware reset Module via nRESET pin, wait the bootup information         *
 *    print from the UART of M8266WIFI module, and check whether it contains       *
 *    the "ready\r\n" which indicating the bootup succeeds                         *
 * Parameter(s):                                                                   *
 *    1. wait_time_in_10ms: max timeout to wait the "ready\r\n" from UART          *
 * Return:                                                                         *
 *       0 = do not receive the expected "ready\r\n" before timeout                *
 *       1 = do     receive the expected "ready\r\n" before timeout                *
 ***********************************************************************************/
u8 M8266WIFI_USART_reboot_module_and_check_if_reboot_successfully(u16 wait_time_in_10ms)
{
	 M8266WIFI_Module_Hardware_Reset();
	 return M8266WIFI_USART_wait_response_ok("ready\r\n", wait_time_in_10ms);
}
/***********************************************************************************
 * M8266WIFI_USART_get_sta_ap_mode                                                       *
 * Description                                                                     *
 *    To get the current op mode of M8266WIFI via UART AT commands                 *
 * Parameter(s):                                                                   *
 *    None                                                                         *
 * Return:                                                                         *
 *       1 = M8266WIFI in STA    mode currently                                    *
 *       2 = M8266WIFI in AP     mode currently                                    *
 *       3 = M8266WIFI in STA+AP mode currently                                    *
 *       0 = error                                                                 *
 ***********************************************************************************/
u8 M8266WIFI_USART_get_sta_ap_mode(void)
{
	char at_cmd[32];
	
	sprintf(at_cmd, "AT+CWMODE_CUR?\r\n");
	M8266HostIf_usart_txd_string(at_cmd);		 
	if(M8266WIFI_USART_wait_response_ok("CWMODE_CUR:1", 1000)==1)  // STA mode
		return 1;

	M8266HostIf_usart_txd_string(at_cmd);		 
	if(M8266WIFI_USART_wait_response_ok("CWMODE_CUR:2", 1000)==1)  // AP mode
		return 2;

	M8266HostIf_usart_txd_string(at_cmd);		 
	if(M8266WIFI_USART_wait_response_ok("CWMODE_CUR:3", 1000)==1)  // AP+STA mode
		return 3;
	
	return 0;  // error
}
/***********************************************************************************
 * M8266WIFI_USART_wait_sta_connecting_to_ap_and_get_ip                                  *
 * Description                                                                     *
 *    Wait M8266WIFI in STA mode connecting to AP/Router and get ip address        *
 *    via UART AT commands                                                         *
 * Parameter(s):                                                                   *
 *    1. max_wait_time_in_s: max timeout to wait in seconds                        *
 * Return:                                                                         *
 *       1 = M8266WIFI module connects AP/router and gets a valid IP               *
 *           from AP/router before the timeout succefully                          *
 *       0 = M8266WIFI module fails to connect the AP/router and get               *
 *           a valid IP before the timeout                                         *
 ***********************************************************************************/
u8 M8266WIFI_USART_wait_sta_connecting_to_ap_and_get_ip(u8 max_wait_time_in_s)
{
	 u16  i;
   char at_cmd[128];
	 
	 for(i=0; i<1*max_wait_time_in_s; i++)  // max wait
	 {
		   sprintf(at_cmd, "AT+CIFSR\r\n");
	     M8266HostIf_usart_txd_string(at_cmd);
		 
		   if(M8266WIFI_USART_wait_response_ok("STAIP,\"0.0.0.0\"", 1000)==1) // still wait ip
			 {
				  M8266WIFI_Module_delay_ms(1000);
				  continue;
       }
       else
       {
				  break;
			 }
	 }
	 if(i>=1*max_wait_time_in_s)
		 return 0; // false
	 
	 return 1;  // true
}
/*************************************************************************************
 * M8266WIFI_USART_setup_spi_ch_and_connection                                             *
 * Description                                                                       *
 *    To set up SPI and UDP/TCP connection via UART AT commands                      *
 * Parameter(s):                                                                     *
 *    1. tcp_udp     : the type of connection to set up                              *
 *                     = 0, to set up connection of UDP                              *
 *                     = 1, to set up connection of TCP                              *
 *    2. local_port  : to specify the local port if any                              *
 *                     if local_port=0, then a random local port will be generated   *
 *    3. remote_ip   : the string of ip address of remote server                     *
 *                     e.g. "192.168.1.2"                                            *
 *    4. remote_port : the port value of remote server                               *
 *    5. link_no     : the link_number to be used. max 8 links as per mutitiple link *
 * Return:                                                                           *
 *       0 = failed                                                                  *
 *       1 = success                                                                 * 
 *************************************************************************************/
u8 M8266WIFI_USART_setup_spi_ch_and_connection(u8 tcp_udp, u16 local_port, u8* remote_ip, u16 remote_port, u8 link_no)
{
   char at_cmd[128];
   sprintf(at_cmd, "AT+SPI=1,\"HSPI\",\"SLAVE\",\"MSB_FIRST\",\"MODE0\",\"10MHZ\",\"%s\",%d,\"%s\",%d,%d\r\n", 
	                 (tcp_udp==1)?"TCP":"UDP", local_port, remote_ip, remote_port, link_no);
	 M8266HostIf_usart_txd_string(at_cmd); 
	
	 return M8266WIFI_USART_wait_response_ok("OK\r\n", 1000); // wait 1000*10ms=10s
}
/*************************************************************************************
 * M8266WIFI_USART_set_led                                                                 *
 * Description                                                                       *
 *    To turn on or off the two LEDs on the M8266WIFI module via UART AT commands    *
 * Parameter(s):                                                                     *
 *    1. led_no      : the number of on-module LEDs                                  *
 *                     = 1,      to turn on/off LED1                                 *
 *                     = others, to turn on/off LED2                                 *
 *    2. on          : turn on or off the LED                                        *
 *                     = 0,      to turn off the specified LED                       *
 *                     = 1,      to turn on  the specified LED                       *
 * Return:                                                                           *
 *       0 = failed                                                                  *
 *       1 = success                                                                 * 
 *************************************************************************************/
u8 M8266WIFI_USART_set_led(u8 led_no, u8 on)
{
   char at_cmd[32];
	 sprintf(at_cmd, "AT+GPIO=0,%d,1,0,%d\r\n",(led_no==1)?0:5,1-on);
	 M8266HostIf_usart_txd_string(at_cmd); 
	
	 return M8266WIFI_USART_wait_response_ok("OK\r\n", 1000); // timeout wait 1000*10ms=10s
}
/*************************************************************************************
 * M8266WIFI_running_ok                                                              *
 * Description                                                                       *
 *    To check whether the module running OK via UART AT command                     *
 * Parameter(s):                                                                     *
 *    None
 * Return:                                                                           *
 *       0 = failed                                                                  *
 *       1 = success                                                                 * 
 *************************************************************************************/
u8 M8266WIFI_running_ok(void)
{
	M8266HostIf_usart_txd_string("AT\r\n");
	return M8266WIFI_USART_wait_response_ok("OK\r\n", 100); // timeout wait 100*10ms=1s
}
/***********************************************************************************
 * M8266WIFI_USART_SmartConfig                                                           *
 * Description                                                                     *
 *    To perform a Smart Config to M8266WIFI module via UART AT command            *
 * Parameter(s):                                                                   *
 *    none                                                                         *
 * Return:                                                                         *
 *    1= SmartConfig succeeds and connected to AP                                  *
 *    0= SmartConfig failed                                                        *
 ***********************************************************************************/
u8 M8266WIFI_USART_SmartConfig(void)
{
	u32 i;
	
	//Step 1: bring module into the smartconfig mode via AT command
	M8266HostIf_usart_txd_string("AT+SMARTCONFIG=1,1\r\n");

	//Step 2: wait the response from the M8266WIFI module to check whether the SmartConfig succeeds
	//        If succeed, the M8266WIFI module will output "SC_STATUS_LINK_OVER" with ip address via UART
	//        The timeout of this process is around 60s
#define SMART_CONFIG_TIME_OUT_IN_TENTH_SECONDS	  ((60+2)*10)		 // timeout wait 62s
	for(i=0; i<SMART_CONFIG_TIME_OUT_IN_TENTH_SECONDS; i++) 
  {
	//if(M8266WIFI_USART_wait_response_ok("WIFI CONNECTED\r\nWIFI GOT IP\r\n", 1)==1)
  //if(M8266WIFI_USART_wait_response_ok("WIFI CONNECTED\r\n", 1)==1) 								
	//if(M8266WIFI_USART_wait_response_ok("WIFI GOT IP\r\n", 1)==1)    								
		if(M8266WIFI_USART_wait_response_ok("SC_STATUS_LINK_OVER\r\n", 1)==1)
		   break;  // When SmartConfig succeeds, the module will exit the SmartConfig mode automatically
		           // Therefore, no need to bring the module out of SmartConfig mode
		M8266WIFI_Module_delay_ms(100); //wait 0.1s
	}
	
	if(i>=SMART_CONFIG_TIME_OUT_IN_TENTH_SECONDS)   // if timeout, then failed
	{
		 M8266HostIf_usart_txd_string("AT+SMARTCONFIG=0\r\n");  // bring module to exit the smartconfig mode
		 return 0; // failed
	}
#undef SMART_CONFIG_TIME_OUT_IN_TENTH_SECONDS

  return 1;    // success
}
/***********************************************************************************
 * M8266WIFI_USART_WpsConfig                                                             *
 * Description                                                                     *
 *    To perform a WPS Config to M8266WIFI module via UART AT command              *
 * Parameter(s):                                                                   *
 *    none                                                                         *
 * Return:                                                                         *
 *    1= WpsConfig succeeds and connected to AP                                    *
 *    0= WpsConfig failed                                                          *
 ***********************************************************************************/
u8 M8266WIFI_USART_WpsConfig(void)
{
	u32 i;
	
	//Step 1: bring module into the wpsconfig mode via AT command
	M8266HostIf_usart_txd_string("AT+WPSCONFIG=1,1\r\n");

	//Step 2: wait the response from the M8266WIFI module to check whether the WpsConfig succeeds
	//        If succeed, the M8266WIFI module will output "WPS_CB_ST_SUCCESS" with ip address via UART
	//        The timeout of this process is around 60s
#define WPS_CONFIG_TIME_OUT_IN_TENTH_SECONDS	  ((60+2)*10)		 // timeout wait 62s
	for(i=0; i<WPS_CONFIG_TIME_OUT_IN_TENTH_SECONDS; i++) 
  {
	//if(M8266WIFI_USART_wait_response_ok("WIFI CONNECTED\r\nWIFI GOT IP\r\n", 1)==1)
  //if(M8266WIFI_USART_wait_response_ok("WIFI CONNECTED\r\n", 1)==1) 								
	//if(M8266WIFI_USART_wait_response_ok("WIFI GOT IP\r\n", 1)==1)    								
		if(M8266WIFI_USART_wait_response_ok("WPS_CB_ST_SUCCESS\r\n", 1)==1)
		   break;  // When SmartConfig succeeds, the module will exit the WpsConfig mode automatically
		           // Therefore, no need to bring the module out of SmartConfig mode
		M8266WIFI_Module_delay_ms(100); //wait 0.1s
	}
	
	if(i>=WPS_CONFIG_TIME_OUT_IN_TENTH_SECONDS)   // if timeout, then failed
	{
		 M8266HostIf_usart_txd_string("AT+WPSCONFIG=0\r\n");  // bring module to exit the wpsconfig mode
		 return 0; // failed
	}
#undef WPS_CONFIG_TIME_OUT_IN_TENTH_SECONDS

  return 1;    // success
}
////////////////////////////////////////////////////////////////////////////////////
// Functions as examples of M8266WIFI Module Operations
//    -- APIs
////////////////////////////////////////////////////////////////////////////////////
/*************************************************************************************
 * M8266WIFI_Config_Connection_via_USART                                             *
 * Description                                                                       *
 *    To setup udp/tcp connection via USART                                          *
 * Parameter(s):                                                                     *
  *    1. tcp_udp     : the type of connection to set up                             *
 *                     = 0, to set up connection of UDP                              *
 *                     = 1, to set up connection of TCP                              *
 *    2. local_port  : to specify the local port if any                              *
 *                     if local_port=0, then a random local port will be generated   *
 *    3. remote_ip   : the string of ip address of remote server                     *
 *                     e.g. "192.168.1.2"                                            *
 *    4. remote_port : the port value of remote server                               *
 *    5. link_no     : the link_number to be used. max 8 links as per mutitiple link *
 * Return:                                                                           *
 *       0 = failed                                                                  *
 *       1 = success                                                                 * 
 *************************************************************************************/
u8 M8266WIFI_Config_Connection_via_USART(u8 tcp_udp, u16 local_port, u8* remote_ip, u16 remote_port, u8 link_no)
{
	//Setup the TCP/UPD link
	//u8 M8266WIFI_USART_setup_spi_ch_and_connection(u8 tcp_udp, u16 local_port, u8* remote_ip, u16 remote_port, u8 link_no)
	if( M8266WIFI_USART_setup_spi_ch_and_connection(tcp_udp, local_port, remote_ip, remote_port, link_no) !=1)
		 return 0;
	
	//Clear up the USART RXD Buffer
	M8266HostIf_USART_Reset_RX_BUFFER();
	
	return 1;
}
/***********************************************************************************
 * M8266WIFI_Module_Init_Via_USART(void)                                           *
 * Description                                                                     *
 *    To perform a Initialization sequency to M8266WIFI module via USART           *
 *    (1) Reboot the M8266WIFI module via nRESET pin and check if reboot successful*
 *    (2) Wait and Check if the M8266WIFI module gets an valid IP address          *
 *        if the module in STA or STA+AP mode                                      *
 *    Call this function after Host USART and SPI Interface Initialised            *
 *    if use USART to config module                                                *
 * Parameter(s):                                                                   *
 *    none                                                                         *
 * Return:                                                                         *
 * Return:                                                                         *
 *       0 = failed                                                                *
 *       1 = success                                                               * 
 ***********************************************************************************/
 u8 M8266WIFI_Module_Init_Via_USART(void)
 {
	  u8 sta_ap_mode;
	 
	  // Hardware reboot module and check whether successfully. 1=success, otherwise=fail
	  if(M8266WIFI_USART_reboot_module_and_check_if_reboot_successfully(100)!=1) 
	  {
		  return 0;
	  }

	  // if STA mode or STA+AP mode, then, wait connection to external AP and getting ip
	  sta_ap_mode = M8266WIFI_USART_get_sta_ap_mode();				
	  if(  (sta_ap_mode == 1)   // if STA mode
	     ||(sta_ap_mode == 3))  // if STA+AP mode
	  {
		   if(M8266WIFI_USART_wait_sta_connecting_to_ap_and_get_ip(10)==0) // max wait 10s to get sta ip
		   {
				  return 0; // printf("M8266WIFI module failed to get sta ip address!\n");
		   }
	  }
		
		M8266HostIf_SPI_Select((uint32_t)M8266WIFI_INTERFACE_SPI, 10500000);
		
		return 1;
}
#endif

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/////// BELOW FUNCTIONS ARE QUERYING OR CONFIGURATION VIA SPI
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
 * M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip                              *
 * Description                                                                     *
 *    Wait M8266WIFI in STA mode connecting to AP/Router and get ip address        *
 *    via SPI API functions in M8266WIFIDrv.lib                                    *
 * Parameter(s):                                                                   *
 *    1. sta_ip: if success, sta_ip return the ip address achieved                 *
 *    2. max_wait_time_in_s: max timeout to wait in seconds                        *
 * Return:                                                                         *
 *       1 = M8266WIFI module connects AP/router and gets a valid IP               *
 *           from AP/router before the timeout succefully                          *
 *       0 = M8266WIFI module fails to connect the AP/router and get               *
 *           a valid IP before the timeout                                         *
 ***********************************************************************************/
u8 M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(char* sta_ip, u8 max_wait_time_in_s)
{
	 u16  i;
	 u16  status=0;
	 
	 for(i=0; i<1*max_wait_time_in_s; i++)  // max wait
	 {
		   if(   (M8266WIFI_SPI_Get_STA_IP_Addr(sta_ip , &status) ==1)
				   && (strcmp(sta_ip, "0.0.0.0")!=0) )
           break;
     			 
			M8266WIFI_Module_delay_ms(1000);
			continue;
   }
   if(i>=1*max_wait_time_in_s)
		 return 0; // false
	 
	 return 1;  // true
}
/***********************************************************************************
 * M8266WIFI_Module_Init_Via_SPI(void)                                             *
 * Description                                                                     *
 *    To perform a Initialization sequency to M8266WIFI module via SPI I/F         *
 *    (1) Reboot the M8266WIFI module via nRESET pin                               *
 *    (2) Wait and Check if the M8266WIFI module gets an valid IP address          *
 *        if the module in STA or STA+AP mode                                      *
 *    Call this function after Host SPI Interface Initialised                      *
 *    if use SPI interface to config module                                        *
 * Parameter(s):                                                                   *
 *    none                                                                         *
 * Return:                                                                         *
 * Return:                                                                         *
 *       0 = failed                                                                *
 *       1 = success                                                               * 
 ***********************************************************************************/
 u8 M8266WIFI_Module_Init_Via_SPI(void)
 {
	u32  spi_clk = 40000000;
	u8   sta_ap_mode = 0;
	u8   connection_status = 0xFF;
	char sta_ip[15+1]={0};
	char ssid[32];
	s8   rssi;
	u16  status      = 0;
	
  //To hardware reset the module (with nCS=0 during reset) and wait up the module bootup
	M8266WIFI_Module_Hardware_Reset();
	
   // Try SPI clock in a fast one as possible up to 40MHz (M8266WIFI could support only upto 40MHz SPI) 
	 /*
	    Notes: 
	     For STM32, SPI Clock = APB2_CLOCK/SPI_BaudRatePrescaler, or something similar
	         (1) For STM32F1, , please see system_stm32f10x.c, APB2_CLOCK = PCLK2 = HCLK = SYSCLK
	                            SYSCLK  = 72MHz, so, APB2_CLOCK = 72MHz
	         (2) For STM32F4, please see system_stm32f4xx.c, APB2_CLOCK = PCLK2 = HCLK/2 = SYSCLK/2
	             Meanwhile, the PCLK2 should <= 90MHz, and SPI_CLK should <= 45MHz
	             - STM32F401xx:     SYSCLK          = 168MHz, so, APB2_CLOCK = 84MHz
	             - STM32F40_41xxx:  SYSCLK          = 180MHz, so, APB2_CLOCK = 90MHz
	             - STM32F427_437xx, STM32F429_439xx = 180MHz, so, APB2_CLOCK = 90MHz
	         (3) For STM32F7, please see sys.c, APB2_CLOCK = PCLK2 = HCLK/2 = SYSCLK/2
	             Meanwhile, the PCLK2 should <= 108MHz, and SPI_CLK should <=54MHz
	             - STM32F767xx:     SYSCLK          = 216MHz, so, APB2_CLOCK = 108MHz
	     For MCU_IS_NXP_LPC17XX
	         (1) SPI Clock =PCLK_SPI/SPI_BaudRatePrescaler
	         (2) PCLK_SPI  =  CPU_CLK = 96MHz, please see system_LPC17xx.c
	     For MCU_IS_HT32F16XX
	         (1) SPI Clock   =  APB_CLK_SPI / SPI_BaudRatePrescaler
	         (2) APB_CLK_SPI =  SYS_CLK /2 = 144MHz /2 = 72MHz, please see system_ht32f165x.c
*/
#if defined(MCU_IS_STM32)
#ifndef SPI_BaudRatePrescaler_2
#define SPI_BaudRatePrescaler_2 				((uint32_t)0x00000000U)
#define SPI_BaudRatePrescaler_4         ((uint32_t)0x00000008U)
#define SPI_BaudRatePrescaler_8         ((uint32_t)0x00000010U)
#define SPI_BaudRatePrescaler_16        ((uint32_t)0x00000018U)
#define SPI_BaudRatePrescaler_32        ((uint32_t)0x00000020U)
#define SPI_BaudRatePrescaler_64        ((uint32_t)0x00000028U)
#define SPI_BaudRatePrescaler_128       ((uint32_t)0x00000030U)
#define SPI_BaudRatePrescaler_256       ((uint32_t)0x00000038U)
#endif
#elif defined(MCU_IS_NXP_LPC17XX)
#define SPI_BaudRatePrescaler_2         ((uint32_t)0x00000002U)
#define SPI_BaudRatePrescaler_4         ((uint32_t)0x00000004U)
#define SPI_BaudRatePrescaler_6         ((uint32_t)0x00000006U)
#define SPI_BaudRatePrescaler_8         ((uint32_t)0x00000008U)
#define SPI_BaudRatePrescaler_16        ((uint32_t)0x00000010U)
#define SPI_BaudRatePrescaler_32        ((uint32_t)0x00000020U)
#define SPI_BaudRatePrescaler_64        ((uint32_t)0x00000040U)
#define SPI_BaudRatePrescaler_128       ((uint32_t)0x00000080U)
#define SPI_BaudRatePrescaler_256       ((uint32_t)0x00000100U)
#else
#endif

#if    defined(MCU_IS_STM32F1XX)
   M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_8);					// Setup SPI Clock. Here 72/8 = 9.0MHz for STM32F1xx, up to 18MHz (since STM32F1xx support SPI only 18MHz)
	 spi_clk =   9000000;
#elif  defined(MCU_IS_STM32F4XX)
   M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_8);					// Setup SPI Clock. Here 90/8 = 11.25MHz for STM32F4xx, up to 40MHz
	 spi_clk = 11250000;
#elif  defined(MCU_IS_STM32F7XX)
   M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);					// Setup SPI Clock. Here 108/4 = 27MHz for STM32F7xx, up to 40MHz
	 spi_clk = 27000000;
#elif  defined(MCU_IS_NXP_LPC17XX)
   M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);					// Setup SPI Clock. Here 96/4 = 24MHz for LPC17XX, upto 40MHz
	 spi_clk = 24000000;
#elif  defined(MCU_IS_HT32F16XX)
   M8266HostIf_SPI_SetSpeed(8);																// Setup SPI Clock. Here 72/4 = 18MHz for HT32F165x, upto 40MHz
	 spi_clk = 18000000;
#else
#error YOU SHOULD DEFINED MCU_IS_XXX near line 17 in brd_cfg.h
#endif

	 // wait clock stable
   M8266WIFI_Module_delay_ms(1);
   //It is very important to call M8266HostIf_SPI_Select() to tell the driver which SPI you used
 	 //and how faster the SPI clock you used. The function must be called before SPI access
	 if(M8266HostIf_SPI_Select((uint32_t)M8266WIFI_INTERFACE_SPI, spi_clk, &status)==0)
   {
		   while(1)  // flash LED1 and LED2 in frequency of 2Hz constantly when error
			 {
	          LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(250);
			      LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(250);
		   }
   }
	 
#ifdef M8266WIFI_SPI_ACCESS_USE_DMA
   //void M8266HostIf_SPI_DMA_Select(uint32_t spi_tx_dam_base_addr, uint32_t spi_tx_dam_stream_base_addr, uint32_t spi_rx_dam_base_addr, uint32_t spi_rx_dam_stream_base_addr);
	 M8266HostIf_SPI_DMA_Select((uint32_t)M8266WIFI_INTERFACE_SPI_TX_DMA, 
															(uint32_t)M8266WIFI_INTERFACE_SPI_TX_DMA_STREAM, 
															(uint32_t)M8266WIFI_INTERFACE_SPI_RX_DMA, 
															(uint32_t)M8266WIFI_INTERFACE_SPI_RX_DMA_STREAM
															);
#endif

#if 0 // test SPI communication is OK. You may give a loop for stress test. Comment it for formal release
{
	 volatile u32  i, j;
	 u8   byte;
	 for(i=0, j=0; i<100000; i++)
	 {
	 	  if(M8266WIFI_SPI_Interface_Communication_OK(&byte)==0) 	  //	if SPI fundamental Communication failed
				j++;
	 }
	 if(j>=10)
   {
		   while(1)  // flash LED1 and LED2 in frequency of 1s constantly when error
			 {
	          LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(500);
			      LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(500);
		   }
	 }
 }
#endif

 #if 1 // test SPI communication is OK. You may give a loop for stress test. Comment it for formal release
{
	 volatile u32  i, j;
	 u8   byte;

	 if(M8266WIFI_SPI_Interface_Communication_OK(&byte)==0) 	  									//	if SPI logical Communication failed
   {
		   while(1)  // flash LED1 and LED2 in frequency of 1s constantly when error
			 {
	          LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(500);
			      LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(500);
		   }
	 }
   
	 i = 100000;
	 j = M8266WIFI_SPI_Interface_Communication_Stress_Test(i);
	 if( (j<i)&&(i-j>10)) 		//  if SPI Communication stress test failed
   {
		   while(1)  // flash LED1 and LED2 in frequency of 1s constantly when error
			 {
	          LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(500);
			      LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(500);
		   }
	 }
 }
#endif
	 
 
#if 0	   // If you hope to reduce the Max Tx power, you could enable it by change to "#if 1"
	//u8 M8266WIFI_SPI_Set_Tx_Max_Power(u8 tx_max_power, u16 *status)
	if(M8266WIFI_SPI_Set_Tx_Max_Power(68, &status)==0)   // tx_max_power=68 to set the max tx power of aroud half of manufacture default, i.e. 50mW or 17dBm
	  return 0;
#endif	
	
#if 1 // If you hope to change the op_mode overrding the default loaded from flash, you could enable it by change to "#if 1"
	// BUT you are strongly recommended not to use "saved" if you call thie function each time upon bootup
	// Since it will write flash each time during bootup and the falsh might go to failure with a limited write life cycle of around tens of thousands times	
	//u8 M8266WIFI_SPI_Set_Opmode(u8 op_mode, u8 saved, u16* status);
	if(M8266WIFI_SPI_Set_Opmode(2, 0, &status)==0)  // set to AP mode, not saved // 1=STA Only, 2=AP Only, 3=STA+AP
    return 0;
#endif

#if 0 // If you hope to change the ap info overrding the default loaded from flash, you could enable it by change to "#if 1"
	// BUT you are strongly recommended not to use "saved" if you call thie function each time upon bootup
	// Since it will write flash each time during bootup and the falsh might go to failure with a limited write life cycle of around tens of thousands times	
  // u8 M8266WIFI_SPI_Config_AP(u8 ssid[13+1], u8 password[13+1], u8 enc, u8 channel, u8 saved, u16* status);	
	if(M8266WIFI_SPI_Config_AP("jiaxing", "jiahewanshixing", 4, 6, 0, &status)==0)  // set to 4=WPA_WPA2_PSK, not saved // 0=OPEN, 1=WEP, 2=WPA_PSK, 3=WPA2_PSK, 4=WPA_WPA2_PSK
    return 0;
#endif
	
  // get current op_mode, if STA mode or STA+AP mode, then, wait connection to external AP and getting ip
	//u8 M8266WIFI_SPI_Get_Opmode(u8* op_mode, u16* status);	
	if(M8266WIFI_SPI_Get_Opmode(&sta_ap_mode, &status)==0)  
    return 0;
		
	if(  (sta_ap_mode == 1)   // if STA mode
	   ||(sta_ap_mode == 3))  // if STA+AP mode
	{
#if 1 // If you hope to use the SSID and password here overriding the default loaded from on-module SPI flash, you could enable it by change to "#if 1"
	// BUT you are strongly recommended not to use "saved" if you call thie function each time upon bootup
	// Since it will write flash each time during bootup and the falsh might go to failure with a limited write life cycle of around tens of thousands times
	// Sure you could call it with "saved" once in your debug and change back to "not saved" to have a way, 
	// alternative to AT or Airkiss/WPS, to update the default in spi flash
		
		 //u8 M8266WIFI_SPI_STA_Connect_Ap(u8 ssid[32], u8 password[64], u8 saved, u8 timeout_in_s, u16* status);
		   if(M8266WIFI_SPI_STA_Connect_Ap("jiaxing", "jiahewanshixing", 0, 20, &status) == 0) // not saved, timeout=20s
			 {
				 return 0;
			 }
			 			 
#endif
			 //u8 M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(char* sta_ip, u8 max_wait_time_in_s)
		   if(M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(sta_ip, 10)==0) // max wait 10s to get sta ip
		   {
				  return 0; // printf("M8266WIFI module failed to get sta ip address!\n");
		   }
#if 1
{
			 //u8 SPI_Set_Web_Server(u8 open_not_shutdown, u16 server_port, u16* status)
			 if(SPI_Set_Web_Server(0, 3128, &status)==0)  // to shutdown the web_server, and the port value could be any value
		   {
				  return 0; // printf("M8266WIFI module failed to get sta ip address!\n");
		   }
       //u8 SPI_Set_Web_Server(u8 open_not_shutdown, u16 server_port, u16* status)
     	 if(SPI_Set_Web_Server(1, 3128, &status)==0)  // to (re-)start the web_server with port = 3128
		   {
				  return 0; // printf("M8266WIFI module failed to get sta ip address!\n");
		   }
}
#endif
			 
#if 1	 // these below two functions are just for examples on how to use them. You may or may not use them during module initialisation		 
			//u8 M8266WIFI_SPI_Get_STA_Connection_Status(u8* connection_status, u16* status)
			 if(M8266WIFI_SPI_Get_STA_Connection_Status(&connection_status, &status)==0)
			 {
					return 0;
			 }
			 //u8 M8266WIFI_SPI_STA_Query_Current_SSID_And_RSSI(char* ssid, u8* rssi, u16* status)
			 if(M8266WIFI_SPI_STA_Query_Current_SSID_And_RSSI(ssid, &rssi, &status)==0)
			 {
				 return 0;
			 }
#endif
			 
	}
  return 1;
 }
/*************************************************************************************
 * M8266WIFI_Config_Connection_Via_SPI                                               *
 * Description                                                                       *
 *    To reboot the module, wait wifi connection, and setup udp/tcp connection       *
 *    whether the response is what is expected via SPI                               *
 * Parameter(s):                                                                     *
 *    1. tcp_udp     : the type of connection to set up                              *
 *                     = 0, to set up connection of UDP                              *
 *                     = 1, to set up connection as TCP Client                       *
 *                     = 2, to set up connection as TCP Server                       *
 *    2. local_port  : to specify the local port if any                              *
 *                     if local_port=0, then a random local port will be generated   *
 *    3. remote_addr : the string of ip/domain address of remote server              *
 *                     e.g. "192.168.1.2" or "www.baidu.com"                         *
 *    4. remote_port : the port value of remote server                               *
 *    5. link_no     : the link_number to be used. max 8 links as per mutitiple link *
 * Return:                                                                           *
 *       0 = failed                                                                  *
 *       1 = success                                                                 * 
 *************************************************************************************/
u8 M8266WIFI_Config_Connection_via_SPI(u8 tcp_udp, u16 local_port, char* remote_addr, u16 remote_port, u8 link_no)
{
	u16 status=0;
  //Setup the TCP/UPD link
  //  u8 M8266WIFI_SPI_Setup_Connection(u8 tcp_udp, u16 local_port, u8* remote_ip, u16 remote_port, u8 link_no, u8 timeout_in_s, u16* status);
	return M8266WIFI_SPI_Setup_Connection(tcp_udp, local_port, remote_addr, remote_port, link_no, 20, &status);
}

/*************************************************************************************
 * M8266WIFI_Sleep_module_for_some_time_and_then_wakeup_automatically                *
 * Description                                                                       *
 *    - To (deep) sleep the module for some time via SPI                             *
 *    - All connections will get lost when entering sleep and after wakeup           *
 *      And should be re-established by the host                                     *
 *      e.g.                                                                         *
 *          	if(M8266WIFI_Sleep_M8266WIFI(5000)==0) // wakeup in 5000ms             *
 *		              return fail; // do somework here for failure                     *
 *            M8266WIFI_Module_Init_Via_SPI();       // re-initial the module        *
 *         or M8266WIFI_Config_Connection_via_SPI(); // re-establish the server      *
 * Parameter(s):                                                                     *
  *     1. time_to_wakeup_in_ms : time to wakeup from sleep, unit in ms              *
 *        - max value 4294967 ms (about 1.19hour).                                   *
 *          if a value exceed this provided, then 4294967ms will be used             *
 *        - use 0 if expect to sleep for ever without automatic wakeup               *
* Return:                                                                            *
 *       0 = failed                                                                  *
 *       1 = success                                                                 * 
 *************************************************************************************/
u8 M8266WIFI_Sleep_module_for_some_time_and_then_wakeup_automatically(u16 time_to_wakeup_in_ms)
{
	u16 status;
  //u8 M8266WIFI_SPI_Sleep_Module(u8 sleep_type, u32 time_to_wakeup_in_ms, u16 *status)
	if(M8266WIFI_SPI_Sleep_Module(3,time_to_wakeup_in_ms, &status)==0) //sleep_type=3=deep_sleep
		 return 0;

	if(time_to_wakeup_in_ms>0) // not a forever sleep
  {
		 M8266HostIf_Set_SPI_nCS_Pin(0);   // set nCS low to enaure the module will reboot from flash after wakeup
	   M8266WIFI_Module_delay_ms(time_to_wakeup_in_ms+1000);  // additional 1000ms for bootup time
	   M8266HostIf_Set_SPI_nCS_Pin(1);
	}
	
	return 1;
}

/*************************************************************************************
 * M8266WIFI_Sleep_Module                                                            *
 * Description                                                                       *
 *    To (deep) sleep the module forever via SPI and should be wakeup by host        *
 *    After calling of this function, all connection will get lost                   *
 * Parameter(s):                                                                     *
 *    None                                                                           *
* Return:                                                                            *
 *       0 = failed                                                                  *
 *       1 = success                                                                 * 
 *************************************************************************************/
u8 M8266WIFI_Sleep_Module(void)
{
	u16 status;
  //u8 M8266WIFI_SPI_Sleep_Module(u8 sleep_type, u32 time_to_wakeup_in_ms, u16 *status)
	if(M8266WIFI_SPI_Sleep_Module(3, 0, &status)==0) //sleep_type=3=deep_sleep
		 return 0;
	
	return 1;
}
/*************************************************************************************
 * M8266WIFI_Wakeup_Module                                                           *
 * Description                                                                       *
 *    To wakeup the module                                                           *
 *    - All connections will get lost when entering sleep and after wakeup           *
 *      And should be re-established by the host after wakeup                        *
 *      e.g.                                                                         *
 *          	if(M8266WIFI_Wakeup_Module()==0)                                       *
 *		        {      ; // do somework here for failure                               *
 *            }                                                                      *
 *            else                                                                   *
 *            {                                                                      *
 *               M8266WIFI_Config_Connection_via_SPI(); // re-establish the server   *
 *            }                                                                      *
 *                                                                                   *
 * Parameter(s):                                                                     *
 *    None                                                                           *
* Return:                                                                            *
 *       0 = failed                                                                  *
 *       1 = success                                                                 * 
 *************************************************************************************/
u8 M8266WIFI_Wakeup_Module(void)
{
	 return M8266WIFI_Module_Init_Via_SPI();
}
