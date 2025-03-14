/************************************************************************************
*************************************************************************************/
#include "HX711.h"
#include "delay.h"
 
u32 HX711_Buffer;
u32 Weight_Maopi;
F32 Weight_Shiwu;
u8 Flag_Error = 0;
extern Data_TypeDef Data_init;						  // 设备数据结构体
// 校准参数
// 因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
// 当发现测试出来的重量偏大时，增加该数值。
// 如果测试出来的重量偏小时，减小改数值。
// 该值可以为小数
 
#define Value 100000
#define Gap_Value 1400
 
void Init_HX711pin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(SCK_GPIO_CLK|DOUT_GPIO_CLK, ENABLE); // 使能PF端口时钟
 
	// HX711_SCK
	GPIO_InitStructure.GPIO_Pin = SCK_GPIO_PIN;		  // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
	GPIO_Init(SCK_GPIO_PORT, &GPIO_InitStructure);			  // 根据设定参数初始化GPIOB
 
	// HX711_DOUT
	GPIO_InitStructure.GPIO_Pin = DOUT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 输入上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	GPIO_SetBits(SCK_GPIO_PORT, SCK_GPIO_PIN); // 初始化设置为0
}
 
//****************************************************
// 读取HX711
//****************************************************
u32 HX711_Read(void) // 增益128
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
	count = count ^ 0x800000; // 第25个脉冲下降沿来时，转换数据
	delay_us(1);
	HX711_SCK = 0;
	return (count);
}
 
//****************************************************
// 获取毛皮重量
//****************************************************
void Get_Maopi(void)
{
	Weight_Maopi = HX711_Read();
}
 
//****************************************************
// 称重
//****************************************************
void Get_Weight(void)
{
	HX711_Buffer = HX711_Read();
	if (HX711_Buffer > Weight_Maopi)
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi; // 获取实物的AD采样数值。
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

		// 计算实物的实际重量
		// 因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的GapValue这个除数。
		// 当发现测试出来的重量偏大时，增加该数值。
		// 如果测试出来的重量偏小时，减小改数值。
	}
}
