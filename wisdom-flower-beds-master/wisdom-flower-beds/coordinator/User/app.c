#define DBG_TAG "app"
#define DBG_LVL DBG_LOG

#include "app.h"

//����1
struct rt_thread th1;
rt_uint8_t th1_stack[1024] = {0};

//����2
struct rt_thread th2;
rt_uint8_t th2_stack[2048] = {0};

//����3
struct rt_thread th3;
rt_uint8_t th3_stack[1024] = {0};

//����4
struct rt_thread th4;
rt_uint8_t th4_stack[1024] = {0};

//����5
struct rt_thread th5;
rt_uint8_t th5_stack[1024] = {0};

//����6
struct rt_thread th6;
rt_uint8_t th6_stack[1024] = {0};

//����7
struct rt_thread th7;
rt_uint8_t th7_stack[1024] = {0};


rt_sem_t A72_Respond;//A72��Ӧ�ź���
rt_sem_t A72_Connect;//A72�����ź���
rt_sem_t A72_Data_handle;
//rt_mutex_t A72_Data_handle;

rt_sem_t FS_Respond;

rt_sem_t BC28_Respond;

rt_sem_t WaterValve;

rt_uint8_t A72_Device_Connect = 0;//ZIGBEEӲ��״����־λ 0������ 1���쳣 

rt_err_t A72_Cheak;

rt_uint8_t BC28_Connect = 0;

rt_uint8_t MQTT_Connect = 0;

int WaterValveLen = 0;

rt_uint8_t WaterValve_RX = 0;

rt_uint8_t STATE_NODE_BUF[6] = {0};



//����1--ZIGBEE��ʼ��
void WLAN_INIT(void *parameter)
{
	rt_err_t _flag;
	
	while(1)
	{
		if(A72_Init() == ERROR)
		rt_kprintf("A72_Init create failed..\n");
		else
		{
			rt_kprintf("A72_Init create successed..\n");
			
			A72_Device_Connect = 1;//�豸���ӳɹ���־λ��1
			
			rt_thread_startup(&th2);
			
			rt_thread_startup(&th3);
			
			_flag = rt_thread_suspend(&th1);//��������
				
			if(_flag == RT_EOK)
				rt_kprintf("th2 rt_thread_suspend successed..\n");
			else
				rt_kprintf("th2 rt_thread_suspend failed..\n");
		}
		rt_thread_mdelay(3000);
	}
}


//����2--ZIGBEE(���ݴ���)
void Zigbee_Data_handle(void *parameter)
{
	while(1)
    {
		rt_sem_take(A72_Data_handle,RT_WAITING_FOREVER);//�����ȴ�
		
		rt_enter_critical();
		
		A72_HANDLE_DATA();
		
		rt_exit_critical();
		
		rt_thread_mdelay(15);
		
    }
}

//����3--ZIGBEE(������)
void Zigbee_Heartbeat(void *parameter)
{
	rt_uint8_t checkbuf[2] = {0x00,0x7A};//0x7AΪ����־
	
	rt_uint8_t MAX_Repeat;//����ظ�����־λ
	
	rt_uint8_t first_check;
	
	rt_uint8_t count;
	
	rt_uint8_t node_state_number;
	
	rt_uint8_t *node;
	
	
	while(1)//��ĳ���ڵ�δ��Ӧ������ ���ٴη���
	{
		
		MAX_Repeat = 2;
		
		first_check = 1;
		
		rt_thread_mdelay(30000);//30s���
		
		rt_kprintf("ZIGBEE Checking...\n");
		
		while(MAX_Repeat -- )
		{
			
			if(first_check)//��һ�η������������㲥����
			{
				
				rt_memset(STATE_NODE_BUF,0,6);
				
				rt_uint8_t max_repeat = 2;
				
				while(max_repeat --)
				{
					if(A72_SEND_DATA(_B_ALL,&A72_SEND_MODE_BROADCAST,checkbuf,2) == SUCCESS)
					{
						break;
					}
				}
				
				if(max_repeat == 0xFF)//Э����ZIGBEE�쳣
				{
					
					ZIGBEE_Disconnect();
					
					rt_kprintf("ZIGBEE Check failed..\n");
					
					break;
				}
	
			}
			
			rt_thread_mdelay(5000);//�ȴ������5s
			
			node_state_number = 0;
			
			for(count = 0;count < 6;count ++)
			{
				
				if(first_check == 1 && STATE_NODE_BUF[count] == 0)//��δ��Ӧ �򵥶�����
				{
					
					A72_SEND_DATA(count+1,&A72_SEND_MODE_IEEE,checkbuf,2);
					
					rt_thread_mdelay(1);
				}
				else
				{
					node_state_number += STATE_NODE_BUF[count];
				}
					
			}
			
			if(node_state_number == 6)break;//���ȫ������ �����˳�ѭ��
			
			first_check = 0;
		}
		
		
		if(A72_Device_Connect)
		{
			node = &STATE_NODE.NODE1;//ȡNODE1�ĵ�ַ
			
			rt_kprintf("ZIGBEE inspection completed\n");
		
			for(count = 0;count < 6;count ++)
			{
				*(node++) = STATE_NODE_BUF[count];
				if(!STATE_NODE_BUF[count])
					rt_kprintf("NODE%d is abnormal\n",count+1);
				else
					rt_kprintf("NODE%d is normal\n",count+1);
			}
		}
				
	}

}
//����4--��ȡ��������
void Air_quality_takeValue(void *parameter)
{
	
	int result;
	
	
	while(1)
	{
		
		rt_thread_mdelay(6000);//�ɼ�Ƶ�� 6s
		
		usart_enable(UART3);//�򿪴���
		
		rt_sem_take(FS_Respond,RT_WAITING_FOREVER);//���õȴ�
		
		usart_disable(UART3);//�رմ���
			
		result = FS_DATA_HANDLE(FS_RX_BUF,FS_RX_LEN);
		
		rt_kprintf("\n");
		
		if(result == FS_SUCCESS)
		{
			
			rt_kprintf("Air quality take successed..\n\n");
			
			FS_DATA_DEBUG();//��ӡ����
			
			rt_kprintf("\n\n");
		}
		else if(result == FS_LEN_ERROR)
		{
			rt_kprintf("Air quality take failed..\n");
			rt_kprintf("reason: data len error\n");
		}
		else if(result == FS_FRAME_ERROR)
		{
			rt_kprintf("Air quality take failed..\n");
			rt_kprintf("reason: Frame format error\n");
		}
		else if(result == FS_CHECK_ERROR)
		{
			rt_kprintf("Air quality take failed..\n");
			rt_kprintf("reason: Verification error\n");
		}
		
		rt_memset(FS_RX_BUF,0,64);
				
	}
}
//����5--MQTT��ʼ��
void MQTT_INIT(void *parameter)
{
	
	rt_err_t _flag;
	
	while(1)
	{
		if(BC28_Connect == 0 && BC28_Init() != SUCCESS )
		{
			rt_kprintf("\nBC28 initialization failed!\nTry again after 10 s...\n\n");
		}
		else
		{
			BC28_Connect = 1;
		}
		
		if(BC28_Connect == 1 && MQTT_Connect == 0 && MQTT_Init() != SUCCESS)
		{
			rt_kprintf("\nMQTT initialization failed!\nTry again after 10 s...\n\n");
		}
		else
		{
			MQTT_Connect = 1;
		}
		
		if(BC28_Connect == 1 && MQTT_Connect == 1)
		{
			rt_kprintf("\nMQTTinitialization successed!\n");
			
			rt_thread_startup(&th6);//��������6
			
			
			
			_flag = rt_thread_suspend(&th5);//��������
				
			if(_flag == RT_EOK)
				rt_kprintf("th5 rt_thread_suspend successed..\n");
			else
				rt_kprintf("th5 rt_thread_suspend failed..\n");
			
		}
		else
		{
			rt_thread_mdelay(10000);
		}
		
	}
	
}
//����6--�ϴ�������������
void MQTT_PUB_DATA(void *parameter)
{
//	char buf[50];
	int len = 0;
	
	while(1)
	{
		
		rt_thread_mdelay(7000);
		
		JSON_Upper();
		
		len = strlen(Json_Buf);
		
		rt_kprintf("\nsend len:%d\n\n",len);
		
		
		if(MQTT_Connect == 1)
		{
			if(MQTT_PUB(Json_Buf) == SUCCESS)
				rt_kprintf("Data upload to Aliyun successed!\n\n");
			else
				rt_kprintf("Data upload to Aliyun failed!\n\n");

		}
		
		
	}
}




void WaterValveControl(void *parameter)
{
	
	SHUIfa_init();
	
	rt_uint8_t buf[200] = {0};
	
	while(1)
	{
		rt_sem_take(WaterValve,RT_WAITING_FOREVER);//�����ȴ�
		
		rt_memcpy(buf,BC28_RX_BUF,WaterValveLen);
		
		rt_kprintf("%s\n",buf);
		
		rt_thread_mdelay(3000);
	}

}


