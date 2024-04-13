#ifndef MODBUS_H
#define MODBUS_H

#include "stm32f4xx.h"
#include "modbus_crc.h"
#include "rs485.h"
#include "modbus_tim.h"
//从机数据类型
typedef struct 
{
    //从机部分代码
    u8 myaddr;//地址
    u8 rcbuf[100];//接收缓冲区
    u8 timout; //数据持续时间
    u8 recount; //接收数据数
    u8 timrun; //定时器记时标志
    u8 reflag; //接收完成标志位
    u8 sendbuf[100]; //发送缓冲区

    //主机代码
    u8 Host_Txbuf[8];	//modbus发送数组	
	u8 slave_add;		//要匹配的从机设备地址（做主机实验时使用）
	u8 Host_send_flag;//主机设备发送数据完毕标志位
	int Host_Sendtime;//发送完一帧数据后时间计数
	u8 Host_time_flag;//发送时间到标志位，=1表示到发送数据时间了
	u8 Host_End;//接收数据后处理完毕


}MODBUS;

extern  MODBUS modbus;
void MODBUS_Init();
void Modbus_Func3();
void Modbus_Func16();
void MODBUS_Handle();

//void Host_send03(void);
void Host_Read03_slave(uint8_t slave,uint16_t StartAddr,uint16_t num);
void Host_RX(void);
//主机接收从机的消息进行处理
void HOST_ModbusRX(void);
void RS485_Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num);
#endif

