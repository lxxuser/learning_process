#ifndef MODBUS_H
#define MODBUS_H

#include "stm32f4xx.h"
#include "modbus_crc.h"
#include "rs485.h"
#include "modbus_tim.h"
//从机数据类型
typedef struct 
{
    u8 myaddr;//地址
    u8 rcbuf[100];//接收缓冲区
    u8 timout; //数据持续时间
    u8 recount; //接收数据数
    u8 timrun; //定时器记时标志
    u8 reflag; //接收完成标志位
    u8 sendbuf[100]; //发送缓冲区
}MODBUS;

extern  MODBUS modbus;
void MODBUS_Init();
void Modbus_Func3();
void Modbus_Func16();
void MODBUS_Handle();

#endif

