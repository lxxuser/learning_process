#include "sys.h"
#include "usart.h"	
static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef 		NVIC_InitStructure;
static USART_InitTypeDef 		USART_InitStructure;




rt_mailbox_t g_mail_data;
/*
#pragma import(__use_no_semihosting_swi)
struct __FILE { int handle; /* Add whatever you need here * };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
	
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	USART_ClearFlag(USART1,USART_FLAG_TXE);
	
	return ch;
}
void _sys_exit(int return_code) {

}

*/

void usart_init(uint32_t baud)
{
	
	//打开PA硬件时钟	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	

	//打开串口1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//配置PA9和PA10为复用功能模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//第9 10根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//多功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//将PA9和PA10引脚连接到串口1的硬件
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	
	
	
	//配置串口1相关参数：波特率、无校验位、8位数据位、1个停止位......
	USART_InitStructure.USART_BaudRate = baud;										//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//允许收发数据
	USART_Init(USART1, &USART_InitStructure);
	
	
	//配置串口1的中断触发方法：接收一个字节触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	
	//配置串口1的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//使能串口1工作
	USART_Cmd(USART1, ENABLE);
}
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	uint8_t d;
	
	//检测标志位
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		//接收数据
		/*d=USART_ReceiveData(USART1);
		if((d == 0x00 | d == 0x01))
		{
			rt_kprintf("可算到我了\r\n");
			CD100M_uart_decode(d, &g_CD100M_res_info, &g_CD100M_dbg_info, &g_CD100M_paras);	
		}
		//清空标志位*/
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}


}

void uart2_init(uint32_t baud)
{
	
	//打开PA硬件时钟	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	

	//打开串口1硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	//配置PA9和PA10为复用功能模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;		//第9 10根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//多功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//将PA9和PA10引脚连接到串口1的硬件
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);	
	
	
	
	//配置串口1相关参数：波特率、无校验位、8位数据位、1个停止位......
	USART_InitStructure.USART_BaudRate = baud;										//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//允许收发数据
	USART_Init(USART2, &USART_InitStructure);
	
	
	//配置串口1的中断触发方法：接收一个字节触发中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	
	//配置串口1的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//使能串口1工作
	USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void)                	//串口1中断服务程序
{	
	uint8_t d;
	
	// 检测标志位
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 清空标志位
		d = USART_ReceiveData(USART2); // 接收数据
		g_recv_da[g_cnt ++] = d;
		
		USART_SendData(USART1,d);
		if((d == 0x00 | d == 0x01) && g_cnt == 3)
		{
			rt_kprintf("可算到我了\r\n");
			for(uint16_t i=0;i < 3;i++)
			{
				CD100M_uart_decode(g_recv_da[i], &g_CD100M_res_info, &g_CD100M_dbg_info, &g_CD100M_paras);	
			}
		}
		// 处理接收到的数据
		
		
	}
}

void  uart2_send(uint8_t* msg,uint16_t len)
{
	uint16_t i;
	for(i=0;i<len;i++)
	{
		while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TXE));
        USART_SendData(USART2, msg[i]);
        while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TC));
		
	}
	// 添加延时等待发送完成
    delay_ms(10);
}

void uart3_init(uint32_t baud)
{
	
	//打开PA硬件时钟	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	

	//打开串口1硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	//配置PA9和PA10为复用功能模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;		//第9 10根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//多功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOB,&GPIO_InitStructure);

    //将PB10和PB11引脚连接到串口1的硬件
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);	
	
	//配置串口3相关参数：波特率、无校验位、8位数据位、1个停止位......
	USART_InitStructure.USART_BaudRate = baud;										//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//允许收发数据
	USART_Init(USART3, &USART_InitStructure);
	
	
	//配置串口3的中断触发方法：接收一个字节触发中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	
	//配置串口3的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//使能串口3工作
	USART_Cmd(USART3, ENABLE);
}

void USART3_IRQHandler(void)                	//串口1中断服务程序
{	
	uint8_t res;
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);//清标志位
		res = USART_ReceiveData(USART3);//读取数据

		if(BC28_RX_FLAG == BC28_RX_OFF)
		{
			rt_timer_start(&bc28_10ms);
		}
		BC28_RX_FLAG = BC28_RX_ON;
		BC28_RX_BUF[BC28_RX_DATA_LEN++] = res;
	}
}

void USART3_Send_Data(uint8_t *data,uint16_t len)
{
	uint16_t i;
	for(i=0;i<len;i++)
	{
		while(RESET == USART_GetFlagStatus(USART3, USART_FLAG_TXE));
		USART_SendData(USART3, data[i]);
	}
}

