#ifndef __USERCONFIG_H
#define __USERCONFIG_H

/*
�ڵ�ѡ��
ʹ�ò�һ���Ľڵ�ʱ ����Ĵ˺궨��
�ڵ㣺_NODE1
      _NODE2
	  _NODE3
	  _NODE4
	  _NODE5
	  _NODE6
*/
#define NODE_BIT            _NODE2         

#define Zigbee_data            0x8A        /*ZIGBEE��������*/
#define Zigbee_check           0x7A        /*ZIGBEE��������*/

/*
���485ģ�鲻֧�����շ�
��Ҫ�ֶ�����CON�ڿ����շ�
�������º궨��
*/
#define RS485_CONTROL_TX_RX       0           


/*ZIGBEE�������*/
typedef struct
{
	unsigned short PH;    //PH
	unsigned short HUM;   //ʪ��
	unsigned short TEM;   //�¶�
	unsigned short EC;    //�絼��
	unsigned short POWER; //����
}STRUCT_ZIGBEETypeDef;


#endif

