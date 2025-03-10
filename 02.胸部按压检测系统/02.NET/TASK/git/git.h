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

// ��������Ϣ
#define SSID "NET"				 // ·����SSID����
#define PASS "12345678"			 // ·��������
#define ServerIP "183.230.40.39" // ������IP��ַ
#define ServerPort 6002			 // ������IP��ַ�˿ں�
// �豸��Ϣ
#define PROID "591614"	   // ��ƷID
#define DEVID "1068069364" // �豸ID
#define AUTH_INFO "1"	   // ��Ȩ��Ϣ
// MQTT����
#define S_TOPIC_NAME "tea_topic"	// ��Ҫ���ĵ�����
#define P_TOPIC_NAME "teaapp_topic" // ��Ҫ����������

//  �豸ʹ�����趨��
#define OLED 1 // �Ƿ�ʹ��OLED
#define SIZE 10

// �Զ���ִ�н������
typedef enum
{
	MY_SUCCESSFUL = 0x01, // �ɹ�
	MY_FAIL = 0x00		  // ʧ��

} mySta; // �ɹ���־λ

typedef enum
{
	OPEN = 0x01, // ��
	CLOSE = 0x00 // �ر�

} On_or_Off_TypeDef; // �ɹ���־λ

typedef enum
{
	DERVICE_SEND = 0x00, // �豸->ƽ̨
	PLATFORM_SEND = 0x01 // ƽ̨->�豸

} Send_directino; // ���ͷ���

// �Զ��岼������
typedef enum
{
	MY_TRUE,
	MY_FALSE
} myBool;

typedef struct
{
	U8 App; // ָ��ģʽ

	U8 Flage;		  // ģʽѡ��
	U16 people;		  // ����
	U8 light;		  // ����
	U16 somg;		  // ����Ũ��
	float hight;	  // �仯�߶�
	U16 hight_begin;  // �仯�߶�
	U16 hight_end;	  // �仯�߶�
	U16 hight_change; // ��¼һ�α仯�߶�(100Ϊ������)

} Data_TypeDef; // ���ݲ����ṹ��

typedef struct
{

	U16 threshold_temp; // �¶�
	U16 threshold_humi; // �¶�

} Threshold_Value_TypeDef; // ���ݲ����ṹ��

typedef struct
{
	U8 open;	   // ��
	U8 check_open; // ����Ƿ�������
	U8 ckeck_humi; // ����Ƿ�������

	U16 hight_state; // ��¼һ�α仯�߶�(100Ϊ������)

	U16 hight_num;	// ��¼10S�仯��������Ƶ��
	U16 hight_hz;	// ��¼10S�仯��������Ƶ��
	U16 hight_time; // Ƶ�ʼ�ʱ��ʱ��

} Device_Satte_Typedef; // ״̬�����ṹ��

extern Data_TypeDef Data_init;
extern Device_Satte_Typedef device_state_init;		 // �豸״̬
extern Threshold_Value_TypeDef threshold_value_init; // �豸��ֵ���ýṹ��

// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data);
// ��ʼ��
mySta Reset_Threshole_Value(Data_TypeDef *Data, Device_Satte_Typedef *device_state);
// ����OLED��ʾ��������
mySta Update_oled_massage(void);
// �����豸״̬
mySta Update_device_massage(void);
// ����json����
mySta massage_parse_json(char *message);
// ��֤���벢����
void Input_Password(void);
// ����ʱ
void updata_time(void);
#endif
