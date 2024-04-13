#include "sys.h"
#include "usart.h"
#include "key.h"
#include "delay.h"
#include "modbus_tim.h"
#include "rs485.h"
#include "modbus.h"
int slave=0;//从机id
int main(void)
{ 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);	 //串口1初始化为9600（只用来打印测试数据）
	Modbus_uart2_init(115200);//初始化modbus串口2和485控制引脚
	Modbus_TIME3_Init(16800-1,10-1);//定时器初始化参数1是重装载数，参数2是分频系数//1ms中断一次
	MODBUS_Init();//MODBUS初始化--本机做作为从机设备地址，本机要匹配的从机地址
	
	while(1)
	{
		slave=1;
		if(modbus.Host_time_flag)//每1s发送一次数据
		{
			USART_SendData(USART2,1);
			//参数1：查看第i个从机数据
			Host_Read03_slave(slave,0x0000,0x0003);//，参数2起始地址，参数3寄存器个数
			if(modbus.Host_send_flag)
			{
				modbus.Host_Sendtime=0;//发送完毕后计数清零（距离上次的时间）
				modbus.Host_time_flag=0;//发送数据标志位清零
				modbus.Host_send_flag=0;//清空发送结束数据标志位
				
				HOST_ModbusRX();//接收数据进行处理

			}
		}
		 
		//LED2=~LED2;
		delay_ms(1000);
	}
}

