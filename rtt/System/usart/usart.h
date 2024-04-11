#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include <rtthread.h>
#include <rtdbg.h>
#include "cd100m.h"
#include "app.h"

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

extern uint8_t       g_uart_sta; 
extern rt_mailbox_t  g_mail_data;
static uint8_t 					g_recv_da[5];
static uint16_t 				g_cnt = 0;
void usart_init(u32 bound);
void uart2_init(uint32_t baud);

void uart3_init(uint32_t baud);

void uart2_send(uint8_t* msg,uint16_t len);
void USART3_Send_Data(uint8_t *data,uint16_t len);//bc28
#endif


