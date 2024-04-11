#ifndef __ZIGBEE_A72_H_
#define __ZIGBEE_A72_H_

#include "gd32f10x.h"
#include "stdio.h"
#include "string.h"
#include <rtthread.h>
#include "app.h"


typedef signed   char                   A72_8;      /**<  8bit integer type */
typedef signed   short                  A72_16;     /**< 16bit integer type */
typedef signed   int                    A72_32;     /**< 32bit integer type */
typedef unsigned char                   A72_u8;     /**<  8bit unsigned integer type */
typedef unsigned short                  A72_u16;    /**< 16bit unsigned integer type */
typedef unsigned int                    A72_u32;    /**< 32bit unsigned integer type */


/*
���ܣ�A72�豸ѡ��
˵������1 ���� ��0 �ر� 
      ͨ��ʹ�õ�A72���͸����µĺ궨��
	  ÿ��ֻ����1�� ��Ȼ�����
*/
#define ZIGBEE_A72_NODE 1          //�ն˽ڵ�
#define ZIGBEE_A72_COORDINATOR 0   //Э����

/*
���ܣ���ַ
˵����
��Ҫ��ӽڵ�����ʱ���������
���磺��ӽڵ�5
#define A72_NODE5_ADDERSS_IEEE                       0x1111111111111111      //�ڵ�5IEEE��ַ    
#define A72_NODE5_ADDERSS_SHORT                      0x1111                  //�ڵ�5�̵�ַ     
*/

#define A72_NODE3_ADDERSS_BROADCAST_ALL              0xFFFF                  /*!<�㲥���͸������豸      */
#define A72_NODE3_ADDERSS_BROADCAST_SLEEP            0xFFFD                  /*!<�㲥���͸�˯���豸      */
#define A72_NODE3_ADDERSS_BROADCAST_ROUTER           0xFFFC                  /*!<�㲥���͸�·�����豸    */

#define A72_COORDINATOR_ADDERSS_IEEE                 0xa4c1381707edafa9      /*!<Э����IEEE��ַ      */
#define A72_COORDINATOR_ADDERSS_SHORT                0x0000                  /*!<Э�����̵�ַ          */

#define A72_NODE1_ADDERSS_IEEE                       0xa4c138850c220ac6      /*!<�ڵ�1IEEE��ַ      */
#define A72_NODE1_ADDERSS_SHORT                      0xb522                  /*!<�ڵ�1�̵�ַ          */

#define A72_NODE2_ADDERSS_IEEE                       0xa4c138804d1dc35a      /*!<�ڵ�2IEEE��ַ      */
#define A72_NODE2_ADDERSS_SHORT                      0xb4f5                  /*!<�ڵ�2�̵�ַ          */

#define A72_NODE3_ADDERSS_IEEE                       0xa4c1382784a9d8c2      /*!<�ڵ�3IEEE��ַ      */
#define A72_NODE3_ADDERSS_SHORT                      0x6937                  /*!<�ڵ�3�̵�ַ          */

#define A72_NODE4_ADDERSS_IEEE                       0xa4c1382784a9d8c2      /*!<�ڵ�4IEEE��ַ      */
#define A72_NODE4_ADDERSS_SHORT                      0x6937                  /*!<�ڵ�4�̵�ַ          */

#define A72_NODE5_ADDERSS_IEEE                       0xa4c1382784a9d8c2      /*!<�ڵ�5IEEE��ַ      */
#define A72_NODE5_ADDERSS_SHORT                      0x6937                  /*!<�ڵ�5�̵�ַ          */

#define A72_NODE6_ADDERSS_IEEE                       0xa4c1382784a9d8c2      /*!<�ڵ�6IEEE��ַ      */
#define A72_NODE6_ADDERSS_SHORT                      0x6937                  /*!<�ڵ�6�̵�ַ          */





/*
���ܣ�����Ŀ��
˵����
��Ҫ��ӽڵ�����ʱ���������
���磺��ӽڵ�4
#define _NODE4             4      //Ŀ��:�ڵ�4
*/
#define _COORDINATOR       0      /*!<Ŀ��:Э����         */
#define _NODE1             1      /*!<Ŀ��:�ڵ�1          */
#define _NODE2             2      /*!<Ŀ��:�ڵ�2          */
#define _NODE3             3      /*!<Ŀ��:�ڵ�3          */
#define _NODE4             4      /*!<Ŀ��:�ڵ�4          */
#define _NODE5             5      /*!<Ŀ��:�ڵ�5          */
#define _NODE6             6      /*!<Ŀ��:�ڵ�6          */
#define _B_ALL            99      /*!<Ŀ��:�㲥�����豸   */
#define _B_SLEEP          98      /*!<Ŀ��:�㲥˯���豸   */
#define _B_ROUTER         97      /*!<Ŀ��:�㲥·����     */

#define s_adderss(x)         A72_NODE##x##_ADDERSS_SHORT
#define ieee_adderss(x)      A72_NODE##x##_ADDERSS_IEEE


/*
˵����
��Ҫ��ӽڵ�����ʱ���������
���磺": (x) == _NODE5 ? s_adderss(5)\"     //��ӽڵ�5
����": A72_COORDINATOR_ADDERSS_SHORT)"����һ��
*/
#define S_adderss(x)     ((x) == _NODE1 ? s_adderss(1)\
                        : (x) == _NODE2 ? s_adderss(2)\
						: (x) == _NODE3 ? s_adderss(3)\
						: (x) == _NODE4 ? s_adderss(4)\
						: (x) == _NODE5 ? s_adderss(5)\
						: (x) == _NODE6 ? s_adderss(6)\
						: A72_COORDINATOR_ADDERSS_SHORT)


/*
˵����
��Ҫ��ӽڵ�����ʱ���������
���磺": (x) == _NODE5 ? ieee_adderss(5)\"   //��ӽڵ�5
����": A72_COORDINATOR_ADDERSS_IEEE)"����һ��
*/
#define IEEE_adderss(x)  ((x) == _NODE1 ? ieee_adderss(1)\
                        : (x) == _NODE2 ? ieee_adderss(2)\
						: (x) == _NODE3 ? ieee_adderss(3)\
						: (x) == _NODE4 ? ieee_adderss(4)\
						: (x) == _NODE5 ? ieee_adderss(5)\
						: (x) == _NODE6 ? ieee_adderss(6)\
						: A72_COORDINATOR_ADDERSS_IEEE)


/*
˵����
�㲥ģʽѡ��
*/
#define BROAD_adderss(x)  ((x) == _B_ALL ? A72_NODE3_ADDERSS_BROADCAST_ALL\
                         : (x) == _NODE2 ? A72_NODE3_ADDERSS_BROADCAST_SLEEP\
						 : A72_NODE3_ADDERSS_BROADCAST_ROUTER)

/*
���ܣ�����ģʽ����
������x:Ŀ�ĵ� y:��ַ��
*/
#define mode_adderss(x,y) ((y) > 2 ? IEEE_adderss(x) : (x) < 90 ? S_adderss(x) : BROAD_adderss(x)) 




/*�������ض���*/
#define A72_Send_Data USART1_Send_Data

#if ZIGBEE_A72_NODE
/*
A72�ն˽ڵ� ����PIN:PC10
*/
#define A72_WAKE_UP_CLOCK   RCU_GPIOA
#define A72_WAKE_UP_PORT    GPIOA
#define A72_WAKE_UP_PIN     GPIO_PIN_1

#define A72_WAKE_UP_SET()     gpio_bit_reset(A72_WAKE_UP_PORT,A72_WAKE_UP_PIN);
#define A72_WAKE_UP_RESET()   gpio_bit_set(A72_WAKE_UP_PORT,A72_WAKE_UP_PIN);

/*��ѯ����״̬*/
#define Connect_status_Bytes 7 //ָ���ֽڸ���
#define C_Connect_status 0x000D //����
#define L_Connect_status 0x0000 //����
#define V_Connect_status 0x00 //У��


/*��ѯ����ʱ��*/
#define Sleep_time_Bytes 7 //ָ���ֽڸ���
#define C_Sleep_time 0x01D1 //����
#define L_Sleep_time 0x0000 //����
#define V_Sleep_time 0x00 //У��

/*��������ʱ��*/
#define SET_time_Bytes 9 //ָ���ֽڸ���
#define C_SET_time 0x01D0 //����
#define L_SET_time 0x0002 //����
#define V_SET_time 0x00 //У��

#endif


/*��ʼ����λ*/
#define A72_Sbit 0x55
#define A72_Ebit 0xAA

/*�ָ���������*/
#define RFS_Bytes 7  //ָ���ֽڸ���
#define C_RFS 0x0005//����
#define L_RFS 0x0000//����
#define V_RFS 0x00//У��

/*��λ*/
#define RESET_Bytes 7  //ָ���ֽڸ���
#define C_RESET 0x000B//����
#define L_RESET 0x0000//����
#define V_RESET 0x00//У��

/*��ȡ��ַ*/
#define Read_Adder_Bytes 7    //ָ���ֽڸ���
#define C_Read_Adder 0x01C0   //����
#define L_Read_Adder 0x0000   //����
#define V_Read_Adder 0x00    //У��
#define L_Adder_LEN 8        //IEEE��ַ����
#define S_Adder_LEN 2       //�̵�ַ����

/*�豸����*/
#define Device_Type_Bytes 7    //ָ���ֽڸ���
#define C_Device_Type 0x000E  //����
#define L_Device_Type 0x0000 //����
#define V_Device_Type 0x00   //У��

/*��������*/
#define C_Send_Data 0x0044        //����
#define L_Send_Data_SHORT 0x0007  //�̵�ַģʽ���ȳ�ʼֵ(1���ֽ�)
#define L_Send_Data_IEEE 0x000D   //IEEEģʽ���ȳ�ʼֵ(1���ֽ�)
#define L_Send_Data_BROADCAST 0X0007  //�㲥ģʽ���ȳ�ʼֵ(1���ֽ�)
#define V_Send_Data 0x00          //У��
#define M_Send_Data_SHORT 0x02    //��ַģʽ:Ŀ��̵�ַ��Ŀ��˿�
#define M_Send_Data_IEEE 0x03     //��ַģʽ:Ŀ��IEEE��ַ��Ŀ��˿�
#define M_Send_Data_BROADCAST 0x04  //��ַģʽ:�㲥
#define SPORT_Send_Data 0x01      //Դ�˿�
#define TPORT_Send_Data 0x01      //Ŀ��˿�
#define User_Send_Data_LEN(x) x   //�û����ݳ���

/*����ģʽ*/
#define A72_SEND_MODE_IEEE         SEND_MODE_IEEE  //IEEE����
#define A72_SEND_MODE_SHORT        SEND_MODE_SHORT //�̵�ַ����
#define A72_SEND_MODE_BROADCAST    SEND_MODE_BROADCAST//�㲥����

/*��������*/
#define C_Receive_Data 0xFF01 //����
#define L_Receive_Data 0x0003 //���ȳ�ʼֵ(1���ֽ�)

#if ZIGBEE_A72_COORDINATOR

/*��������*/
#define CreateNet_Bytes 7  //ָ���ֽڸ���
#define C_CreateNet 0x0001//����
#define L_CreateNet 0x0000//����
#define V_CreateNet 0x00//У��

/*��������*/
#define PermitJoin_Bytes 11  //ָ���ֽڸ���
#define C_PermitJoin 0x0034//����
#define L_PermitJoin 0x0004//����
#define V_PermitJoin 0x00//У��
#define PermitJoin_Destin_adder  0xFFFC//Ŀ���ַ
#define PermitJoin_Time(x) x  //ʱ��
#define R_PermitJoin 0x01//����


#endif

/*A72��Ӧ��Ϣ�ṹ�嶨��*/
typedef struct
{
	A72_u8 L_Adder[L_Adder_LEN];//IEEE
	A72_u8 S_Adder[S_Adder_LEN];//�̵�ַ
	char Type_Device[20];//�豸����
	A72_u8 Channel;//�ŵ�
	A72_u8 N_nodes;//�ڵ�����(Э����)
	A72_u8 S_nodes;//�ڵ�״̬(Э����)
	A72_u8 S_Network;//����״̬(�ն˽ڵ�)
	A72_u8 Sleep_Time;//����ʱ��(�ն˽ڵ�)
	
}A72_Return_Value;

/*A72ָ���ʽ*/
typedef struct
{
	A72_u16 command;//����
	A72_u16 length;//����
	A72_u8 verification;//����
	A72_u8 Order_Bytes;//���͸�ָ��ȣ�������ָ�����ݣ�
}A72_Instruction_Format;


/*A72����ģʽ*/
typedef struct
{
	A72_u16 L_Send_Data;
	A72_u8 M_Send_Data;
	A72_u8 Address_Length;
}A72_SEND_MODE;

/*��������*/
extern rt_device_t A72_dev;
extern A72_Return_Value _value;
extern A72_Instruction_Format _RESET;
extern A72_Instruction_Format _RFS;
extern A72_Instruction_Format READ_ADDERSS;
extern A72_Instruction_Format Device_Type;
extern A72_SEND_MODE SEND_MODE_IEEE;
extern A72_SEND_MODE SEND_MODE_SHORT;
extern A72_SEND_MODE SEND_MODE_BROADCAST;
extern A72_u8 A72_RX_BUF[128];
#if ZIGBEE_A72_COORDINATOR
extern A72_Instruction_Format CreateNet;
#endif
#if ZIGBEE_A72_NODE
extern A72_Instruction_Format Connect_status;
extern A72_Instruction_Format Sleep_time;
#endif

/*��������*/
int A72_Init(void);
int A72_SEND_ORDER(A72_Instruction_Format *order,int(*ORDER)(void));
int A72_NoReply(void);
int A72_Read_Adderss(void);
int A72_Read_Device_Type(void);
int A72_Read_RFS(void);
void A72_RESET(void);
void A72_Print_Information(void);
int A72_SEND_DATA(int target,A72_SEND_MODE *mode,A72_u8 *data,A72_u8 len);
void A72_HANDLE_DATA(void);
#if ZIGBEE_A72_COORDINATOR
int A72_Read_CreateNet(void);
int A72_PermitJoin(A72_u8 time);
#endif
#if ZIGBEE_A72_NODE
int A72_Read_Sleep_time(void);
int A72_Read_Connect_status(void);
int A72_SET_TIME(A72_u16 time);
#endif

#endif
