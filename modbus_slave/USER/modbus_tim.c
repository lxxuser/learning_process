#include "modbus_tim.h"

void Modbus_TIME3_Init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStruct;//中断分组结构体
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//定时器初试化结构体
	//1、使能定时器时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);//使能定时器3时钟
	//2、初始化定时器
	TIM_TimeBaseInitStruct.TIM_Period=arr;//自动装载值(入口参数)
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;//预分频系数（入口参数）
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up ;//向上计数
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1 ;
	TIM_TimeBaseInit( TIM3, &TIM_TimeBaseInitStruct);//参数2是结构体类型
	
	//3、使能定时器中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//要使能定时器的更新中断
	
	//4、中断优先级分组
	NVIC_InitStruct.NVIC_IRQChannel= TIM3_IRQn ;//通道在f10x.h文件中可以找到
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	//5、使能定时器
	 TIM_Cmd(TIM3, ENABLE);//使能定时器

}

void TIM3_IRQHandler(void)   //TIM3中断
{
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
		if(modbus.timrun != 0)//运行时间！=0表明
		 {
		  modbus.timout++;
		  if(modbus.timout >= 8) //可能会出现接收未完成的可能
		  {
		     modbus.timrun = 0;
			 modbus.reflag = 1;//接收数据完毕
		  }
			
		 }
	}
}