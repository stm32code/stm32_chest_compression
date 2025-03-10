#include "git.h"
#include "onenet.h"
#if OLED // OLED文件存在
#include "oled.h"
#endif

#include "adc.h"
#include "led.h"
#include "relay.h"
#include "key.h"
#include "usart2.h"
// mup6050
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
//  C库
#include "cjson.h"
#include <string.h>
#include <stdio.h>
#include "math.h"

Data_TypeDef Data_init;						  // 设备数据结构体
Threshold_Value_TypeDef threshold_value_init; // 设备阈值设置结构体
Device_Satte_Typedef device_state_init;		  // 设备状态
MPU6050_Typedef mup6050_init;				  // 设备状态

F32 nums[SIZE];

float ax, ay, az;				   // 原始加速度值，单位为 g
float sensitivity = 2.0 / 32768.0; // 加速度计的灵敏度，单位为 g/LSB
float ax_mps2, ay_mps2, az_mps2;   // 转换后的加速度值，单位为 m/s^2
float v0 = 0;					   // 初始速度，单位为 m/s
float depth0 = -150;			   // 初始深度，单位为 cm
float dt = 0.05;				   // 时间间隔，单位为 s
float depth, depth1;
// MPU6050初始化代码省略

void MPU6050_Read_Accel(void)
{

	// 转换为加速度值，单位为 m/s^2
	ax_mps2 = mup6050_init.aacx / sensitivity * 9.8;
	ay_mps2 = mup6050_init.aacy / sensitivity * 9.8;
	az_mps2 = mup6050_init.aacz / sensitivity * 9.8;

	// 计算物体当前的速度
	float vx = ax_mps2 * dt;
	float vy = ay_mps2 * dt;
	float vz = az_mps2 * dt;

	// 计算物体当前的平均速度
	float v_avg = (vz + v0) / 2;

	// 计算物体当前在z轴方向上的深度变化
	float delta_z = v_avg * dt;

	// 计算物体当前的深度
	depth = depth0 + (delta_z / 3000) * 3;

	if (depth > 250)
	{
		Data_init.hight = 250;
	}
	else if (depth <= 250)
	{

		Data_init.hight = depth - depth1;
	}
}

// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data)
{

	if (mpu_dmp_get_data(&mup6050_init.pitch, &mup6050_init.roll, &mup6050_init.yaw) == 0)
	{
		// 得到温度值
		mup6050_init.temp = MPU_Get_Temperature();
		// 得到加速度传感器数据
		MPU_Get_Accelerometer(&mup6050_init.aacx, &mup6050_init.aacy, &mup6050_init.aacz);
		// 得到陀螺仪数据
		MPU_Get_Gyroscope(&mup6050_init.gyrox, &mup6050_init.gyroy, &mup6050_init.gyroz);
		MPU6050_Read_Accel();
	}

	return MY_SUCCESSFUL;
}

// 初始化
mySta Reset_Threshole_Value(Data_TypeDef *Data, Device_Satte_Typedef *device_state)
{
	// 阈值定义

	// 状态重置
	device_state->open = 0;
	return MY_SUCCESSFUL;
}
// 更新OLED显示屏中内容
mySta Update_oled_massage()
{
#if OLED // 是否打开
	char str[50];
	u8 i;
	// 更新OELD显示屏数据

	sprintf(str, "time : %02d ", device_state_init.hight_time);
	OLED_ShowString(0, 0, (unsigned char *)str, 16);

	sprintf(str, "num : %03d ", device_state_init.hight_num);
	OLED_ShowString(0, 16, (unsigned char *)str, 16);

	sprintf(str, "HZ : %03d HZ", device_state_init.hight_hz);
	OLED_ShowString(0, 32, (unsigned char *)str, 16);

	sprintf(str, "hight : %03d mm", (U16)Data_init.hight);
	OLED_ShowString(0, 48, (unsigned char *)str, 16);

	OLED_Refresh(); // 开启显示
#endif

	return MY_SUCCESSFUL;
}
// 倒计时
void updata_time(void)
{
	if (device_state_init.hight_time > 0)
	{
		device_state_init.hight_time--;
	}
	else if (device_state_init.hight_time == 0)
	{
		device_state_init.hight_time = 10;
		// 计算频率
		device_state_init.hight_hz = device_state_init.hight_num / device_state_init.hight_time;
		// 清空计数
		device_state_init.hight_num = 0;
		// 发送给APP
		OneNet_SendMqtt(1); // 上传数据到平台
	}
}
//  更新设备状态
mySta Update_device_massage()
{
	// 比较（变化值为120）放置异常值
	if (150 < Data_init.hight && Data_init.hight < 250 && device_state_init.hight_state == 1)
	{
		Data_init.hight_end = Data_init.hight;
		OneNet_SendMqtt(1); // 上传数据到平台
		device_state_init.hight_state = 0;
		// 次数加一
		device_state_init.hight_num++;
	}
	else if (Data_init.hight < 25 && device_state_init.hight_state == 0)
	{
		// 赋值高低(改变一次)
		Data_init.hight_begin = Data_init.hight;
		OneNet_SendMqtt(1); // 上传数据到平台
		device_state_init.hight_state = 1;
	}

	return MY_SUCCESSFUL;
}
// 验证密码并开锁
void Input_Password()
{

	U8 key = 0;
	key = KEY_Scan(0);
	if (key != 0)
	{
		switch (key)
		{
		case 1:
			OLED_Clear();
			OLED_ShowString(5, 16, (unsigned char *)"Initialise", 16);
			OLED_ShowString(10, 40, (unsigned char *)"Waiting...", 16);
			OLED_Refresh();
			while (mpu_dmp_init())
			{
				delay_ms(10);
				// printf("1\n");
			}
			OLED_Clear();
			depth1 = depth;
			break;
		case 2:

			break;
		case 3:

			break;
		}
	}
}
// 解析json数据
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // 检测json格式
	cJSON *cjson_data = NULL; // 数据
	const char *massage;
	// 定义数据类型
	u8 cjson_cmd; // 指令,方向

	/* 解析整段JSO数据 */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// 解析失败
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* 依次根据名称提取JSON数据（键值对） */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* 解析嵌套json数据 */
	cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // 消息包
		// Data_init.price = cJSON_GetObjectItem(cjson_data, "price")->valuedouble;
		Data_init.App = 1;
		break;
	case 0x02: // 消息包
		// Data_init.add_wight = cJSON_GetObjectItem(cjson_data, "wight")->valuedouble;
		Data_init.App = 1;
		break;
	case 0x03: // 数据包

		break;
	case 0x04: // 数据包

		break;
	default:
		break;
	}

	/* 清空JSON对象(整条链表)的所有数据 */
	cJSON_Delete(cjson_test);
	//	cJSON_Delete(cjson_data);

	return MY_SUCCESSFUL;
}
