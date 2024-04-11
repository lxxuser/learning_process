#include "sensor.h"


STRUCT_DEBUGTypeDef   debug_value;


#if RS485_CONTROL_TX_RX
/*
�������ܣ�RS485 CON���ƿڳ�ʼ��
������void
����ֵ��void
��ע��CON��ʹ��������485�ķ��ͺͽ��� TD301D������485��һ�� 
	  CONΪ�ߵ�ƽʱ����
	  CONΪ�͵�ƽʱ����
*/
void RS485_CON_Init(void)
{
	rcu_periph_clock_enable(RS485_CON_CLOCK);//��GPIOʱ��
	
	gpio_init(RS485_CON_PORT,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,RS485_CON_PIN);//GPIO����ģʽ����
	
	RS485_CON_TX();//����Ĭ�Ϸ���״̬
}
/*
�������ܣ����ڷ���1���ֽڵ�ʱ����ʱ ��λms
������baud �� ���ڲ�����
����ֵ��void
��ע������
	�����ڲ�����Ϊ9600ʱ ���ڴ���1�ֽڵ�ʱ��ԼΪ1.04ms ���ô���ʱ���������� ����ʱ1.5ms
	�����ڲ�����Ϊ2400ʱ ���ڴ���1�ֽڵ�ʱ��ԼΪ4.16ms ���ô���ʱ���������� ����ʱ4.5ms
	�����ڲ�����Ϊ115200ʱ ���ڴ���1�ֽڵ�ʱ��ԼΪ0.08ms ���ô���ʱ���������� ����ʱ0.5ms
	ע�����ﲻ����RT-Thread�������
*/
void RS485_TIME_OneByte(unsigned int baud)
{
	
	float ms = ((1.0/((float)baud/10/1000)) * 1);
	
	char time = (char)ms;
	
	if(ms - (float)time >= 0.5)
	{
		time = (time * 2) + 2;
	}
	else
	{
		time = (time * 2) + 1;
	}
	
	while(time -- )
	{
		rt_hw_us_delay(500);
	}
	
}
#endif

/*
�������ܣ���ȡ������ĳ������
������enumTYPE �� ����    REG ����Ӧ��ַ
����ֵ��true : �ɹ�  false :ʧ��
��ע��
enumTYPE ��               REG:
PH��	   TYPE_PH            REG_PH    0x0006
ʪ�ȣ�     TYPE_HUM			  REG_HUM   0x0012
�¶ȣ�	   TYPE_TEM           REG_TEM   0x0013
�絼�ʣ�	   TYPE_EC            REG_EC    0x0015
��ַ��     TYPE_ADD           REG_ADD   0x0100
�����ʣ�	   TYPE_BAUD          REG_BAUD  0x0101
		   TYPE_FREE          ����ҵļĴ���
		   
ע����enumTYPEΪTYPE_FREEʱ����Ϊ��������Ĵ��� ��ʱ��REG����������Ĵ�����Ӧ��ַ
	����ʱ����_FREE��ֵΪ�޷���16λ����
*/
bool Sensor_TakeAValue(ENUM_TYPETypeDef enumTYPE,unsigned short REG)
{
	
	
#if RS485_CONTROL_TX_RX
	
	RS485_CON_TX();//485����
	
#endif
	
	MODBUS_HostToSlave_H03(Slave_address,REG,1);//modbus����

#if RS485_CONTROL_TX_RX	

    //Ϊ��ֹ485��CON�ӿڵ�ƽ������ƻ�����֡����������1���ֽڵ���ʱ���Ҳ�����RT-Thread�������	
	RS485_TIME_OneByte(modbus_uart_baud);
	
	RS485_CON_RX();//485����

#endif
	
	if(rt_sem_take(modbus_respond,100) == 0)//��ȴ�ʱ��100ms
	{
		
		if(MODBUS_Handle_Reply(Slave_address) != modbus_reply_h03)//��֤����
		{
			modbus_rxdata_len = 0;//��־λ����
		
			rt_memset(modbus_rx_buf,0,modbus_rx_max_len);//�������
			
			return false;
		}
			
		modbus_rxdata_len = 0;//��־λ����
		
		rt_memset(modbus_rx_buf,0,modbus_rx_max_len);//�������
		
		switch(enumTYPE)//�ж�����
		{
			case TYPE_PH:
				zigbee_value.PH = Modbus_DataBuff[0];
				debug_value._PH = (float)(Modbus_DataBuff[0]) / 100.0;
				return true;
			case TYPE_HUM:
				zigbee_value.HUM = Modbus_DataBuff[0];
				debug_value._HUM = (float)(Modbus_DataBuff[0]) / 10.0;
				return true;
			case TYPE_TEM:
				zigbee_value.TEM = Modbus_DataBuff[0];
				if(Modbus_DataBuff[0] & (1 << 15))//���¶ȵ�������ʱ�Բ������ʽ�ϴ�
				{
					debug_value._TEM = (float)(Modbus_DataBuff[0] - 0xFFFF) /10.0;//������ԭ��
				}
				else
				{
					debug_value._TEM = (float)(Modbus_DataBuff[0]) / 10.0;
				}
				return true;
			case TYPE_EC:
				zigbee_value.EC = Modbus_DataBuff[0];
				debug_value._EC = Modbus_DataBuff[0];
				return true;
			case TYPE_ADD:
				debug_value._ADD = Modbus_DataBuff[0];
				return true;
			case TYPE_BAUD:
				if(Modbus_DataBuff[0] == 0)
					debug_value._BAUD = 2400;
				else if(Modbus_DataBuff[0] == 1)
					debug_value._BAUD = 4800;
				else if(Modbus_DataBuff[0] == 2)
					debug_value._BAUD = 9600;
				else
					debug_value._BAUD = 0;
				return true;
			case TYPE_FREE:
				debug_value._FREE = Modbus_DataBuff[0];
				return true;
			default:
				return false;

		}
		
	
	}
	else
		return false;
	
	
}
/*
�������ܣ��޸Ĵӻ������Ĵ���ֵ
������  REG ����Ӧ��ַ value ���޸ĵ�ֵ
����ֵ��int
��ע��
����ֵ��
-999��                 �������޻�Ӧ
modbus_reply_h06��     06�������޸ļĴ����ɹ�
modbus_reply_cfailed�� 06�������޸�ʧ�� ԭ�����Ϊ:�üĴ���ֻ���򲻴��ڵļĴ�����
modbus_reply_data_err�����ݴ���
modbus_reply_err��     δ֪����

*/
int Sensor_MakeAValue(unsigned short REG,unsigned short value)
{
	
	int result;
	
	MODBUS_HostToSlave_H06(Slave_address,REG,value);
	
	if((rt_sem_take(modbus_respond,100) == 0) || modbus_rx_falg == modbus_rx_ok)//��ȴ�ʱ��100ms
	{
		
		result = MODBUS_Handle_Reply(Slave_address);
		
		modbus_rxdata_len = 0;//��־λ����
		
		rt_memset(modbus_rx_buf,0,modbus_rx_max_len);//�������
		
		return result;
		
	}
	else
		return -999;
	
}

/*
�������ܣ��޸Ĵӻ�������
������  baud �����ʣ���֧�� 2400  4800 9600��
����ֵ��true �� �޸ĳɹ�  false ���޸�ʧ��
��ע��
*/
bool Modify_Baud_Rate(unsigned int baud)
{
	
	unsigned short data;
	
	switch(baud)
	{
		case 2400:
			data = 0;
			break;
		case 4800:
			data = 1;
			break;
		case 9600:
			data = 2;
			break;
		default:
			return false;
	}
	
	
	if(Sensor_MakeAValue(REG_BAUD,data) == modbus_reply_h06)
		return true;
	else
		return false;

}

/*
�������ܣ��޸Ĵӻ���ַ
������  address ����ַ������С��256��
����ֵ��true �� �޸ĳɹ�  false ���޸�ʧ��
��ע��
*/
bool Modify_Address(unsigned short addre)
{
	if(Sensor_MakeAValue(REG_ADD,addre) == modbus_reply_h06)
		return true;
	else
		return false;

}
	


