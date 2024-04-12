#include "sys.h"
#include "usart.h"
#include "key.h"
#include "delay.h"
#include "modbus_tim.h"
#include "rs485.h"
#include "modbus.h"

int main(void)
{ 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);	 //串口1初始化为9600（只用来打印测试数据）
	Modbus_uart2_init(115200);//初始化modbus串口2和485控制引脚
	Modbus_TIME3_Init(16800-1,10-1);//定时器初始化参数1是重装载数，参数2是分频系数//1ms中断一次
	MODBUS_Init();//MODBUS初始化--本机做作为从机设备地址，本机要匹配的从机地址
	
	while(1)
	{
		MODBUS_Handle();//Modbus事件处理函数(执行读或者写的判断)--从机地址01
		 
		//LED2=~LED2;
		delay_ms(1000);
	}
}

