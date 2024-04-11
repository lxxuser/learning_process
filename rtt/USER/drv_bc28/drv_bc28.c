#include "drv_bc28.h"

rt_device_t BC28_dev;
struct rt_timer bc28_10ms;

//BC28Timer回调函数
void bc28_callback(void *parameter)
{
	if(BC28_RX_FLAG == BC28_RX_ON)
	{
		BC28_RX_FLAG = BC28_RX_OFF;
		BC28_RX_DATA_LEN = 0;
		rt_sem_release(BC28_Respond);
	}
}

//给BC28发送数据
void BC28_SEND_DATA(char *fmt,...)
{
	static __align(8) unsigned char USART3_buffer[128];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf((char *)USART3_buffer,2000,fmt,ap);
	va_end(ap);
	USART3_Send_Data(USART3_buffer,strlen((const char*)USART3_buffer));
}
