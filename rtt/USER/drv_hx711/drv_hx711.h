#ifndef DRV_HX711_H
#define DRV_HX711_H

#include <app.h>
#include "hx711.h"

extern rt_timer_t hx711_10ms;

void get_waterweight(void *parameter);

#endif


