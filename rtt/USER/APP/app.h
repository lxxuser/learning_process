#ifndef __APP_H
#define __APP_H

#include <rtthread.h>
#include <rtdbg.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stm32f4xx.h"
#include "bc28.h"
#include "hx711.h"
#include "cd100m.h"
#include <rtdef.h>
#include <rtconfig.h>
#include <drv_bc28.h>
#include "user_defined.h"
#include "drv_hx711.h"
#include "cd100m.h"
#include "usart.h"
#define th1_test_priority   18   //MQTT初始化
#define th2_test_priority   19   //上传数据至阿里云
#define th3_test_priority   20 	 //CD100M信息
#define th4_test_priority   17   //CD100M中断处理
extern rt_sem_t BC28_Respond;
extern rt_sem_t CD100M_RECV;

extern struct rt_thread th1;
extern rt_uint8_t th1_stack[1024];

extern struct rt_thread th2;
extern rt_uint8_t th2_stack[1024];

extern struct rt_thread th3;
extern rt_uint8_t th3_stack[1024];

extern struct rt_thread th4;
extern rt_uint8_t th4_stack[1024];

void MQTT_INIT(void *parameter);
void MQTT_PUB_DATA(void *parameter);
void CD100M_INFO(void *parameter);
void CD100M_CLEAR(void *parameter);
#endif
