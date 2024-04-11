#include "zigbee_a72.h"

#define DBG_TAG "a72"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_device_t A72_dev;

A72_u8 A72_send_order_buf[20];//����ָ��BUF
A72_u8 A72_send_data_buf[100];//��������BUF
A72_u8 A72_RX_BUF[128];

/*
��������˵����
���ߴ���֧�̵ֶ�ַ��IEEE ��ַ���鲥���㲥�ȴ��䷽ʽ��
�̵�ַ�� IEEE ��ַ���䵥������� 79 �ֽڡ�
�鲥���䵥������� 80 �ֽڡ�
�㲥���䵥������� 81 �ֽڡ�
*/

A72_Return_Value _value;


//�ָ���������
A72_Instruction_Format _RFS = 
{.command = C_RFS,.length = L_RFS,.verification = V_RFS,.Order_Bytes = RFS_Bytes};

//��λ
A72_Instruction_Format _RESET =
{.command = C_RESET,.length = L_RESET,.verification = V_RESET,.Order_Bytes = RESET_Bytes};

//��ȡ��ַָ���ʼ��
A72_Instruction_Format READ_ADDERSS = 
{.command = C_Read_Adder,.length = L_Read_Adder,.verification = V_Read_Adder,.Order_Bytes = Read_Adder_Bytes};

//��ȡ�豸����ָ���ʼ��
A72_Instruction_Format Device_Type = 
{.command = C_Device_Type,.length = L_Device_Type,.verification = V_Device_Type,.Order_Bytes = Device_Type_Bytes};

#if ZIGBEE_A72_COORDINATOR
//��������
A72_Instruction_Format CreateNet = 
{.command = C_CreateNet,.length = L_CreateNet,.verification = V_CreateNet,.Order_Bytes = CreateNet_Bytes};
#endif

#if ZIGBEE_A72_NODE
//����״ָ̬���ʼ��
A72_Instruction_Format Connect_status = 
{.command = C_Connect_status,.length = L_Connect_status,.verification = V_Connect_status,.Order_Bytes = Connect_status_Bytes};

//��ѯ����ʱ��ָ���ʼ��
A72_Instruction_Format Sleep_time =
{.command = C_Sleep_time,.length = L_Sleep_time,.verification = V_Sleep_time,.Order_Bytes = Sleep_time_Bytes};

#endif

//IEEE��ַ����ģʽ��ʼ��
A72_SEND_MODE SEND_MODE_IEEE = 
{.L_Send_Data = L_Send_Data_IEEE,.M_Send_Data = M_Send_Data_IEEE,.Address_Length = 8};

//�̵�ַ����ģʽ��ʼ��
A72_SEND_MODE SEND_MODE_SHORT = 
{.L_Send_Data = L_Send_Data_SHORT,.M_Send_Data = M_Send_Data_SHORT,.Address_Length = 2};

//�㲥����ģʽ��ʼ��
A72_SEND_MODE SEND_MODE_BROADCAST = 
{.L_Send_Data = L_Send_Data_BROADCAST,.M_Send_Data = M_Send_Data_BROADCAST,.Address_Length = 2};

/*
�������ܣ�����A72�޻�Ӧ��ָ��
������void
����ֵ��int
��ע��
*/
int A72_NoReply(void)
{
	rt_thread_mdelay(20);
	return SUCCESS;
}

/*
�������ܣ��鿴�ָ��������û�Ӧ
������void
����ֵ��SUCCESS:�ɹ� ERROR:ʧ��
��ע��
	ע:�ú���ΪA72_SEND_ORDER�����Ĳ��� ���ɶ��Ե���
*/
int A72_Read_RFS(void)
{
	A72_u8 RX_Correct = 0;//������ȷ��־
	A72_u8 i = 0;//��ʱ����

//	memset(A72_RX_BUF,0,128);
	
	if(rt_sem_take(A72_Respond,50) == 0)//�ȴ�������� �ȴ�ʱ��50ms
	{
		for(i=0;i<20;i++)
		{
			
			if(A72_RX_BUF[i] == A72_Sbit && RX_Correct == 0)
				if(A72_RX_BUF[i+1] == 0x80 && A72_RX_BUF[i+2] == 0x00)//��Ӧ���0x81C0
					RX_Correct = 1;//������ȷ
				
			if(RX_Correct == 1)//������յ���ȷ������
			{
				i += 5;//��λ
				
				
				if(A72_RX_BUF[i] == 0x81 && A72_RX_BUF[i+1] == 0x00 && A72_RX_BUF[i+2] == 0x05)
				{
					return SUCCESS;
				}
				else
				{
					return ERROR;
				}
				
			}
			
		}
		
	}
				
	return ERROR;
}


/*
�������ܣ��鿴��ȡ��ַ��Ӧ
������void
����ֵ��SUCCESS:�ɹ� ERROR:ʧ��
��ע��
	ע:�ú���ΪA72_SEND_ORDER�����Ĳ��� ���ɶ��Ե���
*/
int A72_Read_Adderss(void)
{
	A72_u8 RX_Correct = 0;//������ȷ��־
	A72_u8 i = 0;//��ʱ����
	A72_u8 count = 0;//��ʱ����

//	memset(A72_RX_BUF,0,128);
	
	if(rt_sem_take(A72_Respond,50) == 0)//�ȴ�������� �ȴ�ʱ��50ms
	{
		for(i=0;i<20;i++)
		{
			
			if(A72_RX_BUF[i] == A72_Sbit && RX_Correct == 0)
				if(A72_RX_BUF[i+1] == 0x81 && A72_RX_BUF[i+2] == 0xC0)//��Ӧ���0x81C0
					RX_Correct = 1;//������ȷ
				
			if(RX_Correct == 1)//������յ���ȷ������
			{
				i += 6;//��������λ������λ��У��λ
				
				for(count=0;count<L_Adder_LEN;count++)//IEEE��ַ��ȡ
					_value.L_Adder[count] = A72_RX_BUF[i++];
				
				for(count=0;count<S_Adder_LEN;count++)//�̵�ַ��ȡ
					_value.S_Adder[count] = A72_RX_BUF[i++];
				
				return SUCCESS;
			}
		}
	}
				
	return ERROR;
}



/*
�������ܣ��鿴��ȡ�豸���ͻ�Ӧ
������void
����ֵ��SUCCESS:�ɹ� ERROR:ʧ��
��ע��
	ע:�ú���ΪA72_SEND_ORDER�����Ĳ��� ���ɶ��Ե���
*/
int A72_Read_Device_Type(void)
{
	A72_u8 RX_Correct = 0;//������ȷ��־
	A72_u8 i = 0;//��ʱ����

//	memset(A72_RX_BUF,0,128);
	
	if(rt_sem_take(A72_Respond,50) == 0)//�ȴ�������� �ȴ�ʱ��50ms
	{
		for(i=0;i<20;i++)
		{
			
			if(A72_RX_BUF[i] == A72_Sbit && RX_Correct == 0)
				if(A72_RX_BUF[i+1] == 0x80 && A72_RX_BUF[i+2] == 0x0E)//��Ӧ���0x800E
					RX_Correct = 1;//������ȷ
				
			if(RX_Correct == 1)//������յ���ȷ������
			{
				i += 7;//��������λ������λ��У��λ������λ
				
				switch(A72_RX_BUF[i])
				{
					case 0x00://Э����
						sprintf(_value.Type_Device,"Coordinator");
						break;
					case 0x01://·����
						sprintf(_value.Type_Device,"Router");
						break;
					case 0x02://�ն˽��
						sprintf(_value.Type_Device,"Node");
						break;
					default:
						break;
				
				}
				
				return SUCCESS;
			}
		}
	}
		
	return ERROR;
}


#if ZIGBEE_A72_NODE
/*
�������ܣ��鿴�ڵ�����״̬
������void
����ֵ��SUCCESS:�ɹ� ERROR:ʧ��
��ע�����ã��ڵ�
		ע:�ú���ΪA72_SEND_ORDER�����Ĳ��� ���ɶ��Ե���
*/
int A72_Read_Connect_status(void)
{
	A72_u8 RX_Correct = 0;//������ȷ��־
	A72_u8 i = 0;//��ʱ����

//	memset(A72_RX_BUF,0,128);
	
	if(rt_sem_take(A72_Respond,50) == 0)//�ȴ�������� �ȴ�ʱ��50ms
	{
		for(i=0;i<20;i++)
		{
			
			if(A72_RX_BUF[i] == A72_Sbit && RX_Correct == 0)
				if(A72_RX_BUF[i+1] == 0x80 && A72_RX_BUF[i+2] == 0x0D)//��Ӧ���0x800D
					RX_Correct = 1;//������ȷ
				
			if(RX_Correct == 1)//������յ���ȷ������
			{
				i += 6;//��������λ������λ��У��λ
				
				_value.S_Network = A72_RX_BUF[i];
				
				return SUCCESS;
			}
		}
	}
	
	return ERROR;
}
/*
�������ܣ��鿴�ڵ�����ʱ��
������void
����ֵ��SUCCESS:�ɹ� ERROR:ʧ��
��ע�����ã��ڵ�
		ע:�ú���ΪA72_SEND_ORDER�����Ĳ��� ���ɶ��Ե���
*/
int A72_Read_Sleep_time(void)
{
	
	A72_u8 RX_Correct = 0;//������ȷ��־
	A72_u8 i = 0;//��ʱ����

//	memset(A72_RX_BUF,0,128);
	
	if(rt_sem_take(A72_Respond,50) == 0)//�ȴ�������� �ȴ�ʱ��50ms
	{
		for(i=0;i<20;i++)
		{
			
			if(A72_RX_BUF[i] == A72_Sbit && RX_Correct == 0)
				if(A72_RX_BUF[i+1] == 0x81 && A72_RX_BUF[i+2] == 0xD1)//��Ӧ���0x81D1
					RX_Correct = 1;//������ȷ
				
			if(RX_Correct == 1)//������յ���ȷ������
			{
				i += 6;//��������λ������λ��У��λ
				
				_value.Sleep_Time = (A72_RX_BUF[i] << 8) | A72_RX_BUF[i+1];
				
				return SUCCESS;
			}
		}
	}
	
	return ERROR;

}
/*
�������ܣ��ڵ���������ʱ��
������time �� ���õ�ʱ��
����ֵ��SUCCESS:�ɹ� ERROR:ʧ��
��ע�����ã��ڵ�
*/
int A72_SET_TIME(A72_u16 time)
{
	
	A72_u8 MAX_Repeat = 5;//����ظ�����
	A72_u8 RX_Correct = 0;//������ȷ��־
	A72_u8 Set_successed = 0;
	A72_u8 i;
	
	A72_WAKE_UP_SET();//����
	
	memset(A72_send_order_buf,0,20);
	
	A72_send_order_buf[0] = A72_Sbit;
	A72_send_order_buf[1] = C_SET_time >> 8;
	A72_send_order_buf[2] = C_SET_time & 255;
	A72_send_order_buf[3] = L_SET_time >> 8;
	A72_send_order_buf[4] = L_SET_time & 255;
	A72_send_order_buf[5] = V_SET_time;
	A72_send_order_buf[6] = time >> 8;
	A72_send_order_buf[7] = time & 255;
	A72_send_order_buf[8] = A72_Ebit;
	
	//A72_Send_Data(A72_send_order_buf,SET_time_Bytes);

	
	memset(A72_RX_BUF,0,128);
	
	while(--MAX_Repeat)
	{
		
		RX_Correct = 0;
		
		A72_Send_Data(A72_send_order_buf,SET_time_Bytes);
		
		if(rt_sem_take(A72_Respond,50) == 0)//�ȴ�������� �ȴ�ʱ��50ms
		{
			for(i=0;i<20;i++)
			{
				if(A72_RX_BUF[i] == A72_Sbit && RX_Correct == 0)
					if(A72_RX_BUF[i+1] == 0x80 && A72_RX_BUF[i+2] == 0x00)//��Ӧ���0x8000
						RX_Correct = 1;//������ȷ
					
				if(RX_Correct == 1)//������յ���Ӧ������
				{
					
					i += 5;//��λ
					
					if(A72_RX_BUF[i] == 0x55 && A72_RX_BUF[i+1] == 0x01 && A72_RX_BUF[i+2] == 0xD0)
					{
						Set_successed = 1;
						break;
					}
					else
						break;
					
				}
				
			}
			
		}
		
		
		if(RX_Correct == 1 && Set_successed == 1)break;//����ѭ��
	
	}

	A72_WAKE_UP_RESET();//����
	
	if(MAX_Repeat > 0)		
		return SUCCESS;	
	else
		return ERROR;
			
}
#endif

#if ZIGBEE_A72_COORDINATOR

/*
�������ܣ��鿴���������Ӧ
������void
����ֵ��SUCCESS:�ɹ� ERROR:ʧ��
��ע�����ã�Э����
		ע:�ú���ΪA72_SEND_ORDER�����Ĳ��� ���ɶ��Ե���
*/
int A72_Read_CreateNet(void)
{
	
	A72_u8 i = 0;//��ʱ����
	A72_u8 RX_Correct = 0;//������ȷ��־
	

//	memset(A72_RX_BUF,0,128);
	
	if(rt_sem_take(A72_Respond,50) == 0)//�ȴ�������� �ȴ�ʱ��50ms
	{
		for(i=0;i<20;i++)
		{
			
			if(A72_RX_BUF[i] == A72_Sbit)
				if(A72_RX_BUF[i+1] == 0x80 && A72_RX_BUF[i+2] == 0x00)//��Ӧ���0x8000
					RX_Correct = 1;
				
			if(RX_Correct == 1)//������յ���ȷ������
			{
				i += 4;//��λ
				
				if(A72_RX_BUF[i] == 0x04 && A72_RX_BUF[i+1] == 0x85)
				{
					return SUCCESS;
				}
				else
				{
					return ERROR;
				}
				
			}
				
		}
	}
	
	return ERROR;
}
/*
�������ܣ�Э��������ڵ�����
������time ʱ�� ������С��256s��
����ֵ��SUCCESS:�ɹ� ERROR:ʧ��
��ע�����ã�Э����
*/
int A72_PermitJoin(A72_u8 time)
{
	
	A72_u8 MAX_Repeat = 5;//����ظ�����
	A72_u8 RX_Correct = 0;//������ȷ��־
	A72_u8 _success = 0;
	A72_u8 i;
//	A72_u8 k;
	
	memset(A72_send_order_buf,0,20);
	
	A72_send_order_buf[0] = A72_Sbit;
	A72_send_order_buf[1] = C_PermitJoin >> 8;
	A72_send_order_buf[2] = C_PermitJoin & 255;
	A72_send_order_buf[3] = L_PermitJoin >> 8;
	A72_send_order_buf[4] = L_PermitJoin & 255;
	A72_send_order_buf[5] = V_PermitJoin;
	A72_send_order_buf[6] = PermitJoin_Destin_adder >> 8;
	A72_send_order_buf[7] = PermitJoin_Destin_adder & 255;
	A72_send_order_buf[8] = time;
	A72_send_order_buf[9] = R_PermitJoin;
	A72_send_order_buf[10] = A72_Ebit;
	
	
	memset(A72_RX_BUF,0,128);
	
	while(--MAX_Repeat)
	{
		
		RX_Correct = 0;
		
		A72_Send_Data(A72_send_order_buf,PermitJoin_Bytes);
		
		if(rt_sem_take(A72_Respond,50) == 0)//�ȴ�������� �ȴ�ʱ��50ms
		{
			for(i=0;i<20;i++)
			{
				if(A72_RX_BUF[i] == A72_Sbit && RX_Correct == 0)
					if(A72_RX_BUF[i+1] == 0x80 && A72_RX_BUF[i+2] == 0x00)//��Ӧ���0x800D
						RX_Correct = 1;//������ȷ
					
				if(RX_Correct == 1)//������յ���Ӧ������
				{
					
					i += 5;//��λ
					
					if(A72_RX_BUF[i] == 0xB0 && A72_RX_BUF[i+1] == 0x00 && A72_RX_BUF[i+2] == 0x34)
					{
						_success = 1;
						break;
					}
					else
						break;
					
				}
				
			}
			
		}
		
		if(RX_Correct == 1 && _success == 1)break;//����ѭ��
	
	}


	
	if(MAX_Repeat > 0)
		return SUCCESS;	
	else
		return ERROR;

}


#endif

/*
�������ܣ�����ָ��
������A72_Instruction_Format *order,int(*ORDER)(void)
����ֵ��int (����0���ɹ� ����1��ʧ��)
��ע�����ã�Э�������ն˽ڵ㡢·����
		�ú������ڷ���A72�л�Ӧ��ָ��	
*/
int A72_SEND_ORDER( A72_Instruction_Format *order,int(*ORDER)(void) )
{	
	
	A72_u8 MAX_Repeat = 5;//����ظ�����
	
#if ZIGBEE_A72_NODE
	A72_WAKE_UP_SET();//����
#endif
	
	memset(A72_send_order_buf,0,20);
	
	A72_send_order_buf[0] = A72_Sbit;
	A72_send_order_buf[1] = order->command >> 8;
	A72_send_order_buf[2] = order->command & 255;
	A72_send_order_buf[3] = order->length >> 8;
	A72_send_order_buf[4] = order->length & 255;
	A72_send_order_buf[5] = order->verification;
	A72_send_order_buf[6] = A72_Ebit;
	
	
	while(--MAX_Repeat)
	{
		
		memset(A72_RX_BUF,0,128);
		
		A72_Send_Data(A72_send_order_buf,order->Order_Bytes);
		
		if(ORDER() == SUCCESS)break;
				
	}
	
//	rt_kprintf("MAX_Repeat:%d\n",MAX_Repeat);
	
#if ZIGBEE_A72_NODE
	A72_WAKE_UP_RESET();//����
#endif
	
	if(MAX_Repeat > 0)
		return SUCCESS;	//���سɹ�
	else
		return ERROR;//����ʧ��
}

/*
�������ܣ��������ݺ���
������int target:����Ŀ�� A72_SEND_MODE *mode:����ģʽ  A72_u8 *data:��������   A72_u8 len:���ݳ���
����ֵ��int   SUCCESS:�ɹ�   ERROR:ʧ��
��ע���ú���ͬ�����Է���1���ֽڵ����� ������1���ֽڵ�����ʱ �贴��һ��BUF[1] ������ 
	  target��ȡֵ�У�_COORDINATOR��0
	                 _NODE1 ��1  _NODE2 ��2  _NODE3 ��3  _B_ALL ��99  _B_SLEEP ��98  _B_ROUTER ��97
*mode��ȡֵ�У�&A72_SEND_MODE_IEEE ��IEEE��ַ����   &A72_SEND_MODE_SHORT ���̵�ַ����  &SEND_MODE_BROADCAST ���㲥����   
	  ע���������������targetֵ��С��90 ��Ĭ�Ϸ��͸�Э����
*/
int A72_SEND_DATA(int target,A72_SEND_MODE *mode,A72_u8 *data,A72_u8 len)
{
	
	A72_u8 Read_Data_OK = 0;//������ɱ�־
	A72_u8 RX_Correct = 0;//������ȷ��־
	A72_u8 i = 0;//��ʱ����
	A72_u8 MAX_Repeat = 5;//����ظ�����
	A72_u8 count_num;//��ʱ����(��¼ָ���)
	A72_u8 count_bit;//��ʱ����
	
#if ZIGBEE_A72_NODE
	A72_WAKE_UP_SET();//����
#endif
	
	memset(A72_send_data_buf,0,100);
	
	mode->L_Send_Data += len - 1;//L_Send_Data����
	
	/*****************��������ָ���ʼ��*************************/
	
	A72_send_data_buf[0] = A72_Sbit;
	A72_send_data_buf[1] = C_Send_Data >> 8;
	A72_send_data_buf[2] = C_Send_Data & 255;
	A72_send_data_buf[3] = mode->L_Send_Data >> 8;
	A72_send_data_buf[4] = mode->L_Send_Data & 255;
	A72_send_data_buf[5] = V_Send_Data;
	A72_send_data_buf[6] = mode->M_Send_Data;
	for(count_num=0,count_bit=mode->Address_Length-1;count_num<mode->Address_Length;count_num++,count_bit--)
	{
		A72_send_data_buf[count_num+7] = (mode_adderss(target,mode->Address_Length) >> (count_bit * 8)) & 255;
	}
	A72_send_data_buf[7+count_num] = SPORT_Send_Data;
	A72_send_data_buf[8+count_num] = TPORT_Send_Data;
	A72_send_data_buf[9+count_num] = len;
	for(count_num=0;count_num<len;count_num++)
	{
		A72_send_data_buf[count_num+10+mode->Address_Length] = *data;
		data++;
	}
	A72_send_data_buf[count_num+10+mode->Address_Length] = A72_Ebit;
	
	/*****************��������ָ���ʼ��*************************/
	
	count_num = 11+len+mode->Address_Length;//ָ���������
	
	mode->L_Send_Data -= len - 1;//L_Send_Data��λ
	
//Debug:
//	rt_kprintf("send order:\n");
//	for(A72_u8 count=0;count<count_num;count++)
//	{
//		rt_kprintf("%x ",A72_send_data_buf[count]);
//	}
//	rt_kprintf("\n");
	
	memset(A72_RX_BUF,0,128);
	
	while(--MAX_Repeat)
	{
		
		RX_Correct = 0;
		
		A72_Send_Data(A72_send_data_buf,count_num);
		
		if(rt_sem_take(A72_Respond,50) == 0)//�ȴ�������� �ȴ�ʱ��50ms
		{
			for(i=0;i < 20;i++)
			{
				if(A72_RX_BUF[i] == A72_Sbit && RX_Correct == 0)
					if(A72_RX_BUF[i+1] == 0x80 && A72_RX_BUF[i+2] == 0x00)//��Ӧ���0x800D
						RX_Correct = 1;//������ȷ
					
				if(RX_Correct == 1)//������յ���Ӧ������
				{
					
					i += 5;//��λ
					
					if(A72_RX_BUF[i] == 0xC0 && A72_RX_BUF[i+1] == 0x00 && A72_RX_BUF[i+2] == 0x44)
					{
						Read_Data_OK = 1;//������ȷ
						break;
					}
					else
						break;
					
				}
				
			}
			
		}
		
		
		if(RX_Correct == 1 && Read_Data_OK == 1)break;//����ѭ��
	
	}
	
//	rt_kprintf("MAX_Repeat:%d\n",MAX_Repeat);

#if ZIGBEE_A72_NODE
	A72_WAKE_UP_RESET();//����
#endif	
	
	if(MAX_Repeat > 0)
		return SUCCESS;	
	else
		return ERROR;
		
}

/*
�������ܣ�Zigbee-A72���ݴ���
�������û��Զ��� Ĭ�ϣ�void
����ֵ���û��Զ��� Ĭ�ϣ�void
��ע���ú�����Ҫ���ڴ�������Zigbee������������
	  �������ݷ�ʽΪ�û��Զ��� 
*/
void A72_HANDLE_DATA(void)
{
	
	A72_u8 len;//���ݳ��ȴ洢����
	A72_u8 count;//��ʱ����
	A72_u8 rx_data_buf[81] = {0};
	struct ZIGBEE_NODE_REPLY node_reply = {100,0,0};
	
	len = A72_RX_BUF[4] - L_Receive_Data + 1;
	
//	rt_kprintf("receive data:\n");//Debug
	for(count=0;count<len;count++)
	{
		rx_data_buf[count] = A72_RX_BUF[count+6];
//		rt_kprintf("%x ",A72_RX_BUF[count+6]);//Debug
	}
//	rt_kprintf("\n");//Debug
	
	memset(A72_RX_BUF,0,128);
	
	rt_kprintf("\nzigbee data hadle....\n\n");
	
	/******************�û�������******************/
	
	node_reply = ZIGBEE_USER_DEFINE(rx_data_buf);
	
	if(node_reply.state == ZIGBEE_SUCCESS)
	{
		if(node_reply.type_reply == Zigbee_data)
		{
			rt_kprintf("\nnode%d data:\n",node_reply.node);
			Debug_printf("PH :%.2f fpH\r\n",(float)(node_reply.node_data->_Ph) / 100.0);
			Debug_printf("HUM:%.1f %%RH\r\n",(float)(node_reply.node_data->_Hum) / 10.0);
			if(node1_data._Tem & (1 << 15))
			{
				Debug_printf("TEM:%.1f C\r\n",(float)(node_reply.node_data->_Tem - 0xFFFF) / 10.0);
			}
			else
			{
				Debug_printf("TEM:%.1f C\r\n",(float)(node_reply.node_data->_Tem) / 10.0);
			}
			Debug_printf("EC :%d us/cm\n",node_reply.node_data->_Ec);
			rt_kprintf("\n\n");
		}
		else if(node_reply.type_reply == Zigbee_check)
		{
			rt_kprintf("\nnode%d check reply\n\n",node_reply.node);
		}
	}
	

	/******************�û�������******************/
	
}





/*
�������ܣ�Zigbee-A72��ʼ��
������void
����ֵ��int (����0���ɹ� ����1��ʧ��)
��ע�����ã�Э�������ն˽ڵ㡢·����
*/
int A72_Init(void)
{
#if ZIGBEE_A72_NODE
	//����GPIO(��������)
	rcu_periph_clock_enable(A72_WAKE_UP_CLOCK);//��GPIOʱ��
	gpio_init(A72_WAKE_UP_PORT,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,A72_WAKE_UP_PIN);//GPIO����ģʽ����
#endif
	
	A72_SEND_ORDER(&_RESET,A72_NoReply);//��λ(�ϵ�ʱ���븴λ)
	
	rt_thread_mdelay(100);//��ʱ
	
//	if(A72_SEND_ORDER(&_RFS,A72_Read_RFS) == SUCCESS)//�ָ���������,,,
//		rt_kprintf("A72_Read_RFS successed..\n");
//	else
//	{
//		rt_kprintf("A72_Read_RFS failed..\n");
//		return ERROR;//ʧ�ܷ���
//	}
//	
//	rt_thread_mdelay(1000);//��ʱ
	
	
	
	if(A72_SEND_ORDER(&READ_ADDERSS,A72_Read_Adderss) == SUCCESS)//����ַ
		rt_kprintf("A72_Read_Address successed..\n");
	else
	{
		rt_kprintf("A72_Read_Address failed..\n");
		return ERROR;//ʧ�ܷ���
	}
	
	rt_thread_mdelay(100);//��ʱ
	
	if(A72_SEND_ORDER(&Device_Type,A72_Read_Device_Type) == SUCCESS)//���豸����
		rt_kprintf("A72_Read_Device_Type successed..\n");
	else
	{
		rt_kprintf("A72_Read_Device_Type failed..\n");
		return ERROR;//ʧ�ܷ���
	}
	
#if ZIGBEE_A72_NODE
	
//	rt_thread_mdelay(100);//��ʱ
//	
//	A72_SET_TIME(1);//��������1s
	
	rt_thread_mdelay(100);//��ʱ
	
	_value.Sleep_Time = 0;//��ֹ����
	
	if(A72_SEND_ORDER(&Sleep_time,A72_Read_Sleep_time) == SUCCESS)//�鿴����ʱ��
		rt_kprintf("A72_Read_Sleep_time successed..\n");
	else
		rt_kprintf("A72_Read_Sleep_time failed..\n");
	
#endif		
	
#if ZIGBEE_A72_COORDINATOR
	
	rt_thread_mdelay(100);//��ʱ
	
	if(A72_SEND_ORDER(&CreateNet,A72_Read_CreateNet) == SUCCESS)//Э������������
		rt_kprintf("A72_Read_CreateNet successed..\n");
	else
	{
		rt_kprintf("A72_Read_CreateNet failed..\n");
		return ERROR;//ʧ�ܷ���
	}
	
	rt_thread_mdelay(100);//��ʱ
	
	if(A72_PermitJoin(180) == SUCCESS)//Э������������
		rt_kprintf("A72_PermitJoin successed..\n");
	else
	{
		rt_kprintf("A72_PermitJoin failed..\n");
		return ERROR;//ʧ�ܷ���
	}
	
#endif
		
	A72_Print_Information();//��ӡ��Ϣ
	
	return SUCCESS;
}


/*
�������ܣ���ӡA72��Ӧ��Ϣ
������void
����ֵ��void
��ע����Ҫ��ӡʱ����
	  ��ӡǰӦ�ȶ�ȡA72��Ӧ����
*/
void A72_Print_Information(void)
{
	A72_u8 count;
	
	rt_kprintf("IEEE: ");
	for(count=0;count<L_Adder_LEN;count++)
		rt_kprintf("%x ",_value.L_Adder[count]);
	rt_kprintf("\nshort adderss: ");
	for(count=0;count<S_Adder_LEN;count++)
		rt_kprintf("%x ",_value.S_Adder[count]);
	rt_kprintf("\nDevice Type:%s\n",_value.Type_Device);
#if ZIGBEE_A72_NODE
	rt_kprintf("Node Sleep time:%ds\n",_value.Sleep_Time);
#endif
}




