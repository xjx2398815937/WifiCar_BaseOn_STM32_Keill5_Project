【Files decriptions】

----------------------------HARDWARE INTERFACE------------------------------------------
- M8266WIFIHostIf.c -- example source of MCU inteface to M8266WIFI Module              -
- M8266WIFIHostIf.h -- example header of MCU inteface to M8266WIFI Module              -
Notes:                                                                                 -
  1. Hardware Layer fundamental control/query operations including GPIO/UART/SPI       -
     Operations                                                                        -
  2. Should be adjust according to the specific hardware logic                         -
----------------------------------------------------------------------------------------

----------------------------DRIVER------------------------------------------------------
- M8266WIFIDRV.lib  -- Driver.lib of MCU to perform SPI-WIFI transparent transception
单片机驱动程序库实现SPI-wifi透明传输
- M8266WIFIHostIf.h -- example header of MCU inteface to M8266WIFI Module
m8266wifi模块单片机接口实例头 
Notes:                                                                                 -
  1. Driver Layer operations mainly for High-Speed SPI-WIFI Operations,                -
     including High-Speed SPI-WIFI transparent transception, timing, hands-shaking etc 
	 驱动层操作主要用于高速spi-wifi操作，包括高速spi-wifi透明传输、定时、握手等。-
  2. PLEASE DO NOT MODIFY THEM. Just embed them into your project work bentch
请不要修改它们，只需将它们嵌入到您的项目工作中即可   -
----------------------------------------------------------------------------------------


----------------------------HAL OPERATIONS----------------------------------------------
- M8266WIFI_ops.c   -- example source of MCU HAL operations to M8266WIFI module
m8266wifi模块的单片机Hal操作实例 -
- M8266WIFI_ops.h   -- example header of MCU HAL operations to M8266WIFI module        -
Notes:                                                                                 -
  1. Hardare Abtraction Layer operations for MCU to config and communication with      -
     M8266WIFI module
单片机的硬件缩减层操作与-m8266wifi模块的配置和通信 	 -
  2. You may please extend the operations following these example functions 
请按照以下示例函数扩展操作   -
----------------------------------------------------------------------------------------

----------------------------APPLICATIONS__----------------------------------------------
- test_m8266wifi.c  -- example application source of test M8266WIFI module
示例应用程序源：测试M8266WiFi模块 -
Notes:                                                                                 -
  1. example testing application to test speed of MCU-WIFI transparent transception 
单片机无线透明传输速度测试应用实例   -
  2. You may please modify or reference it for your own application
  您可以为您自己的应用程序修改或引用它
----------------------------------------------------------------------------------------
 
【Architecture】

  ------------
 |APPLICATIONS|
  ------------
        \
         \
          \
       ----------------
      | HAL OPERATIONS |
       ----------------
             \
              \
               \
           ---------
          | DRIVER  |
           ---------
                 \
                  \
                   \
            --------------------
           | HARDWARE INTERFACE |
            --------------------

【Usage】
       
 1. Implement the sources of hardware interface according to your hardware design
根据您的硬件设计实现硬件接口的来源
    Maily including 
     - GPIO initialization and operations for nCS and nRESER
	 NCS和nRESER的GPIO初始化和操作
     - SPI  initialization and read/write operations
	 SPI初始化和读写操作
     - UART initialization and read/write operations
	 UART初始化和读写操作
    You are recommended to uses the manner of macros following brd_cfg.h
议在BRD_CFG.H之后使用宏的方式
 2. Add Driver Lib into your project
将驱动程序Lib添加到项目中
 3. Copy sources of HAL OPERATION to your project
对您的项目执行HAL操作的多个源
    If necessary, you may modify or extend your own HAL operations according to 
    AT commands and other related protocols 
如果需要，可以根据AT命令和其他相关协议修改或扩展自己的HAL操作。
 4. Compose your applications referencing to APPLICATION SOURCE
编写引用应用程序源的应用程序




AnyLinkin!
Link anytime, anywhere, and anyhow!
IoT@anylinkin.com
http://www.anylinkin.com
htpp://anylinkin.taobao.com
              