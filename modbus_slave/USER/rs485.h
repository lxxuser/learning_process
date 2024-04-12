#ifndef RS485_H
#define RS485_H

#include "stm32f4xx.h"
#include "sys.h"
#include "modbus.h"
#define RS485_RX_ENABLE GPIO_ResetBits(GPIOG,GPIO_Pin_8) //接收使能、低电平有效
#define RS485_TX_ENABLE GPIO_SetBits(GPIOG,GPIO_Pin_8) //发送使能、高电平有效

void Modbus_Send_Byte(u8 Modbus_byte);
void Modbus_uart2_init(u32 bound);

#endif

