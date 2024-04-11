#include "app.h"



rt_thread_t start = NULL;
#define Test 1
#define SHIZHAN 0
void APP_Start(void *parameter)
{
	
	
	//动态信号量创建
	BC28_Respond = rt_sem_create("BC28_Respond",0,RT_IPC_FLAG_FIFO);//按照先进先出方式获取资源
	CD100M_RECV  = rt_sem_create("CD100M_RECV",0,RT_IPC_FLAG_FIFO);//按照先进先出方式获取资源
	g_mail_data =  rt_mb_create("my_mailbox", 1 , RT_IPC_FLAG_FIFO);
	//printf("当前所在行号:%d\r\n", __LINE__);
	//静态硬定时创建
	/*rt_timer_init(&bc28_10ms,"bc28_10ms",bc28_callback,NULL,10,//1s执行一次 
	RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_HARD_TIMER);//一次性 硬定时 */
	
	//任务初始化
	//rt_thread_init(&th1,"MQTT_INIT",MQTT_INIT,NULL,th1_stack,sizeof(th1_stack),th1_test_priority,5);
	//rt_thread_init(&th2,"MQTT_PUB_DATA",MQTT_PUB_DATA,NULL,th2_stack,sizeof(th2_stack),th2_test_priority,5);
	rt_thread_init(&th3,"CD100M_INFO",CD100M_INFO,NULL,th3_stack,sizeof(th3_stack),th3_test_priority,5);
	//rt_thread_init(&th4,"CD100M_SEM_POST",CD100M_CLEAR,NULL,th4_stack,sizeof(th4_stack),th4_test_priority,5);
	//printf("当前所在行号:%d\r\n", __LINE__);
	//启动任务
	rt_thread_startup(&th3);
	//rt_thread_startup(&th4);
}
int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	usart_init(115200);  	//初始化调试串口
	uart2_init(9600);		//初始化CD100M串口
	
	//uart3_init(9600);		//初始化BC28串口 没有板子 
	
	//Init_Hx711();			//称重初始化
#if 1	
	//开始任务
	start = rt_thread_create("Task Start",APP_Start,NULL,1024,20,5);
	rt_thread_startup(start); //启动任务
#endif	
#if  0
	uint8_t g_cmd[]		=	{0x28,0x21,0x00,0x00};
	uint8_t g_cmd_init[]             =   {0x28,0x9F,0x00,0x00}; //模块初始化 
	uint8_t g_cmd_start_app[]        = 	{0x28,0x9E,0x00,0x00}; //启动检测算法 
	rt_enter_critical();
		uart2_send(g_cmd_init,4);
		rt_exit_critical();
	delay_ms(1000);
	
	rt_enter_critical();
		uart2_send(g_cmd_start_app,4);
		rt_exit_critical();
	delay_ms(1000);
	while(1)
	{
		g_uart_sta=0;
		rt_enter_critical();
		uart2_send(g_cmd,4);
		rt_exit_critical();
		delay_ms(2000);
		
	}
#endif
}







