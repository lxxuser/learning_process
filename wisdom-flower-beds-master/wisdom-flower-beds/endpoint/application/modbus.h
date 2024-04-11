#ifndef __MODBUS_H
#define __MODBUS_H


#include <app.h>

/*�ӻ���ַ*/
#define Slave_address 0x01

/*����2���� ��������֡���ֵ64*/
#define modbus_send_data USART2_Send_Data
#define modbus_rx_max_len  64

/*���ڲ�����  ֡���*/
#define modbus_uart_baud   9600
#define modbus_byte_interval 3.5
#define modbus_ms_interval   ((1.0/((float)modbus_uart_baud/10/1000)) * modbus_byte_interval)

/*���ձ�־λ*/
#define modbus_rx_off   0     /*�رս���*/
#define modbus_rx_on    1     /*���ڽ���*/
#define modbus_rx_ok    2     /*�������*/


typedef signed   char                   modbus_8;      /**<  8bit integer type */
typedef signed   short                  modbus_16;     /**< 16bit integer type */
typedef signed   int                    modbus_32;     /**< 32bit integer type */
typedef unsigned char                   modbus_u8;     /**<  8bit unsigned integer type */
typedef unsigned short                  modbus_u16;    /**< 16bit unsigned integer type */
typedef unsigned int                    modbus_u32;    /**< 32bit unsigned integer type */

/*modbus��Ӧ�������*/
#define modbus_reply_noreg     0
#define modbus_reply_cfailed   1
#define modbus_reply_h03       2
#define modbus_reply_h06       3
#define modbus_reply_data_err -1
#define modbus_reply_addr_err -2
#define modbus_reply_err      -3


/*��������*/
extern rt_timer_t modbus_timer;
extern rt_sem_t modbus_respond;

extern modbus_u8 modbus_rx_buf[modbus_rx_max_len];
extern modbus_u8 modbus_rx_falg;
extern modbus_u16 modbus_rxdata_len;
extern modbus_u32 modbus_time;
extern modbus_u32 modbus_time_interval;

extern modbus_u16 Modbus_DataBuff[8];

/*��������*/
void MODBUS_HostToSlave_H03(modbus_u8 slave_address,modbus_u16 start_address,modbus_u16 reg_number);
void MODBUS_HostToSlave_H06(modbus_u8 slave_address,modbus_u16 start_address,modbus_u16 data);
modbus_32 MODBUS_Handle_Reply(modbus_u8 slave_address);
int CRC_Verification(uint8_t *buff,int len);
void modbus_callback(void *parameter);

#endif

