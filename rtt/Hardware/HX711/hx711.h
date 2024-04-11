#ifndef HX711_H
#define HX711_H

#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include <rtthread.h>
#include <rtdbg.h>
#include "delay.h"
#define HX711_SCK PFout(3)// PF3
#define HX711_DOUT PFin(4)// PF4
extern long Weight_Maopi,Weight_Shiwu;

void Init_Hx711();
unsigned long HX711_Read(void);
long Get_Weight();
void Get_Maopi();

#endif
