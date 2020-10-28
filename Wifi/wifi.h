#ifndef __WIFI_H
#define __WIFI_H

#include "stdio.h"
#include "sys.h"
#include "delay.h"
#include "misc.h"
#include "M8266HostIf.h"
#include "led.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
#include "brd_cfg.h"

void WifiConfig(void);
extern u8 M8266WIFI_SPI_Has_DataReceived(void);//标志位:如果wifi有值，返回值位1，反之；
extern u16 M8266WIFI_SPI_Send_Data(u8 Data[], u16 Data_len, u8 link_no, u16* status);//发送的数据，数据长度，链接号，状态
extern u16 M8266WIFI_SPI_RecvData(u8 Data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);//接受的数据，最大的数据长度，等待超时的时间，链接号，状态
extern u8 M8266WIFI_SPI_Get_STA_Connection_Status(u8* connection_status, u16* status);
#endif   /*__WIFI_H*/
