/************************************************************************************
*************************************************************************************/
#include "HX711.h"
#include "delay.h"
 
u32 HX711_Buffer;
u32 Weight_Maopi;
F32 Weight_Shiwu;
u8 Flag_Error = 0;
extern Data_TypeDef Data_init;						  // �豸���ݽṹ��
// У׼����
// ��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
// �����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
// ������Գ���������ƫСʱ����С����ֵ��
// ��ֵ����ΪС��
 
#define Value 100000
#define Gap_Value 1400
 
void Init_HX711pin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(SCK_GPIO_CLK|DOUT_GPIO_CLK, ENABLE); // ʹ��PF�˿�ʱ��
 
	// HX711_SCK
	GPIO_InitStructure.GPIO_Pin = SCK_GPIO_PIN;		  // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO���ٶ�Ϊ50MHz
	GPIO_Init(SCK_GPIO_PORT, &GPIO_InitStructure);			  // �����趨������ʼ��GPIOB
 
	// HX711_DOUT
	GPIO_InitStructure.GPIO_Pin = DOUT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	GPIO_SetBits(SCK_GPIO_PORT, SCK_GPIO_PIN); // ��ʼ������Ϊ0
}
 
//****************************************************
// ��ȡHX711
//****************************************************
u32 HX711_Read(void) // ����128
{
	unsigned long count;
	unsigned char i;
	HX711_DOUT = 1;
	delay_us(1);
	HX711_SCK = 0;
	count = 0;
	while (HX711_DOUT)
		;
	for (i = 0; i < 24; i++)
	{
		HX711_SCK = 1;
		count = count << 1;
		delay_us(1);
		HX711_SCK = 0;
		if (HX711_DOUT)
			count++;
		delay_us(1);
	}
	HX711_SCK = 1;
	count = count ^ 0x800000; // ��25�������½�����ʱ��ת������
	delay_us(1);
	HX711_SCK = 0;
	return (count);
}
 
//****************************************************
// ��ȡëƤ����
//****************************************************
void Get_Maopi(void)
{
	Weight_Maopi = HX711_Read();
}
 
//****************************************************
// ����
//****************************************************
void Get_Weight(void)
{
	HX711_Buffer = HX711_Read();
	if (HX711_Buffer > Weight_Maopi)
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi; // ��ȡʵ���AD������ֵ��
		Weight_Shiwu = Weight_Shiwu / Gap_Value;
		if(Weight_Shiwu>0.5){
			Data_init.wight = Weight_Shiwu*11.996693711;

		}else{
			Data_init.git_wight=0;
			Data_init.wight=0;
			Data_init.wight_time=SIZE;
			Data_init.average_wight=0;
			Data_init.add_price=0;
		}

		// ����ʵ���ʵ������
		// ��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������GapValue���������
		// �����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
		// ������Գ���������ƫСʱ����С����ֵ��
	}
}
