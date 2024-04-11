#ifndef __CD100M_H
#define __CD100M_H

#include "stm32f4xx.h"
#include <stdio.h>
#include "sys.h"
#include "usart.h"
#include "delay.h"

#define ONE_BYTE_ALIGNED __attribute__((packed, aligned(1)))

#define ID_GET_PARAS  0x2D 	//获取检测参数
#define ID_GET_STATUS 0x20 	//获取状态
#define ID_SET_PARA1  0x91  //接水太距离
#define ID_SET_PARA2  0x92	//最大检测距离
#define ID_SET_PARA3  0x93	//满杯阈值
#define ID_SET_PARA4  0x94  //出水状态
#define ID_SET_PARA5  0x95  //有杯滤波次数
#define ID_SET_PARA6  0x96  //无杯滤波次数
#define ID_SET_PARA7  0x97  //预满滤波次数
#define ID_SET_PARA8  0x98	//满杯滤波次数
#define ID_SET_PARA9  0x99	//T2液面信号阈值
#define ID_SET_PARA10 0x9A	//T2杯沿信号阈值
#define ID_SET_PARA11 0x80  //T1杯沿信号阈值

typedef enum {
    E_OK                    = 0,
    E_UNK                   = 1,
    E_PARA                  = 2,     /* 参数错误 */
} tof_error_t;

typedef struct result_info {
    uint8_t  module_status;
    uint8_t  cup_status;
    uint16_t xtalk[8];          /**< \brief 串扰，确定盖板质量 */
    uint32_t id;                /**< \brief 模块ID */
    uint16_t Revid;             /**< \brief 模块固件版本ID */
} result_info_t;

/*
	调试信息
*/
typedef union {
	struct{
		uint8_t   cup_status; /*杯子状态 0：无杯子 1：有杯 2：满杯 3：预满*/
		uint16_t  bottom_dis; /*杯底距离*/ 
		uint16_t  level_dis;  /*液面距离*/
		uint16_t  edge_dis;	  /*杯沿距离*/
		uint16_t  T1_dis1;	  /*T1测到目标物1距离*/
		uint16_t  T1_dis2;	  /*T1测到目标物2距离*/
		uint16_t  T1_dis3;	  /*T1测到目标物3距离*/
		uint16_t  T2_dis1;	  /*T2测到目标物1距离*/
		uint16_t  T2_dis2;	  /*T2测到目标物2距离*/
		uint16_t  T2_dis3;	  /*T2测到目标物3距离*/
	}ONE_BYTE_ALIGNED  info;
	uint8_t             info_buf[19];
}debuge_info_t;
/*
	检测参数
*/
typedef union{
	struct {
		uint16_t platform_dis;		//接水台距离
		uint16_t work_max_dis;		//最大检测距离
		uint8_t	 remain_thr;		//设备沿杯沿距离高八位
		uint8_t  water_start;		//出水状态 0：没出水 1：出水
		uint8_t  no_cup_cnt;		//有杯滤波次数
		uint8_t  get_cup_cnt;		//无杯滤波次数
		uint8_t  pre_full_cnt; 		//预满滤波次数
		uint8_t  full_cup_cnt;		//满杯滤波次数
		uint16_t tof2_obj1_water;	//T2液面信号阈值
		uint16_t tof2_obj1_edge; 	//T2沿信号阈值
		uint16_t tof1_obj1_edge; 	//T1沿信号阈值
	}ONE_BYTE_ALIGNED paras;
	uint8_t 			REG_PARAS[16];
}cup_detect_para_t;

extern result_info_t       g_CD100M_res_info;   
extern debuge_info_t       g_CD100M_dbg_info;
extern cup_detect_para_t   g_CD100M_paras   ;
extern uint8_t 			   g_uart_sta;


uint8_t CD100M_set_platform_dis(uint16_t platform_dis); //设置平台距离
uint8_t CD100M_set_work_max_dis(uint16_t work_max_dis); //设置最大检测距离(默认400)
uint8_t CD100M_set_remain_thr (uint16_t remain_thr);	//设置满杯阈值
uint8_t CD100M_set_get_cup_cnt (uint8_t sta);			//设置有杯滤波
uint8_t CD100M_set_no_cup_cnt (uint8_t sta);			//设置无杯滤波
uint8_t CD100M_set_pre_full_cnt (uint8_t sta);			//设置预满滤波
uint8_t CD100M_set_full_cup_cnt (uint8_t sta);			//设置满杯滤波
uint8_t CD100M_set_tof2_obj1_water (uint16_t tof2_obj1_water);  //设置液面信号阈值
uint8_t CD100M_set_tof2_obj1_edge (uint16_t tof2_obj1_edge);    //设置T2杯沿信号阈值
uint8_t CD100M_set_tof1_obj1_edge (uint16_t tof1_obj1_edge);    //设置T1杯沿信号阈值
uint8_t CD100M_init (uint16_t iter, cup_detect_para_t *p_paras);// 初始化
uint8_t CD100M_start (void);							//启动检测算法
uint8_t CD100M_stop (void); 							//停止检测算法
uint8_t CD100M_hot_start (void);						//开启加热
uint8_t CD100M_hot_stop (void);							//停止加热
uint8_t CD100M_get_cup_status(void);					//获取设备当前状态
uint8_t CD100M_get_debuge_info (void);					//获取调试信息
uint8_t CD100M_get_detect_paras (void);					//获取设备检测参数
uint8_t CD100M_get_xtalk (void) ;						//获取盖板串扰值
uint8_t CD100M_set_baud (uint32_t baud);				//设置波特率
uint8_t CD100M_sta_auto_en (void);						//使能杯子状态自动输出
uint8_t CD100M_sta_auto_dis (void);						//禁能杯子状态自动输出
uint8_t CD100M_dbg_auto_en (void);						//使能调试信息自动输出
uint8_t CD100M_dbg_auto_dis (void);						//禁能调试信息自动输出
uint8_t CD100M_set_water_status (uint8_t sta);			//水状态调整检测算法
uint8_t CD100M_clr_cup_status (void);					//设置串口波特率
uint8_t CD100M_get_id (void) ;							//获取id
void CD100M_uart_decode(uint8_t uart_rx,result_info_t *p_res_info,debuge_info_t *p_dbg_info, cup_detect_para_t *p_paras); //数据处理


#endif
