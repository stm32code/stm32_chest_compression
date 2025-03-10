#include "git.h"
#include "onenet.h"
#if OLED // OLED�ļ�����
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
//  C��
#include "cjson.h"
#include <string.h>
#include <stdio.h>
#include "math.h"

Data_TypeDef Data_init;						  // �豸���ݽṹ��
Threshold_Value_TypeDef threshold_value_init; // �豸��ֵ���ýṹ��
Device_Satte_Typedef device_state_init;		  // �豸״̬
MPU6050_Typedef mup6050_init;				  // �豸״̬

F32 nums[SIZE];

float ax, ay, az;				   // ԭʼ���ٶ�ֵ����λΪ g
float sensitivity = 2.0 / 32768.0; // ���ٶȼƵ������ȣ���λΪ g/LSB
float ax_mps2, ay_mps2, az_mps2;   // ת����ļ��ٶ�ֵ����λΪ m/s^2
float v0 = 0;					   // ��ʼ�ٶȣ���λΪ m/s
float depth0 = -150;			   // ��ʼ��ȣ���λΪ cm
float dt = 0.05;				   // ʱ��������λΪ s
float depth, depth1;
// MPU6050��ʼ������ʡ��

void MPU6050_Read_Accel(void)
{

	// ת��Ϊ���ٶ�ֵ����λΪ m/s^2
	ax_mps2 = mup6050_init.aacx / sensitivity * 9.8;
	ay_mps2 = mup6050_init.aacy / sensitivity * 9.8;
	az_mps2 = mup6050_init.aacz / sensitivity * 9.8;

	// �������嵱ǰ���ٶ�
	float vx = ax_mps2 * dt;
	float vy = ay_mps2 * dt;
	float vz = az_mps2 * dt;

	// �������嵱ǰ��ƽ���ٶ�
	float v_avg = (vz + v0) / 2;

	// �������嵱ǰ��z�᷽���ϵ���ȱ仯
	float delta_z = v_avg * dt;

	// �������嵱ǰ�����
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

// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data)
{

	if (mpu_dmp_get_data(&mup6050_init.pitch, &mup6050_init.roll, &mup6050_init.yaw) == 0)
	{
		// �õ��¶�ֵ
		mup6050_init.temp = MPU_Get_Temperature();
		// �õ����ٶȴ���������
		MPU_Get_Accelerometer(&mup6050_init.aacx, &mup6050_init.aacy, &mup6050_init.aacz);
		// �õ�����������
		MPU_Get_Gyroscope(&mup6050_init.gyrox, &mup6050_init.gyroy, &mup6050_init.gyroz);
		MPU6050_Read_Accel();
	}

	return MY_SUCCESSFUL;
}

// ��ʼ��
mySta Reset_Threshole_Value(Data_TypeDef *Data, Device_Satte_Typedef *device_state)
{
	// ��ֵ����

	// ״̬����
	device_state->open = 0;
	return MY_SUCCESSFUL;
}
// ����OLED��ʾ��������
mySta Update_oled_massage()
{
#if OLED // �Ƿ��
	char str[50];
	u8 i;
	// ����OELD��ʾ������

	sprintf(str, "time : %02d ", device_state_init.hight_time);
	OLED_ShowString(0, 0, (unsigned char *)str, 16);

	sprintf(str, "num : %03d ", device_state_init.hight_num);
	OLED_ShowString(0, 16, (unsigned char *)str, 16);

	sprintf(str, "HZ : %03d HZ", device_state_init.hight_hz);
	OLED_ShowString(0, 32, (unsigned char *)str, 16);

	sprintf(str, "hight : %03d mm", (U16)Data_init.hight);
	OLED_ShowString(0, 48, (unsigned char *)str, 16);

	OLED_Refresh(); // ������ʾ
#endif

	return MY_SUCCESSFUL;
}
// ����ʱ
void updata_time(void)
{
	if (device_state_init.hight_time > 0)
	{
		device_state_init.hight_time--;
	}
	else if (device_state_init.hight_time == 0)
	{
		device_state_init.hight_time = 10;
		// ����Ƶ��
		device_state_init.hight_hz = device_state_init.hight_num / device_state_init.hight_time;
		// ��ռ���
		device_state_init.hight_num = 0;
		// ���͸�APP
		OneNet_SendMqtt(1); // �ϴ����ݵ�ƽ̨
	}
}
//  �����豸״̬
mySta Update_device_massage()
{
	// �Ƚϣ��仯ֵΪ120�������쳣ֵ
	if (150 < Data_init.hight && Data_init.hight < 250 && device_state_init.hight_state == 1)
	{
		Data_init.hight_end = Data_init.hight;
		OneNet_SendMqtt(1); // �ϴ����ݵ�ƽ̨
		device_state_init.hight_state = 0;
		// ������һ
		device_state_init.hight_num++;
	}
	else if (Data_init.hight < 25 && device_state_init.hight_state == 0)
	{
		// ��ֵ�ߵ�(�ı�һ��)
		Data_init.hight_begin = Data_init.hight;
		OneNet_SendMqtt(1); // �ϴ����ݵ�ƽ̨
		device_state_init.hight_state = 1;
	}

	return MY_SUCCESSFUL;
}
// ��֤���벢����
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
// ����json����
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // ���json��ʽ
	cJSON *cjson_data = NULL; // ����
	const char *massage;
	// ������������
	u8 cjson_cmd; // ָ��,����

	/* ��������JSO���� */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// ����ʧ��
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* ���θ���������ȡJSON���ݣ���ֵ�ԣ� */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* ����Ƕ��json���� */
	cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // ��Ϣ��
		// Data_init.price = cJSON_GetObjectItem(cjson_data, "price")->valuedouble;
		Data_init.App = 1;
		break;
	case 0x02: // ��Ϣ��
		// Data_init.add_wight = cJSON_GetObjectItem(cjson_data, "wight")->valuedouble;
		Data_init.App = 1;
		break;
	case 0x03: // ���ݰ�

		break;
	case 0x04: // ���ݰ�

		break;
	default:
		break;
	}

	/* ���JSON����(��������)���������� */
	cJSON_Delete(cjson_test);
	//	cJSON_Delete(cjson_data);

	return MY_SUCCESSFUL;
}
