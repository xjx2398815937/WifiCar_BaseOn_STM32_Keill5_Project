//			DataL = (Camera_Data & 0xff);
//			DataH = (Camera_Data>>8) &0xff;
//			Datah = &DataH;
//			Datal = &DataL;
////			for(int m = 0;m < 8;m++)
////			{
////				Datal[m] = DataL & 0x01; 
////				Datah[m] = DataH & 0x01;
////				DataL = DataL >>1;
////				DataH = DataH >>1; 
////			}
//		
//			M8266WIFI_SPI_Send_Data(Datal,8,NULL,NULL);
//			printf("%d",*Datal);
//			M8266WIFI_SPI_Send_Data(Datah,8,NULL,NULL);
//			printf("%d",*Datah);



//	uint8_t DataH,DataL;
//	uint8_t *Datah,*Datal;
//	uint8_t Datah[8],Datal[8];
//			printf("%d",Camera_Data);
//			for(int m = 0; m < 16; m++)
//			{
//				data[m] = Camera_Data & 0x0001;
//				Camera_Data = Camera_Data>>1;
//			}
//			M8266WIFI_SPI_Send_Data(data,16 ,NULL,NULL);