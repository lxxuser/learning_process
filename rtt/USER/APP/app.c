#include "app.h"

rt_sem_t BC28_Respond;
rt_sem_t CD100M_RECV;
/*任务1 MQTT_init*/
struct rt_thread th1;
rt_uint8_t th1_stack[1024] = {0};

//任务2
struct rt_thread th2;
rt_uint8_t th2_stack[1024] = {0};

//任务3
struct rt_thread th3;
rt_uint8_t th3_stack[1024] = {0};

//任务4
struct rt_thread th4;
rt_uint8_t th4_stack[1024] = {0};

rt_uint8_t BC28_Connect = 0;
rt_uint8_t MQTT_Connect = 0;

uint8_t g_cmd1[]		=	{0x28,0x9F,0x00,0x00};
uint8_t g_cmd2[]		=	{0x28,0x9E,0x00,0x00};
uint8_t g_cmd3[]		=	{0x28,0x21,0x00,0x00};
void MQTT_INIT(void *parameter)
{
	rt_err_t _flag;
	
	while(1)
	{
		if(BC28_Connect == 0 && BC28_Init() != SUCCESS)
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
			
			rt_thread_startup(&th2);//启动任务2
			
			_flag = rt_thread_suspend(&th1);//挂起自身
				
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

//任务2--上传数据至阿里云
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

void CD100M_INFO(void *parameter)
{
#if 0
	 //printf("当前所在行号:%d\r\n", __LINE__);
	 uint8_t result  = 0;
	 //CD100M_get_id();
	 /* 获取模块ID */
	 printf("CD10%d V%d.%d.%d \r\n", 	  (g_CD100M_res_info.Revid >> 12) & 0x0F,
                                          (g_CD100M_res_info.Revid >> 8)  & 0x0F,
                                          (g_CD100M_res_info.Revid >> 4)  & 0x0F,
                                          (g_CD100M_res_info.Revid >> 0)  & 0x0F);
	//printf("当前所在行号:%d\r\n", __LINE__);
	/* 获取检测参数 */
    result = CD100M_get_detect_paras();
	if (result != 0) {
        #if CD100M_TestDebug
        printf("CD100_get_detect_paras err! %x \r\n", result);
        #endif
    }
	//printf("当前所在行号:%d\r\n", __LINE__);
	
	printf("检测参数：%d %d %d %d %d %d %d %d %d %d %d\r\n", 
                g_CD100M_paras.paras.platform_dis,
                g_CD100M_paras.paras.work_max_dis,
                g_CD100M_paras.paras.remain_thr,
                g_CD100M_paras.paras.water_start,
                g_CD100M_paras.paras.no_cup_cnt,
                g_CD100M_paras.paras.get_cup_cnt,
                g_CD100M_paras.paras.pre_full_cnt,
                g_CD100M_paras.paras.full_cup_cnt,
                g_CD100M_paras.paras.tof2_obj1_water,
                g_CD100M_paras.paras.tof2_obj1_edge,
                g_CD100M_paras.paras.tof1_obj1_edge);
	CD100M_init(900,&g_CD100M_paras);  //CD100M 
	if (result != 0) {
        #if CD100M_TestDebug
        printf("cd100_init err! %x \r\n", result);
        #endif
    }
    result = CD100M_start();        
    if (result != 0) {
        #if CD100M_TestDebug
        printf("CD100_start err! %x \r\n", result);
        #endif
    }
	//CD100M_sta_auto_en(); //使能自动输出
	while (1) {
		
		//printf("当前所在行号:%d\r\n", __LINE__);
		//CD100M_get_cup_status();
		//printf("当前所在行号:%d\r\n", __LINE__);
        printf("g_CD100M_dbg_info 水杯状态 ：%d \r\n",g_CD100M_dbg_info.info.cup_status);
		//printf("当前所在行号:%d\r\n", __LINE__);
		delay_ms(2000);
		//printf("当前所在行号:%d\r\n", __LINE__);
    }
#endif
#if 1
	uint8_t result  = 0;
	uart2_send(g_cmd1,4);
	delay_ms(1000);
	//CD100M_dbg_auto_en();
	rt_enter_critical();
	uart2_send(g_cmd2,4);
	rt_exit_critical();
	delay_ms(1000);
	while (1) {
		memset(g_recv_da,0,sizeof(g_recv_da));
		g_cnt = 0;
		g_uart_sta = 0; //重新赋值		
		rt_enter_critical();
		uart2_send(g_cmd3,4);
		rt_exit_critical();
		

		rt_kprintf("当前杯子状态 : %d\r\n", g_CD100M_res_info.cup_status);
		
		delay_ms(2000);
	}
#endif
}


void CD100M_CLEAR(void *parameter)
{
	while(1)
	{
		rt_uint32_t message;
		rt_mb_recv(g_mail_data, &message, RT_WAITING_FOREVER);
		rt_kprintf("------------%d------------\r\n",message);
		rt_enter_critical();
		CD100M_uart_decode((uint8_t)message, &g_CD100M_res_info, &g_CD100M_dbg_info, &g_CD100M_paras);	
		rt_exit_critical();
		
	}
}