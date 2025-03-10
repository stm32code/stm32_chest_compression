#ifndef __GIT_H
#define __GIT_H
#include "stdio.h"

typedef unsigned char U8;
typedef signed char S8;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned int U32;
typedef signed int S32;
typedef float F32;

// 服务器信息
#define SSID "NET"				 // 路由器SSID名称
#define PASS "12345678"			 // 路由器密码
#define ServerIP "183.230.40.39" // 服务器IP地址
#define ServerPort 6002			 // 服务器IP地址端口号
// 设备信息
#define PROID "591614"	   // 产品ID
#define DEVID "1068069364" // 设备ID
#define AUTH_INFO "1"	   // 鉴权信息
// MQTT主题
#define S_TOPIC_NAME "tea_topic"	// 需要订阅的主题
#define P_TOPIC_NAME "teaapp_topic" // 需要发布的主题

//  设备使用外设定义
#define OLED 1 // 是否使用OLED
#define SIZE 10

// 自定义执行结果类型
typedef enum
{
	MY_SUCCESSFUL = 0x01, // 成功
	MY_FAIL = 0x00		  // 失败

} mySta; // 成功标志位

typedef enum
{
	OPEN = 0x01, // 打开
	CLOSE = 0x00 // 关闭

} On_or_Off_TypeDef; // 成功标志位

typedef enum
{
	DERVICE_SEND = 0x00, // 设备->平台
	PLATFORM_SEND = 0x01 // 平台->设备

} Send_directino; // 发送方向

// 自定义布尔类型
typedef enum
{
	MY_TRUE,
	MY_FALSE
} myBool;

typedef struct
{
	U8 App; // 指令模式

	U8 Flage;		  // 模式选择
	U16 people;		  // 人数
	U8 light;		  // 光照
	U16 somg;		  // 烟雾浓度
	float hight;	  // 变化高度
	U16 hight_begin;  // 变化高度
	U16 hight_end;	  // 变化高度
	U16 hight_change; // 记录一次变化高度(100为触发点)

} Data_TypeDef; // 数据参数结构体

typedef struct
{

	U16 threshold_temp; // 温度
	U16 threshold_humi; // 温度

} Threshold_Value_TypeDef; // 数据参数结构体

typedef struct
{
	U8 open;	   // 门
	U8 check_open; // 监测是否由物体
	U8 ckeck_humi; // 监测是否由物体

	U16 hight_state; // 记录一次变化高度(100为触发点)

	U16 hight_num;	// 记录10S变化次数计算频率
	U16 hight_hz;	// 记录10S变化次数计算频率
	U16 hight_time; // 频率计时的时间

} Device_Satte_Typedef; // 状态参数结构体

extern Data_TypeDef Data_init;
extern Device_Satte_Typedef device_state_init;		 // 设备状态
extern Threshold_Value_TypeDef threshold_value_init; // 设备阈值设置结构体

// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data);
// 初始化
mySta Reset_Threshole_Value(Data_TypeDef *Data, Device_Satte_Typedef *device_state);
// 更新OLED显示屏中内容
mySta Update_oled_massage(void);
// 更新设备状态
mySta Update_device_massage(void);
// 解析json数据
mySta massage_parse_json(char *message);
// 验证密码并开锁
void Input_Password(void);
// 倒计时
void updata_time(void);
#endif
