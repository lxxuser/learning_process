/*                                                    
	                                                    ʹ��˵��
												��Ƭ�������������������ӻ�
								�ó����е�MODBUSЭ�鲢�����ƣ�Ҳ�����Ͻ�����ʵ�ֵĹ���ֻ�������ڱ�����
								�û�����ֱ�ӵ��ñ������е�MODBUSЭ��ȥִ���������̣�����ᵼ�¸��ִ���

*/

#include "modbus.h"


rt_sem_t modbus_respond;
rt_timer_t modbus_timer;

modbus_u8 modbus_rx_buf[modbus_rx_max_len] = {0};
modbus_u8 modbus_rx_falg = modbus_rx_off;
modbus_u16 modbus_rxdata_len = 0;
modbus_u32 modbus_time = 0;
modbus_u32 modbus_time_interval = (modbus_u32)(modbus_ms_interval + 1);

//�û��Զ���
modbus_u16 Modbus_DataBuff[8] = {0};

/*
�������ܣ�������ӻ���ѯ�Ĵ�������
������slave_address����ѯ�Ĵӻ���ַ start_address����ѯ�Ĵ����Ŀ�ʼ��ַ reg_number����ѯ�ļĴ�������
����ֵ��void
��ע�������룺0x03
*/
void MODBUS_HostToSlave_H03(modbus_u8 slave_address,modbus_u16 start_address,modbus_u16 reg_number)
{
	modbus_u8 tx_buf[8] = {0};
	modbus_u16 crc = 0;
	
	tx_buf[0] = slave_address;
	tx_buf[1] = 0x03;
	tx_buf[2] = start_address >> 8;
	tx_buf[3] = start_address & 255;
	tx_buf[4] = reg_number >> 8;
	tx_buf[5] = reg_number & 255;
	crc = CRC_Verification(tx_buf,6);
	tx_buf[6] = crc >> 8;
	tx_buf[7] = crc & 255;
	
	modbus_send_data(tx_buf,8);
	
}

/*
�������ܣ������޸Ĵӻ������Ĵ�������
������slave_address���޸ĵĴӻ���ַ start_address���޸ĵļĴ����Ŀ�ʼ��ַ data���޸ĵ���������
����ֵ��void
��ע�������룺0x06
*/
void MODBUS_HostToSlave_H06(modbus_u8 slave_address,modbus_u16 start_address,modbus_u16 data)
{
	modbus_u8 tx_buf[8] = {0};
	modbus_u16 crc = 0;
	
	
	tx_buf[0] = slave_address;
	tx_buf[1] = 0x06;
	tx_buf[2] = start_address >> 8;
	tx_buf[3] = start_address & 255;
	tx_buf[4] = data >> 8;
	tx_buf[5] = data & 255;
	crc = CRC_Verification(tx_buf,6);
	tx_buf[6] = crc >> 8;
	tx_buf[7] = crc & 255;
	
	modbus_send_data(tx_buf,8);
	
	
}
/*
�������ܣ���������ӻ��Ļ�Ӧ
������slave_address������Ĵӻ��ĵ�ַ 
����ֵ��modbus_32
��ע��
modbus_reply_h03��03��������Ҳ����ɹ� �ҷ���������
modbus_reply_h06��06�������޸ļĴ����ɹ�
modbus_reply_noreg��03��������Ҳ����ɹ� �����޲��ҵļĴ���
modbus_reply_cfailed��06�������޸�ʧ�� ԭ�����Ϊ:�üĴ���ֻ���򲻴��ڵļĴ�����
modbus_reply_addr_err���ӻ���ַ����
modbus_reply_data_err�����ݴ���
modbus_reply_err��δ֪����
*/
modbus_32 MODBUS_Handle_Reply(modbus_u8 slave_address)
{
	
	modbus_u8 len = 0;
	
	//crcУ��
	modbus_u16 crc = (modbus_rx_buf[modbus_rxdata_len - 2] << 8) | modbus_rx_buf[modbus_rxdata_len - 1];
	if(crc != CRC_Verification(modbus_rx_buf,modbus_rxdata_len - 2))
		return modbus_reply_data_err;
	
	//��ַλ�ж�
	if(modbus_rx_buf[0] != slave_address)
		return modbus_reply_addr_err;
	
	//�������ж�
	switch(modbus_rx_buf[1])
	{
		case 0x03:
			memset(Modbus_DataBuff,0,8);
			len = modbus_rx_buf[2] / 2; 
			for(modbus_u8 i = 0;i < len;i ++)
			{
				Modbus_DataBuff[i] = (modbus_rx_buf[3 + (i * 2)] << 8) | modbus_rx_buf[4 + (i * 2)];
			}
			return modbus_reply_h03;
		case 0x06:
			return modbus_reply_h06;
		case 0x83:
			return modbus_reply_noreg;
		case 0x86:
			return modbus_reply_cfailed;
		default:
			return modbus_reply_err;
	
	}
	
}


/*
�������ܣ�CRCУ��
������buff������֡��ȥCRCУ���ʣ��Ĳ��� len��ʣ�ಿ�ֵĳ���
����ֵ��16λ��CRCУ��ֵ
��ע��
*/
int CRC_Verification(uint8_t *buff,int len)
{
	unsigned short tmp = 0xffff;
    unsigned short ret1 = 0;
   

    for (int n = 0; n < len; n++) {/*�˴���n -- ҪУ���λ��Ϊn��*/
        tmp = buff[n] ^ tmp;
        for (int i = 0; i < 8; i++) {  /*�˴���8 -- ָÿһ��char������8bit��ÿbit��Ҫ����*/
            if (tmp & 0x01) {
                tmp = tmp >> 1;
                tmp = tmp ^ 0xa001;
            }
            else {
                tmp = tmp >> 1;
            }
        }
    }
    /*CRCУ����ֵ*/
    //printf("%X\n", tmp);
    /*��CRCУ��ĸߵ�λ�Ի�λ��*/
    ret1 = tmp >> 8;
    ret1 = ret1 | (tmp << 8);
    //printf("ret: %X\n", ret1);
	return ret1;

}

/*
�������ܣ�modbus_tmer �ص�����
������  void *parameter
����ֵ��void
��ע��
��֡���Ϊ3.5���ֽ����ж���һ֡���ݴ�������
*/
void modbus_callback(void *parameter)
{
	modbus_time ++ ;
	if(modbus_time > modbus_time_interval && modbus_rx_falg == modbus_rx_on)
	{
		modbus_rx_falg = modbus_rx_ok;
		rt_sem_release(modbus_respond);
		
	}
		
}
