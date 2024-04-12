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
	uart_init(115200);	 //����1��ʼ��Ϊ9600��ֻ������ӡ�������ݣ�
	Modbus_uart2_init(115200);//��ʼ��modbus����2��485��������
	Modbus_TIME3_Init(16800-1,10-1);//��ʱ����ʼ������1����װ����������2�Ƿ�Ƶϵ��//1ms�ж�һ��
	MODBUS_Init();//MODBUS��ʼ��--��������Ϊ�ӻ��豸��ַ������Ҫƥ��Ĵӻ���ַ
	
	while(1)
	{
		MODBUS_Handle();//Modbus�¼�������(ִ�ж�����д���ж�)--�ӻ���ַ01
		 
		//LED2=~LED2;
		delay_ms(1000);
	}
}

