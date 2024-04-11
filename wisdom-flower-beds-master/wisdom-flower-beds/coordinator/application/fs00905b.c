#include "fs00905b.h"

unsigned short FS_Ver = 0;//У��
unsigned char FS_RX_BUF[64];//��������
unsigned char FS_RX_FLAG = FS_RX_NO;//���ձ�־λ
unsigned short FS_RX_LEN = 0;//���ճ���

Air_Quality air_quality;


/*
�������ܣ������������������ݴ���
������Data���������� DataLen�����ݳ��� 
����ֵ��FS_SUCCESS���ɹ�
		FS_LEN_ERROR�����ȴ���
		FS_FRAME_ERROR������֡��ʽ����
		FS_CHECK_ERROR��У�����
*/
int FS_DATA_HANDLE(unsigned char *Data,unsigned short DataLen)
{
	
	if(DataLen != 40)return FS_LEN_ERROR;
	
	if((Data[0] != 0x42) || (Data[1] != 0x4d))return FS_FRAME_ERROR;
	
	unsigned short cheak = Data[38] << 8 | Data[39];//����У��ֵ
	
	FS_Ver -= Data[38] + Data[39];//��ȥ����λ
	
	if(cheak == FS_Ver && (Data[2] << 8 | Data[3]) == 36)//����λΪ�����ݳ���+У�� 2��17 + 2 = 36
	{
		
		unsigned short *AirQuality = &air_quality.PM1_0;//ȡAir_Quality�ṹ���׵�ַ
		
		for(int i = 0;i < 17;i ++)//һ��17������
		{
			*(AirQuality++) = Data[4 + (i * 2)] << 8 | Data[5 + (i * 2)];//���θ�ֵ
		}
		
	}
	else
	{
		return FS_CHECK_ERROR;
	}
	
	return FS_SUCCESS;

}
/*
�������ܣ����������������������ݽ���
���������ڵĻ�������ֵ 
����ֵ��FS_RX_NO��δ�������
		FS_RX_OK���������
��ע��������������������֡�̶�40�ֽ�
*/
int FS_UART_IRQHandler(unsigned char DR_Value)
{
	static unsigned char fs_data_len = 2;//��ʼ���ܳ�����(�����̶���ͷ)
	static unsigned short fs_ver = 0;//У���ʼΪ0
	static unsigned char fs_head = 0;//��ͷ��λ������
	static unsigned char fs_receive_start = 0;//��ʼ���ձ�־λ
	
	if(fs_receive_start == 1)//����ʼ���ձ�־λ��1ʱ����ʼ�����ݴ�������
	{
		
		FS_RX_BUF[fs_data_len++] = DR_Value;//��������
		
		fs_ver += DR_Value;//У��+
		
		if(fs_data_len == 40)//�������
		{
			FS_RX_LEN = 40;//���ȱ�־λ
			FS_RX_FLAG = FS_RX_OK;//������ɱ�־λ��1
			FS_Ver = fs_ver;//У��ֵ
			fs_ver = 0;//��־λ��λ
			fs_data_len = 2;//��־λ��λ
			fs_head = 0;//��־λ��λ
			fs_receive_start = 0;//��־λ��λ
			return FS_RX_OK;//���سɹ�
		}
		
		return FS_RX_NO;//���ؽ���Ϊ���
	}
	
	
	if(DR_Value == 0x42 && fs_receive_start == 0)//������Ϊ0x42��δ��ʼ����
	{
		fs_head = 0x42;//��ͷ����������0x42
	}	
	else if(DR_Value == 0x4d && fs_head == 0x42 && fs_receive_start == 0)//���ڶ����ֽ�Ϊ0x4d�Ұ�ͷ��λΪ0x42��Ϊ��ʼ����
	{
		fs_receive_start = 1;//��ʼ���ձ�־λ��1
		fs_head = 0;//��ͷ��λ��������0
		FS_RX_BUF[0] = 0x42;//BUF[0] �����ͷ��λ
		FS_RX_BUF[1] = 0x4d;//BUF[1] �����ͷ��λ
		fs_ver = 0x42 + 0x4d;//У��Ѱ�ͷ����
	}
	else
		fs_head = 0;//�����ͷ��������0
	
	
	
	return FS_RX_NO;//���ؽ���Ϊ���
	
}


/*
�������ܣ�����������������ӡ����
������void
����ֵ��void
��ע��ʹ�ô���0��ӡ �û��ɸ���
*/
void FS_DATA_DEBUG(void)
{
	
	Debug_printf("TSET_INTERRUPT:%d\r\n",TSET_INTERRUPT);
	Debug_printf("PM1.0    :%d ug/m3\r\n",air_quality.PM1_0);
	Debug_printf("PM2.5    :%d ug/m3\r\n",air_quality.PM2_5);
	Debug_printf("PM10     :%d ug/m3\r\n",air_quality.PM10);
	Debug_printf("PM1.0_air:%d ug/m3\r\n",air_quality.PM1_0_air);
	Debug_printf("PM2.0_air:%d ug/m3\r\n",air_quality.PM2_5_air);
	Debug_printf("PM10_air :%d ug/m3\r\n",air_quality.PM10_air);
	Debug_printf("TSP 0.3  :%d\r\n",air_quality.TSP_0_3um);
	Debug_printf("TSP 0.5  :%d\r\n",air_quality.TSP_0_5um);
	Debug_printf("TSP 1.0  :%d\r\n",air_quality.TSP_1_0um);
	Debug_printf("TSP 2.5  :%d\r\n",air_quality.TSP_2_5um);
	Debug_printf("TSP 5.0  :%d\r\n",air_quality.TSP_5_0um);
	Debug_printf("TSP 10   :%d\r\n",air_quality.TSP_10um);
	Debug_printf("TVOC     :%d ug/m3\r\n",air_quality.TVOC);
	Debug_printf("TEM      :%.1f C\r\n",(float)(air_quality.TEM - 450) / 10.0);
	Debug_printf("HUM      :%.1f %%RH\r\n",(float)(air_quality.HUM)/10.0);
	Debug_printf("CO2      :%d ppm\r\n",air_quality.CO2);
	Debug_printf("CH2O     :%d ug/m3\r\n",air_quality.CH2O);

}


