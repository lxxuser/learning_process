#define DBG_TAG "app"
#define DBG_LVL DBG_LOG



#include "app.h"

//����1
struct rt_thread th1;
rt_uint8_t th1_stack[512] = {0};

//����2
struct rt_thread th2;
rt_uint8_t th2_stack[512] = {0};

//����3
struct rt_thread th3;
rt_uint8_t th3_stack[512] = {0};

//����4
struct rt_thread th4;
rt_uint8_t th4_stack[512] = {0};

//����5
struct rt_thread th5;
rt_uint8_t th5_stack[512] = {0};

//����6
struct rt_thread th6;
rt_uint8_t th6_stack[512] = {0};

//����7
struct rt_thread th7;
rt_uint8_t th7_stack[1024] = {0};


rt_sem_t A72_Respond;//A72��Ӧ�ź���
rt_sem_t A72_Connect;//A72�����ź���
rt_sem_t A72_Data_handle;

rt_uint8_t A72_Device_Connect = 0;//ZIGBEEӲ��״����־λ 0������ 1���쳣 

rt_err_t A72_Cheak;

rt_uint8_t Sensor_Connect = 1;//Ĭ��Ϊ1

STRUCT_ZIGBEETypeDef  zigbee_value;

//����1����--Debug
void Sensor_Debug(void *parameter)
{
	char buf[30];
	
	rt_uint8_t flag = 0;

#if RS485_CONTROL_TX_RX
	
	//RS485CON��ʼ��
	RS485_CON_Init();
	
#endif
	
	while(1)
	{
		
		if(Flag_AC_DO)
		{
			if(Sensor_TakeAValue(TYPE_PH,REG_PH) == false)
			{
				rt_kprintf("take ph failed..\n");
				flag++;
			}
				
		
			if(Sensor_TakeAValue(TYPE_HUM,REG_HUM) == false)
			{
				rt_kprintf("take hum failed..\n");
				flag++;
			}
				
			
			if(Sensor_TakeAValue(TYPE_TEM,REG_TEM) == false)
			{
				rt_kprintf("take tem failed..\n");
				flag++;
			}
				
			
			if(Sensor_TakeAValue(TYPE_EC,REG_EC) == false)
			{
				rt_kprintf("take ec failed..\n");
				flag++;
			}
			
			if(flag == 4)
				Sensor_Connect = 0;
			else 
				Sensor_Connect = 1;
			
			flag = 0;
				
			
			rt_kprintf("\n");
			
			sprintf(buf,"PH:%.2fpH",debug_value._PH);
			rt_kprintf("%s\n",buf);
			
			sprintf(buf,"HUM:%.1f%%RH",debug_value._HUM);
			rt_kprintf("%s\n",buf);
			
			sprintf(buf,"TEM:%.1fC",debug_value._TEM);
			rt_kprintf("%s\n",buf);
			
			rt_kprintf("EC:%dus/cm\n\n",debug_value._EC);
			
		}
		
		rt_thread_mdelay(5000);//5s
		
	}
	
}

//����2--ZIGBEE��ʼ��
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
			
			rt_thread_startup(&th3);
			rt_thread_startup(&th4);
			rt_thread_startup(&th5);
			rt_thread_startup(&th6);
			
			_flag = rt_thread_suspend(&th2);//��������
				
			if(_flag == RT_EOK)
				rt_kprintf("th2 rt_thread_suspend successed..\n");
			else
				rt_kprintf("th2 rt_thread_suspend failed..\n");
		}
		rt_thread_mdelay(10000);
	}
}

//����3--ZIGBEE�������״̬(����ǰ����)
void Zigbee_Network_Status_Cheak_front(void* paremeter)
{

	while(1)
    {
		if(A72_SEND_ORDER(&Connect_status,A72_Read_Connect_status) == ERROR)
		{
			
			rt_kprintf("A72_Read_Connect_status failed..\n");
			
			rt_thread_mdelay(5000);
		}
		else
		{
			
			rt_kprintf("A72_Read_Connect_status successed..\n");
			
			if(_value.S_Network == 0)//���δ���� 5����ٴγ���
			{
				rt_kprintf("A72_Connect_status: OFF\n");
				
				rt_thread_mdelay(5000);
			}
			else//�������� �������� ͬʱ�������ͺ���
			{
				rt_kprintf("A72_Connect_status: ON\n");
				rt_kprintf("A72_Connect successed..\n");
				
				rt_sem_release(A72_Connect);
				
				
				A72_Cheak = rt_thread_suspend(&th3);//��������
				
				if(A72_Cheak == RT_EOK)
					rt_kprintf("th3 rt_thread_suspend successed..\n");
				else
					rt_kprintf("th3 rt_thread_suspend failed..\n");
					
			 }
		}	
    }
}

//����4--ZIGBEE�������״̬(��������)
void Zigbee_Network_Status_Cheak_after(void *parameter)
{
	while(1)
	{
		
		if(_value.S_Network == 1)//�������
		{
			
			rt_thread_mdelay(45000);//45sһ���
			
			rt_kprintf("Connection status Cheaking...\nOnce every 45s\n");
			
			if(A72_SEND_ORDER(&Connect_status,A72_Read_Connect_status) == ERROR)
			{
				rt_kprintf("Cheak Connect failed..\n");
			}
			else
			{
				rt_kprintf("Cheak Connect successed..\n");
				
				if(_value.S_Network == 0)//���A72�Ͽ�
				{
					rt_thread_resume(&th3);//�ָ�th3 A72�����߳�
				}
				
			}
		}
		else
		{
			rt_thread_mdelay(3000);//λ����ʱ3����һ��
		}
	}
}

//����5--ZIGBEE��������
void Zigbee_Send_Data(void *parameter)
{
	
	rt_uint8_t zigbee_filed_number = 0;//zigbee�쳣����
	
	rt_uint8_t send_buf[10] = {0}; //0x8a������
	send_buf[0] = NODE_BIT;        //�������ֽ�δ�ڵ�δ
	send_buf[1] = Zigbee_data;     //������������λ

	while(1)
    {
		
		rt_sem_take(A72_Connect,RT_WAITING_FOREVER);//�����ȴ�
		
		 rt_thread_mdelay(5000);//����Ƶ��3s
		
		while(!Sensor_Connect)//����������쳣
		{
			rt_thread_mdelay(1000);//����ʱ�ȴ�
		}
		
		send_buf[2] = zigbee_value.PH >> 8;
		send_buf[3] = zigbee_value.PH & 255;
		send_buf[4] = zigbee_value.TEM >> 8;
		send_buf[5] = zigbee_value.TEM & 255;
		send_buf[6] = zigbee_value.HUM >> 8;
		send_buf[7] = zigbee_value.HUM & 255;
		send_buf[8] = zigbee_value.EC >> 8;
		send_buf[9] = zigbee_value.EC & 255;
		
		if(A72_SEND_DATA(_COORDINATOR,&A72_SEND_MODE_IEEE,send_buf,10) == SUCCESS)
		{
			rt_kprintf("A72_Send_Data successed..\n");
		}
		else
		{
			rt_kprintf("A72_Send_Data fialed..\n");
			zigbee_filed_number++;
		}

        rt_thread_mdelay(5000);//����Ƶ��3s
		
		if(zigbee_filed_number > 5)
		{
			zigbee_filed_number = 0;
			/*
			���zigbee����ʧ�ܴ�������5�� 
			����Ϊzigbee�쳣 �����³�ʼ��zigbee
			*/
			ZIGBEE_DAMAGE();
			
			rt_kprintf("\nZigbee Reinitialization\n");
		}
		
		if(_value.S_Network == 1)//���Zigbee�Ѿ�����
			rt_sem_release(A72_Connect);//����
    }
}

//����6--ZIGBEE(���ݴ���)
void Zigbee_Data_handle(void *parameter)
{
	while(1)
    {
		rt_sem_take(A72_Data_handle,RT_WAITING_FOREVER);//�����ȴ�
		
		A72_HANDLE_DATA();
		
		rt_thread_mdelay(100);
		
    }
}
//����7--���ʼ��
void Power_Metering(void *parameter)
{
	
	char buf[70];
	
	float voltage = 0;
	float current = 0;
	float power = 0;
	
	power_metering_init();//���ʼ�������
	
	while(1)
	{
		
		voltage = (float)(adc_value[0]) / 4096 * 3.3 * 2;//��ѹת��
		
		current = (float)(adc_value[1]) / 4096 * 3.3 / 6;//����ת��
		
		power = voltage * current;//���ʼ���
		
		zigbee_value.POWER = (rt_uint16_t)(power * 1000);//��16λ�޷������ݴ���Zigbee������
		
		sprintf(buf,"voltage:%.2f V\ncurrent:%.2f mA\npower  :%.2f W",voltage,current * 1000,power);
		
		rt_kprintf("\n\nPower_Metering..\n");
		
		rt_kprintf("%s\n\n",buf);//Debug
		
		rt_thread_mdelay(10000);//10sһ��
		
	}
}
