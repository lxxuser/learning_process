#include "sys.h"
#include "usart.h"
#include "key.h"
#include "delay.h"
#include "modbus_tim.h"
#include "rs485.h"
#include "modbus.h"
int slave=0;//�ӻ�id
int main(void)
{ 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);	 //����1��ʼ��Ϊ9600��ֻ������ӡ�������ݣ�
	Modbus_uart2_init(115200);//��ʼ��modbus����2��485��������
	Modbus_TIME3_Init(16800-1,10-1);//��ʱ����ʼ������1����װ����������2�Ƿ�Ƶϵ��//1ms�ж�һ��
	MODBUS_Init();//MODBUS��ʼ��--��������Ϊ�ӻ��豸��ַ������Ҫƥ��Ĵӻ���ַ
	
	while(1)
	{
		slave=1;
		if(modbus.Host_time_flag)//ÿ1s����һ������
		{
			USART_SendData(USART2,1);
			//����1���鿴��i���ӻ�����
			Host_Read03_slave(slave,0x0000,0x0003);//������2��ʼ��ַ������3�Ĵ�������
			if(modbus.Host_send_flag)
			{
				modbus.Host_Sendtime=0;//������Ϻ�������㣨�����ϴε�ʱ�䣩
				modbus.Host_time_flag=0;//�������ݱ�־λ����
				modbus.Host_send_flag=0;//��շ��ͽ������ݱ�־λ
				
				HOST_ModbusRX();//�������ݽ��д���

			}
		}
		 
		//LED2=~LED2;
		delay_ms(1000);
	}
}

