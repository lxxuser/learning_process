#ifndef __FS00905B_H_
#define __FS00905B_H_

#include "gd32f10x.h"
#include "drv_usart.h"

typedef struct
{
	
	unsigned short PM1_0;//PM1.0
	unsigned short PM2_5;//PM2.5
	unsigned short PM10;//PM10
	unsigned short PM1_0_air;//PM1.0(����������)
	unsigned short PM2_5_air;//PM2.5(����������)
	unsigned short PM10_air;//PM10(����������)
	unsigned short TSP_0_3um;//0.1��������ֱ����0.3��m���ϵĿ��������
	unsigned short TSP_0_5um;//0.1��������ֱ����0.5��m���ϵĿ��������
	unsigned short TSP_1_0um;//0.1��������ֱ����1.0��m���ϵĿ��������
	unsigned short TSP_2_5um;//0.1��������ֱ����2.5��m���ϵĿ��������
	unsigned short TSP_5_0um;//0.1��������ֱ����5.0��m���ϵĿ��������
	unsigned short TSP_10um;//0.1��������ֱ����10��m���ϵĿ��������
	unsigned short TVOC;//TVOCŨ��ֵ
	unsigned short TEM;//�¶�
	unsigned short HUM;//ʪ��
	unsigned short CO2;//������̼Ũ��
	unsigned short CH2O;//��ȩŨ��
	
}Air_Quality;


/*FS���������ڲ�����*/
#define FS_UART_BAUD     9600


/*������������*/
#define FS_SUCCESS       0
#define FS_LEN_ERROR     1
#define FS_FRAME_ERROR   2
#define FS_CHECK_ERROR   3

/*���ձ�־λ*/
#define FS_RX_OK         1
#define FS_RX_NO         0



/*��������*/
int FS_DATA_HANDLE(unsigned char *Data,unsigned short DataLen);
int FS_UART_IRQHandler(unsigned char DR_Value);
void FS_DATA_DEBUG(void);

/*��������*/
extern unsigned short FS_Ver;
extern unsigned char FS_RX_BUF[64];
extern unsigned char FS_RX_FLAG;
extern unsigned short FS_RX_LEN;
extern Air_Quality air_quality;


#endif

