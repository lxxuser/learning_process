#include "drv_hx711.h"


rt_timer_t hx711_10ms;

void get_waterweight(void *parameter)
{
	Weight_Shiwu = Get_Weight();
	
}
