#ifndef BC28_H
#define BC28_H

#include "stm32f4xx.h"
#include <stdio.h>
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include <rtdef.h>
#include "drv_bc28.h"

#define BC28_RX_ON    0
#define BC28_RX_OFF   1
#define BC28_RX_OK    2
#define ProductKey "k0c3d91mYBS"						
#define DeviceName "water_tx"					
#define DeviceSecret "5204e7d577baa4d9ba675a4de8638cc8" 

#define Domain "139.224.42.2"
#define Port "1883"														                
#define ConnectID "water_tx"
#define PubTopic "/sys/k0c3d91mYBS/water_tx/thing/event/property/post"			      
#define SubTopic "/sys/k0c3d91mYBS/water_tx/thing/event/property/post" 

extern rt_uint8_t BC28_RX_BUF[200];//BC28接收数组
extern rt_uint8_t BC28_RX_FLAG;//BC28接收标志位
extern rt_uint16_t BC28_RX_DATA_LEN;//BC28接收数据长度

int BC28_Init(void);
int MQTT_Init(void);
int MQTT_PUB(char *data);

#endif

