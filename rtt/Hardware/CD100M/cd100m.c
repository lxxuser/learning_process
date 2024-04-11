#include "cd100m.h"

static uint8_t g_cmd_get_id[]           =   {0x28,0x2C,0x00,0x00}; // 获取模块ID   
static uint8_t g_cmd_get_RevID[]      	=  	{0x28,0x2F,0x00,0x00}; //获取固件版本
static uint8_t g_cmd_get_debug_info[]  	=	{0x28,0x2E,0x00,0x00}; //获取调试信息
static uint8_t g_cmd_get_paras[] 		= 	{0x28,0x2D,0x00,0x00}; //获取检测参数
static uint8_t g_cmd_get_modu_sta[]		=	{0x28,0x20,0x00,0x00}; //获取状态 
static uint8_t g_cmd_get_cup_sta[]		=	{0x28,0x21,0x00,0x00}; //获取杯子状态
static uint8_t g_cmd_get_xtalk[]		=	{0x28,0x22,0x00,0x00}; //获取xtalk
static uint8_t g_cmd_hot_sta[]          = 	{0x28,0x9C,0x3C,0x00}; //启动盖板加热 
static uint8_t g_cmd_hot_stop[]         = 	{0x28,0x9B,0x00,0x00}; //关闭盖板加热

static uint8_t g_cmd_init[]             =   {0x28,0x9F,0x00,0x00}; //模块初始化 
static uint8_t g_cmd_start_app[]        = 	{0x28,0x9E,0x00,0x00}; //启动检测算法 
static uint8_t g_cmd_stop_app[]         =   {0x28,0x9D,0x00,0x00}; //停止检测算法 
static uint8_t g_cmd_set_iterat[]       =   {0x28,0x90,0x84,0x03}; //设置迭代次数 
static uint8_t g_cmd_set_para1[]        =   {0x28,0x91,0xE6,0x00}; //设置接水台距离(默认230) E6 默认是230
static uint8_t g_cmd_set_para2[]        =   {0x28,0x92,0x90,0x01}; //设置最大检测距离(默认400)
static uint8_t g_cmd_set_para3[]        =   {0x28,0x93,0x1E,0x00}; //设置满杯阈值(默认30) 
static uint8_t g_cmd_set_para4[]        =   {0x28,0x94,0x00,0x00}; //设置出水状态(默认1) 
static uint8_t g_cmd_set_para5[]        =   {0x28,0x95,0x05,0x00}; //设置有杯滤波次数(默认5) 
static uint8_t g_cmd_set_para6[]        =   {0x28,0x96,0x05,0x00}; //设置无杯滤波次数(默认5) 
static uint8_t g_cmd_set_para7[]        =   {0x28,0x97,0x05,0x00}; //设置预满滤波次数(默认5) 
static uint8_t g_cmd_set_para8[]        =   {0x28,0x98,0x08,0x00}; //设置满杯滤波次数(默认8) 
static uint8_t g_cmd_set_para9[]        =   {0x28,0x99,0xa4,0x06}; //设置液面波峰阈值(默认1500)
static uint8_t g_cmd_set_para10[]       =   {0x28,0x9A,0xE8,0x03}; //设置杯沿波峰阈值(默认1000) 
static uint8_t g_cmd_set_para11[]       =   {0x28,0x80,0x14,0x05}; //设置杯沿波峰阈值(默认1300) 

static uint8_t g_cmd_sta_auto_en[]      =   {0x28,0xCF,0x00,0x00}; //使能杯子状态自动输出 
static uint8_t g_cmd_sta_auto_dis[]     =   {0x28,0xCE,0x00,0x00}; //禁能杯子状态自动输出 
static uint8_t g_cmd_dbg_auto_en[]      =   {0x28,0xCD,0x00,0x00}; //使能调试信息自动输出 
static uint8_t g_cmd_dbg_auto_dis[]     =   {0x28,0xCC,0x00,0x00}; //禁能调试信息自动输出 
static uint8_t g_cmd_set_baud[]         =   {0x28,0xC0,0x03,0x00}; //设置串口波特率 
static uint8_t g_cmd_clr_cupsta[]       =   {0x28,0xC3,0x00,0x00}; //设置串口波特率 

static uint8_t g_sta_auto_output = 0;          /**< 杯子状态自动输出标志 */
static uint8_t g_dbg_auto_output = 0;          /**< 调试信息自动输出标志 */

result_info_t       g_CD100M_res_info;   
debuge_info_t       g_CD100M_dbg_info;
/*
	接水台各项参数，需要自行修改

*/
cup_detect_para_t   g_CD100M_paras = {
    .paras = {
        .platform_dis 		= 260,      //接水台距离 mm
		.work_max_dis       = 400,      //最大检测距离 mm
        .remain_thr      	= 30,       //设备沿杯沿距离高八位
        .water_start    	= 0,        //出水状态 0：没出水 1：出水
        .no_cup_cnt         = 5,        //有杯滤波次数
        .get_cup_cnt     	= 5,        //无杯滤波次数
        .pre_full_cnt    	= 5,        //预满滤波次数
        .full_cup_cnt    	= 8,        //满杯滤波次数
        .tof2_obj1_water    = 1500,     //T2液面信号阈值
        .tof2_obj1_edge     = 1000,     //T2沿信号阈值
        .tof1_obj1_edge     = 1300,     //T1沿信号阈值
    }
};

uint8_t g_uart_sta = 0;

static uint8_t write_cmd_wait(uint8_t *p_cmd)
{
	uint32_t cnt_10ms = 0;

	rt_enter_critical();

	uart2_send(p_cmd,4);

	rt_exit_critical();
	
	while(1)
	{
		if(g_uart_sta & 0x80)
		{
		
			if(g_CD100M_res_info.module_status & 0x0F) {
                
                return 0xFF;
            }
			
			g_uart_sta = 0;
			
			break;
			
		}
		delay_ms(10);
		
		cnt_10ms++;
		
		if (cnt_10ms > 100) {   /* 100ms超时 */
            return 0xFF;
			//printf("发送命令所在行号:%d\r\n", __LINE__);
        }
		//printf("发送命令所在行号:%d\r\n", __LINE__);
	}
	//delay_ms(30);
	//printf("发送命令所在行号:%d\r\n", __LINE__);
	return g_CD100M_res_info.module_status;
	
}
/*
		在启动设备检测算法前，一定要根据实际安装测量出接水台离设备的距离，单位是 mm，
	然后通过命令 91 将这个距离发送给设备。例如实际接水台离设备距离 230mm，转化为 16
	进制是 E6mm，
	详细描述见表 3.8。
*/
uint8_t CD100M_set_platform_dis(uint16_t platform_dis)
{
	uint8_t status = 0;
	g_cmd_set_para1[2] = (uint8_t)(platform_dis & 0x00FF);
	g_cmd_set_para1[3] = (uint8_t)(platform_dis  >> 8);
	status += write_cmd_wait(g_cmd_set_para1);
	if (status & 0x0F) {
        return status;
    }
	
	return 0;
}
/*
		设备默认最大检测距离为 400mm，表示超过 400mm 以外的目标不会被设备检测到，一
	般来说这个默认值已满足所有饮水机应用，所以该参数一般无需重新设置。若需要将该参数
	重设为 350mm，转化为 16 进制是 15Emm，示例操作如所表 3.21 所示，设备返回的参数的
	Bit2~Bit0 表示该命令的运行结果
	详细描述见表 3.8。
*/
uint8_t CD100M_set_work_max_dis(uint16_t work_max_dis)
{
	uint8_t status = 0;
	g_cmd_set_para2[2] = (uint8_t)(work_max_dis & 0x00FF);
	g_cmd_set_para2[3] = (uint8_t)(work_max_dis  >> 8);
	status +=write_cmd_wait(g_cmd_set_para2);
	if (status & 0x0F) {
        return status;
    }
	return 0;
}
/*
		设置满杯阈值
		设备默认满杯阈值为 30mm，表示设备测到液面距离杯沿小于 30mm 时，认为杯子已经
	装满水，实际设备给出满杯信号时，液面离杯沿距离不一定为 30mm，一般来说会小于 30mm，
	原因有二：一是设备本身测量杯沿距离和液面距离会有误差；二是设备在进行状态切换的时
	候有多次滤波处理，保证状态切换的稳定性，但这也会造成状态切换的延迟。若需要将该参
	数重设为 20mm，转化为 16 进制是 14mm，示例操作如表 3.22 所示，设备返回的参数的
	Bit2~Bit0 表示该命令的运行结果
*/
uint8_t CD100M_set_remain_thr (uint16_t remain_thr)
{
	uint8_t status = 0;
    g_cmd_set_para3[2] = remain_thr;
    status += write_cmd_wait(g_cmd_set_para3);
	if (status & 0x0F) {
        return status;
    }
	return 0;
}
/*
		设备默认有杯滤波次数为 5，表示连续 5 次判断满足有杯条件时，才输出“有杯”状态，
	该参数越小，进入有杯状态的响应越快但抗干扰性也会越差，参数设置需大于 1。该参数一
	般无需调整，用默认值即可。示例操作如表 3.24 所示，设备返回的参数的 Bit2~Bit0 表示该
	命令的运行结果，详细描述见表 3.8。
*/
uint8_t CD100M_set_get_cup_cnt (uint8_t sta)
{
    uint8_t status = 0;
    g_cmd_set_para5[2] = sta;
    status =write_cmd_wait(g_cmd_set_para5);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		设备默认无杯滤波次数为 5，表示连续 5 次判断满足无杯条件时，才输出“无杯”状态，
	该参数越小，进入无杯状态的响应越快但抗干扰性也会越差，参数设置需大于 1。该参数一
	般无需调整，用默认值即可。示例操作如表 3.25 所示，设备返回的参数的 Bit2~Bit0 表示该
	命令的运行结果，详细描述见表 3.8。
*/
uint8_t CD100M_set_no_cup_cnt (uint8_t sta)
{
    uint8_t status = 0;
    g_cmd_set_para6[2] = sta;
    write_cmd_wait(g_cmd_set_para6);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		设备默认预满滤波次数为 5，表示连续 5 次判断满足预满条件时，才进入预满状态，该
	状态表示杯子快要满了，不对外输出。该参数越小，进入预满状态的响应越快但抗干扰性也
	会越差，参数设置需大于 1。该参数一般无需调整，用默认值即可。示例操作如表 3.26 所示，
	设备返回的参数的 Bit2~Bit0 表示该命令的运行结果
*/
uint8_t CD100M_set_pre_full_cnt (uint8_t sta)
{
    uint8_t status = 0;
    g_cmd_set_para7[2] = sta;
    status = write_cmd_wait(g_cmd_set_para7);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		设备默认满杯滤波次数为 8，表示连续 8 次判断满足满杯条件时，才输出“满杯”状态，
	该参数越小，进入满杯状态的响应越快但抗干扰性也会越差，参数设置需大于 1。该参数一
	般无需调整，用默认值即可。示例操作如表 3.27 所示，设备返回的参数的 Bit2~Bit0 表示该
	命令的运行结果，详细描述见表 3.8。
*/
uint8_t CD100M_set_full_cup_cnt (uint8_t sta)
{
    uint8_t status = 0;
    g_cmd_set_para8[2] = sta;
	status =write_cmd_wait(g_cmd_set_para8);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		设备默认液面信号阈值为 1500，表示当传感器 T2 检测到液面信号强度高于 1500 时才
	认为液面信号有效，该参数过小会导致部分杯子接不满水，过大会导致部分杯子溢出。该参
	数一般无需调整，用默认值即可。
*/
uint8_t CD100M_set_tof2_obj1_water (uint16_t tof2_obj1_water)
{
    uint8_t status = 0;
    g_cmd_set_para9[2] = (uint8_t)(tof2_obj1_water & 0x00FF);
    g_cmd_set_para9[3] = (uint8_t)(tof2_obj1_water >> 8);
    status +=write_cmd_wait(g_cmd_set_para9);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		设备默认杯沿信号阈值为 1000，表示当传感器 T2 检测到杯沿信号强度低于 1000 才认
	为杯沿信号有效，该参数与“T1 杯沿信号阈值”配合使用限制杯子摆放位置，该参数越小对
	杯子的摆放位置要求越苛刻。该参数一般无需调整，用默认值即可。
*/
uint8_t CD100M_set_tof2_obj1_edge (uint16_t tof2_obj1_edge)
{
    uint8_t status = 0;
    g_cmd_set_para10[2] = (uint8_t)(tof2_obj1_edge & 0x00FF);
    g_cmd_set_para10[3] = (uint8_t)(tof2_obj1_edge >> 8);
    status += write_cmd_wait(g_cmd_set_para10);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		设备默认杯沿信号阈值为 1200，表示当传感器 T1 检测到杯沿信号强度高于 1200 才认
	为杯沿信号有效，该参数与“T2 杯沿信号阈值”配合使用限制杯子摆放位置，该参数越大对
	杯子的摆放位置要求越苛刻。该参数一般无需调整，用默认值即可。
*/
uint8_t CD100M_set_tof1_obj1_edge (uint16_t tof1_obj1_edge)
{
    uint8_t status = 0;
    g_cmd_set_para11[2] = (uint8_t)(tof1_obj1_edge & 0x00FF);
    g_cmd_set_para11[3] = (uint8_t)(tof1_obj1_edge >> 8);
    status += write_cmd_wait(g_cmd_set_para11);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
	初始化
*/
uint8_t CD100M_init (uint16_t iter, cup_detect_para_t *p_paras)
{
	uint8_t  status = 0;

	/* 查询CD100M初始化完成状态 */
	status = write_cmd_wait(g_cmd_init);
	if (status & 0x0F) {
        return status;
    }
	//------------------------------------------
	/* 设置迭代次数 */
	g_cmd_set_iterat[2] = (uint8_t)(iter & 0x00FF);
	g_cmd_set_iterat[3] = (uint8_t)(iter >> 8);
	status += write_cmd_wait(g_cmd_set_iterat);
	if (status & 0x0F) {
        return status;
    }
	/* 设置参数1~10 */
	g_cmd_set_para1[2] = (uint8_t)(p_paras->paras.platform_dis & 0x00FF);
	g_cmd_set_para1[3] = (uint8_t)(p_paras->paras.platform_dis >> 8);
	status += write_cmd_wait(g_cmd_set_para1);
	if (status & 0x0F) {
        return status;
    }
	
	g_cmd_set_para2[2] = (uint8_t)(p_paras->paras.work_max_dis & 0x00FF);
	g_cmd_set_para2[3] = (uint8_t)(p_paras->paras.work_max_dis >> 8);
	status += write_cmd_wait(g_cmd_set_para2);
	if (status & 0x0F) {
        return status;
    }
	
	g_cmd_set_para3[2] = p_paras->paras.remain_thr;
	status += write_cmd_wait(g_cmd_set_para3);
	if (status & 0x0F) {
        return status;
    }
	
	g_cmd_set_para4[2] = p_paras->paras.water_start;
	status += write_cmd_wait(g_cmd_set_para4);
	if (status & 0x0F) {
		return status;
	}
	g_cmd_set_para5[2] = p_paras->paras.get_cup_cnt;
	status += write_cmd_wait(g_cmd_set_para5);
	if (status & 0x0F) {
		return status;
	}
	g_cmd_set_para6[2] = p_paras->paras.no_cup_cnt;
	status += write_cmd_wait(g_cmd_set_para6);
	if (status & 0x0F) {
		return status;
	}
	g_cmd_set_para7[2] = p_paras->paras.pre_full_cnt;
	status += write_cmd_wait(g_cmd_set_para7);
	if (status & 0x0F) {
		return status;
	}
	g_cmd_set_para8[2] = p_paras->paras.full_cup_cnt;
	status += write_cmd_wait(g_cmd_set_para8);
	if (status & 0x0F) {
		return status;
	}
	g_cmd_set_para9[2] = (uint8_t)(p_paras->paras.tof2_obj1_water & 0x00FF);
	g_cmd_set_para9[3] = (uint8_t)(p_paras->paras.tof2_obj1_water >> 8);
	status += write_cmd_wait(g_cmd_set_para9);
	if (status & 0x0F) {
		return status;
	}
	g_cmd_set_para10[2] = (uint8_t)(p_paras->paras.tof2_obj1_edge & 0x00FF);
	g_cmd_set_para10[3] = (uint8_t)(p_paras->paras.tof2_obj1_edge >> 8);
	status += write_cmd_wait(g_cmd_set_para10);
	if (status & 0x0F) {
		return status;
	}
	g_cmd_set_para11[2] = (uint8_t)(p_paras->paras.tof1_obj1_edge & 0x00FF);
	g_cmd_set_para11[3] = (uint8_t)(p_paras->paras.tof1_obj1_edge >> 8);
	status += write_cmd_wait(g_cmd_set_para11);
	
	if (status & 0x0F) {
		return status;
	}
	
	//-------------------------------------------
    return 0;
}
/*
		主机发送命令 9E 启动检测算法，然后根据设备返回的参数判断设备是否成功启动检测
	算法。启动检测算法的示例操作如表 3.15 所示，设备返回的参数的 Bit2~Bit0 表示该命令的
	运行结果，
*/
uint8_t CD100M_start (void)
{
    uint8_t status = 0;
    status =write_cmd_wait(g_cmd_start_app);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		主机发送命令 9D 停止检测算法，然后根据设备返回的参数判断设备是否成功停止检测
	算法。停止检测算法的示例操作如表 3.16 所示，设备返回的参数的 Bit2~Bit0 表示该命令的
	运行结果，
*/
uint8_t CD100M_stop (void)
{
    uint8_t status = 0;
	status =write_cmd_wait(g_cmd_stop_app);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		设备盖板为导电玻璃，当在出热饮环境使用时可以通过加热盖板防止凝结水雾影响功
	能。主机发送命令 9C 启动加热，该命令带 1 个参数，表示加热目标温度，该目标温度的取
	值范围是 30℃~70℃。然后根据设备返回的参数判断设备是否成功启动加热。
	例如要设置加热电热膜到 60℃，转化为 16 进制，即为 3C℃。那就可以发送 28 9C 3C 00
	完成设置。设置启动加热的示例操作如表 3.17 所示，设备返回的参数的 Bit2~Bit0 表示该命
	令的运行结果
*/
uint8_t CD100M_hot_start (void)
{
//    uint8_t status = 0;
    write_cmd_wait(g_cmd_hot_sta);
    return 0;
}
/*
		当不需要加热盖板时主机发送命令 9B 即可停止加热，然后根据设备返回的参数判断设
	备是否成功停止加热。停止加热的示例操作如表 3.18 所示，设备返回的参数的 Bit2~Bit0 表
	示该命令的运行结果，
*/
uint8_t CD100M_hot_stop (void)
{
    uint8_t status = 0;
    status =write_cmd_wait(g_cmd_hot_stop);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		向设备发送命令帧前后，都有必要对设备的状态进行查询，主机可发送命令 20 来获取
	设备当前状态。
*/
uint8_t CD100M_get_cup_status(void)
{
	 uint8_t  status     =  0;
    //printf("当前所在行号:%d\r\n", __LINE__);
    if (g_sta_auto_output) {      /* 使能了自动输出 */
		//printf("当前所在行号:%d\r\n", __LINE__);
        if (g_uart_sta & 0x80) {
			//printf("当前所在行号:%d\r\n", __LINE__);
            g_uart_sta = 0;
        } else {
			//printf("当前所在行号:%d\r\n", __LINE__);
            return 0xFF;
        } 
    } else {
     
        /* 判断TOF结果是否更新完成 */
		//printf("当前所在行号:%d\r\n", __LINE__);
        /*status = write_cmd_wait(g_cmd_get_modu_sta);
		//printf("当前所在行号:%d\r\n", __LINE__);
        if (!(status & 0x10)) {    // 判断bit4 
			printf("设备未更新:%d\r\n", __LINE__);			
            return status;
        }
		printf("设备数据已经更新:%d\r\n", __LINE__);	
        //printf("当前所在行号:%d\r\n", __LINE__);   */   
        /* 获取测量结/果 */
        write_cmd_wait(g_cmd_get_cup_sta);
        if (status & 0x0F) {
			//printf("当前所在行号:%d\r\n", __LINE__);
            return status;
        }
    }
	//printf("当前所在行号:%d\r\n", __LINE__);
    return 0;
}
/*
		调试信息是设备在调试阶段观察的信息，这些信息对设备的安装、开发有指导性作用。
	如测试结构内壁是否对设备有影响时可在无放置任何杯子等物体情况下打印出调试信息，查
	看 T1 的距离信息是否只有一个接水台距离，若是则内壁结构无影响
	主机发送命令 2E 获取调试信息，设备返回的调试信息数据由 19 个字节构成。获取调
	试信息的示例操作如表 3.2 所示，设备返回的调试信息数据描述如表 3.3 所示。
*/
uint8_t CD100M_get_debuge_info (void) 
{
	uint8_t  status     =  0;
    
    if (g_dbg_auto_output) {     /* 使能了自动输出 */
        if (g_uart_sta & 0x80) {
            g_uart_sta = 0;
        } else {
            #if CD100M_TestDebug
                printf("__g_dbg_auto_output 0xFF\r\n");
            #endif
            return 0xFF;
        } 
    } else {
     
        /* 判断TOF结果是否更新完成 */
        status = write_cmd_wait(g_cmd_get_modu_sta);
        if ((status & 0xD0) != 0xD0) {    /* 判断bit4 */
            return status;
        }

        /* 获取调试信息 */
        status = write_cmd_wait(g_cmd_get_debug_info);
        if (status & 0x0F) {
            return status;
        }
    }
    return 0;
}	
/*
		设备在跑检测算法之前，会先接收主机设置的检测参数，同时主机也可以发送命令 2D
	获取这些参数。获取设备检测参数的示例操作如表 3.4 所示，设备返回的检测参数数据描述
	如表 3.5 所示。
*/
uint8_t CD100M_get_detect_paras (void) 
{
    uint8_t status = 0;
    
    status =write_cmd_wait(g_cmd_get_paras);
   if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		获取盖板串扰值
	串扰值指示盖板安装好坏程度，给设备装上盖板后，要保证串扰值在 200~6000 之间（迭
	代次数为 900k 时）！
	设备启动检测算法后，主机可发送命令 22 来获取盖板串扰值（注意：需保证距离设备
	40cm 内没有目标物，40~100cm 内有目标物）。获取盖板串扰值的示例操作如表 3.11 所示，
	设备返回的串扰数据描述如表 3.12 所示。
*/
uint8_t CD100M_get_xtalk (void) 
{
    uint8_t status = 0;
    status =write_cmd_wait(g_cmd_get_xtalk);  
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
		主机可通过命令 C0 设置设备 UART 通信波特率，该命令帧的参数 1 用于设置通信波特
	率，具体描述如表 3.35 所示，参数 2 为 0。设备断电后新波特率不丢失！
*/
uint8_t CD100M_set_baud (uint32_t baud)
{
	uint8_t  status   = 0;
    uint8_t  val      = 0;
    uint32_t cnt_10ms = 0;
    
    switch (baud) {
        case 2400:   val = 1; break;
        case 4800:   val = 2; break;
        case 9600:   val = 3; break;
        case 56000:  val = 4; break;
        case 115200: val = 5; break;
        default: break;
    }
    
    g_cmd_set_baud[2] = val;
     
    uart2_send(g_cmd_set_baud, 4);         /* 发送命令 */
    uart2_init(baud);                  /* 重新设置主机波特率 */
    g_uart_sta = 0;
    
    while (1) {                             /* 等待串口接收到数据 */
        if (g_uart_sta & 0x80) {
            g_uart_sta = 0;
            break;
        }
        delay_ms(10);
        cnt_10ms++;
        if (cnt_10ms > 200) {               /* 超时 */
            #if CD100M_TestDebug
            printf("tof10x_set_baud tmo!\r\n");
            #endif
            return 0xFF;
        }
    }
    
    if (g_CD100M_res_info.module_status & 0x0F) {
        return status;
    }
    return 0;
	
}
/*
		主机可通过命令 CF 使能杯子状态自动输出，当设备更新到杯子状态时会自动把数据发
	送给主机。发送杯子状态格式见 3.1.6。使能杯子状态自动输出的示例操作如表 3.31 所示。
*/
uint8_t CD100M_sta_auto_en (void)
{
	uint8_t status = 0;
    
    status = write_cmd_wait(g_cmd_sta_auto_en);
    if (status & 0x0F) {
        return status;
    }
    g_sta_auto_output = 1;    
    return 0;
}
/*
	禁能杯子状态自动输出的示例操作如表 3.32 所示
*/
uint8_t CD100M_sta_auto_dis (void)
{
    uint8_t status = 0;
    write_cmd_wait(g_cmd_sta_auto_dis);
	if (status & 0x0F) {
        return status;
    }
    g_sta_auto_output = 0; 
    return 0;
}
/*
		主机可通过命令 CD 使能调试信息自动输出，当设备更新到调试信息时会自动把数据发
	送给主机。发送的调试信息格式见 3.1.2。使能调试信息自动输出的示例操作如表 3.33 所示
*/
uint8_t CD100M_dbg_auto_en (void)
{
    uint8_t status = 0;
    status =write_cmd_wait(g_cmd_dbg_auto_en);
	if (status & 0x0F) {
        return status;
    }
    g_dbg_auto_output = 1;    
    return 0;
}
/*
	禁能调试信息自动输出的示例操作如表 3.34 所示。
*/
uint8_t CD100M_dbg_auto_dis (void)
{
    uint8_t status = 0;
    status =write_cmd_wait(g_cmd_dbg_auto_dis);
	 if (status & 0x0F) {
        return status;
    }
    g_dbg_auto_output = 0; 
    return 0;
}
/*
		设备要根据饮水机的出水状态调整检测算法，因此当饮水机要进行出水或停水的状态切
	换时，要将接下来的状态发送给设备。例如设备检测到有杯并且用户按下出水按钮时，主机
	要发送 1（表示饮水机马上要出水）给设备，注意传入参数限制为 0 或 1。
	
*/
uint8_t CD100M_set_water_status (uint8_t sta)
{
    uint8_t status = 0;
    g_cmd_set_para4[2] = sta;
    status = write_cmd_wait(g_cmd_set_para4);
	if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
	设置串口波特率
*/
uint8_t CD100M_clr_cup_status (void)
{
    uint8_t status = 0;
	status = write_cmd_wait(g_cmd_clr_cupsta);
	 if (status & 0x0F) {
        return status;
    }
    return 0;
}
/*
	g_cmd_get_RevID
		主机可发送命令 2F 来获取设备的固件版本号，获取设备固件版本的示例操作如表 3.7
	所示。设备的固件版本信息由两个字节构成，前一个字节的低 4 位代表修订号，高 4 位代表
	次版本号；后一个字节的低 4 位代表主版本号，高 4 位代表设备型号（CD100 为 0）。
	例如读到的两个字节分别为 16 进制的 01 01，则表示该设备型号为 CD100，固件版本
	为 V1.0.1。
	g_cmd_get_id
		向设备发送命令帧前后，都有必要对设备的状态进行查询，主机可发送命令 20 来获取
	设备当前状态。获取设备状态的示例操作如表 3.7 所示，设备返回的状态数据描述如表 3.8
	所示。
*/
uint8_t CD100M_get_id (void) 
{
  uint8_t status = 0;
	//printf("当前所在行号:%d\r\n", __LINE__);
    status =write_cmd_wait(g_cmd_get_RevID);
	if (status & 0x0F) {
        return status;
    }
	//printf("当前所在行号:%d\r\n", __LINE__);
    status =write_cmd_wait(g_cmd_get_id);
	if (status & 0x0F) {
        return status;
    }
	//printf("当前所在行号:%d\r\n", __LINE__);
    return 0;
}
/*
	串口解码：
		数据获取，更新数据
*/

void CD100M_uart_decode(uint8_t uart_rx,result_info_t *p_res_info,debuge_info_t *p_dbg_info, cup_detect_para_t *p_paras)
{
	
	static uint8_t frame_id       =  0;             /* 帧标识 */
    static uint8_t frame_data[19] = {0};            /* 帧数据 */
    static uint8_t data_len       =  0;             /* 帧数据长度 */
    static uint8_t data_cnt       =  0;             /* 接收帧数据计数 */
	
    if ((g_uart_sta & 0x80) == 0) {                 /* 接收未完成 */
		
        if (g_uart_sta & 0x40) {                    /* 接收到了帧头 */
			//rt_kprintf("接收所在行号:%d------%d\r\n", __LINE__,uart_rx);
            if (g_uart_sta & 0x20) {                /* 接收到了帧标识 */
				//rt_kprintf("接收所在行号:%d------%d\r\n", __LINE__,uart_rx);
                frame_data[data_cnt] = uart_rx;     /* 存储帧数据 */
                data_cnt++;                         /* 接收到的帧数据加1 */
                if (data_cnt == data_len) {         /* 接收完所有帧数据 */
					//rt_kprintf("接收所在行号:%d------%d\r\n", __LINE__,uart_rx);
                    data_cnt = 0;
                    
                    g_uart_sta |= 0x80;             /* 标记这帧数据接收完成 */
                    switch (frame_id) {             /* 解码 */
                        case 0x20:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x9F:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x9E:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x9D:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x90:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x91:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x92:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x93:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x94:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x95:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x96:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x97:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x98:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x99:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x9A:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x80:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0xCF:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0xCE:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0xCD:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0xCC:
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0xC0: 
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0xC1: 
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0xC2: 
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0xC3: 
                            p_res_info->module_status = frame_data[0];
                            break;
                        case 0x2C:
                            p_res_info->id = (frame_data[3] << 24) | frame_data[2] << 16 | frame_data[1] << 8 | frame_data[0];
                            break;
                        case 0x2F:
                            p_res_info->Revid = ((uint16_t)frame_data[1] << 8) | frame_data[0];
                            break;
                        case 0x2E:
                            for (uint8_t i = 0; i < 19; i++) {
                                p_dbg_info->info_buf[i] = frame_data[i];
                            }
                            break;
                        case 0x2D:
                            for (uint8_t i = 0; i < 16; i++) {
                                p_paras->REG_PARAS[i] = frame_data[i];
                            }
                            break;
                        case 0x21:                      
                            p_res_info->cup_status = frame_data[0];
							rt_kprintf("--------------接收所在行号:%d------%d----------%d\r\n", __LINE__,uart_rx,frame_data[0]);
                            break;
                        case 0x22:                   
                            for (uint8_t i = 0; i < 8; i++) {
                                p_res_info->xtalk[i] = ((uint16_t)frame_data[2*i+1] << 8) | frame_data[2*i];
                            }
                            break;
                    }
                }
            } else {                                /* 之前没接收到帧标识 */
				
                switch (uart_rx) {
                    case 0x20:                      /* 帧数据长度为1 */ 
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x21:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
						//rt_kprintf("接收所在行号:%d------%d\r\n", __LINE__,uart_rx);
                        break;
                    case 0x9F:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x9E:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x9D:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x9C:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x9B:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x90:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x91:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x92:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x93:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x94:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x95:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x96:
                        frame_id = uart_rx;
                        data_len = 1;
                        g_uart_sta |= 0x20;
                        break;
                    case 0x97:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x98:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x99:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x9A:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x80:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0xCF:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0xCE:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0xCD:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0xCC:
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0xC0: 
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0xC1: 
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0xC2: 
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0xC3: 
                        frame_id = uart_rx;
                        data_len = 1; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x2F:                      /* 帧数据长度为2 */
                        frame_id = uart_rx;
                        data_len = 2; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x2E:                        
                        frame_id = uart_rx;
                        data_len = 19; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x2D:
                        frame_id = uart_rx;
                        data_len = 16; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x2C:                      /* 帧数据长度为2 */
                        frame_id = uart_rx;
                        data_len = 4; 
                        g_uart_sta |= 0x20;
                        break;
                    case 0x22:
                        frame_id = uart_rx;
                        data_len = 16; 
                        g_uart_sta |= 0x20;
                        break;
                    default  :                      /* 接收错误，重新接收 */
						//rt_kprintf("接收错误，重新接收:%d------%d\r\n", __LINE__,uart_rx);
                        g_uart_sta = 0;        
                        data_cnt   = 0;
                }
            }
        } else {                                    /* 之前没接收到帧头 */
			//rt_kprintf("接收所在行号:%d------%d\r\n", __LINE__,uart_rx);
            if (uart_rx == 0x28) {                  /* 接收到0x28 */
                g_uart_sta |= 0x40;                 /* 标记接收到了帧头 */
				//rt_kprintf("接收所在行号:%d------%d\r\n", __LINE__,uart_rx);
            }
        }
    }


}

