#ifndef  ZIGBEE_A72_H
#define  ZIGBEE_A72_H

typedef signed   char                   A72_8;      /**<  8bit integer type */
typedef signed   short                  A72_16;     /**< 16bit integer type */
typedef signed   int                    A72_32;     /**< 32bit integer type */
typedef unsigned char                   A72_u8;     /**<  8bit unsigned integer type */
typedef unsigned short                  A72_u16;    /**< 16bit unsigned integer type */
typedef unsigned int                    A72_u32;    /**< 32bit unsigned integer type */

#define A72_NODE3_ADDERSS_BROADCAST_ALL              0xFFFF                  
#define A72_NODE3_ADDERSS_BROADCAST_SLEEP            0xFFFD                  
#define A72_NODE3_ADDERSS_BROADCAST_ROUTER           0xFFFC                  

#define A72_COORDINATOR_ADDERSS_IEEE                 0xa4c1381707edafa9      
#define A72_COORDINATOR_ADDERSS_SHORT                0x0000                  

#define A72_NODE1_ADDERSS_IEEE                       0xa4c138850c220ac6      
#define A72_NODE1_ADDERSS_SHORT                      0xb522                 

#define A72_NODE2_ADDERSS_IEEE                       0xa4c138804d1dc35a     
#define A72_NODE2_ADDERSS_SHORT                      0xb4f5                

#define A72_NODE3_ADDERSS_IEEE                       0xa4c1382784a9d8c2    
#define A72_NODE3_ADDERSS_SHORT                      0x6937                 

#define A72_NODE4_ADDERSS_IEEE                       0xa4c1382784a9d8c2    
#define A72_NODE4_ADDERSS_SHORT                      0x6937                 

#define A72_NODE5_ADDERSS_IEEE                       0xa4c1382784a9d8c2     
#define A72_NODE5_ADDERSS_SHORT                      0x6937                  

#define A72_NODE6_ADDERSS_IEEE                       0xa4c1382784a9d8c2     
#define A72_NODE6_ADDERSS_SHORT                      0x6937                 

#define A72_Sbit 0x55 //开始位
#define A72_Ebit 0xAA //结束位

//恢复出厂设置
#define REFOR_Bytes 7  //指令字节个数
#define REFOR_COMMAND 0x0005
#define REFOR_LENGTH  0x0000
#define REFOR_CRC     0x00

//重启模块
#define RESET_Bytes 7  //指令字节个数
#define RESET_COMMAND 0x000b
#define RESET_LENGTH  0x0000
#define RESET_CRC     0x00

//读取地址
#define Read_Adder_Bytes 7    //指令字节个数
#define READDIS_COMMAND 0x01c0
#define READDIS_LENGTH  0x0000
#define READDIS_CRC     0x00
#define L_Adder_LEN 8        //IEEE地址长度
#define S_Adder_LEN 2       //短地址长度


//程序信息
#define INFO_COMMAND 0x000e
#define INFO_LENGTH  0x0000
#define INFO_CRC     0x00

//无线信道设置
#define SETCHANNEL_COMMAND 0x0007
#define SETCHANNEL_LENGTH  0x0001
#define SETCHANNEL_NUM     0x11 //可选择11 - 26
#define SETCHANNEL_CRC     SETCHANNEL_NUM ^ SETCHANNEL_LENGTH ^ SETCHANNEL_COMMAND

//无线信道读取
#define READCHANNEL_COMMAND 0x000c
#define READCHANNEL_LENGTH  0x0000
#define READCHANNEL_CRC     0x00

//创建网络
#define CREATENET_COMMAND 0x0001
#define CREATENET_LENGTH  0x0000
#define CREATENET_CRC     0x00

//Permit Join
#define JOIN_COMMAND 0x0034
#define JOIN_LENGTH  0x0004
#define JOIN_DISTANCE 0xfffc
#define JOIN_TIME    0x00 //0x00 - 0xff
#define JOIN_SAVE    0x01
#define JOIN_CRC     JOIN_COMMAND ^  JOIN_LENGTH ^ ( JOIN_DISTANCE  << 2 | JOIN_TIME << 1 | JOIN_SAVE )

//读取节点
#define READNODE_COMMAND 0x0040
#define READNODE_LENGTH  0x0002
#define READNODE_SUOYIN  0x0000 
#define READNODE_CRC     READNODE_COMMAND ^ READNODE_LENGTH ^ READNODE_SUOYIN

//删除节点
#define REMOVENODE_COMMAND 0x0016
#define REMOVENODE_LENGTH  0x000a
#define REMOVENODE_IEEE    
#define REMOVENODE_SAVE    0x0000
#define REMOVENODE_CRC     REMOVENODE_COMMAND ^ REMOVENODE_LENGTH ^ ( REMOVENODE_IEEE << 2 |  REMOVENODE_SAVE)

//地址信息格式
#define DISTANCE_MODE  0x01 //0x01 - 0x04
#define DISTANCE_TARGETDIASTANCE 
#define S_PORT 0x01
#define E_PORT 0x01


/*设备类型*/
#define Device_Type_Bytes 7    //指令字节个数
#define C_Device_Type 0x000E  //命令
#define L_Device_Type 0x0000 //长度
#define V_Device_Type 0x00   //校验

/*发送数据*/
#define C_Send_Data 0x0044        //命令
#define L_Send_Data_SHORT 0x0007  //短地址模式长度初始值(1个字节)
#define L_Send_Data_IEEE 0x000D   //IEEE模式长度初始值(1个字节)
#define L_Send_Data_BROADCAST 0X0007  //广播模式长度初始值(1个字节)
#define V_Send_Data 0x00          //校验
#define M_Send_Data_SHORT 0x02    //地址模式:目标短地址和目标端口
#define M_Send_Data_IEEE 0x03     //地址模式:目标IEEE地址和目标端口
#define M_Send_Data_BROADCAST 0x04  //地址模式:广播
#define SPORT_Send_Data 0x01      //源端口
#define TPORT_Send_Data 0x01      //目标端口
#define User_Send_Data_LEN(x) x   //用户数据长度


/*发送模式*/
#define A72_SEND_MODE_IEEE         SEND_MODE_IEEE  //IEEE发送
#define A72_SEND_MODE_SHORT        SEND_MODE_SHORT //短地址发送
#define A72_SEND_MODE_BROADCAST    SEND_MODE_BROADCAST//广播发送


/*A72回应信息结构体定义*/
typedef struct
{
	A72_u8 L_Adder[L_Adder_LEN];//IEEE
	A72_u8 S_Adder[S_Adder_LEN];//短地址
	char Type_Device[20];//设备类型
	A72_u8 Channel;//信道
	A72_u8 N_nodes;//节点数量(协调器)
	A72_u8 S_nodes;//节点状态(协调器)
	A72_u8 S_Network;//入网状态(终端节点)
	A72_u8 Sleep_Time;//休眠时间(终端节点)
	
}A72_Return_Value;

/*A72指令格式*/
typedef struct
{
	A72_u16 command;//命令
	A72_u16 length;//长度
	A72_u8 verification;//检验
	A72_u8 Order_Bytes;//发送该指令长度（不属于指令内容）
}A72_Instruction_Format;


/*A72发送模式*/
typedef struct
{
	A72_u16 L_Send_Data;
	A72_u8 M_Send_Data;
	A72_u8 Address_Length;
}A72_SEND_MODE;

typedef struct{

}A72_RESPONSE;
#endif