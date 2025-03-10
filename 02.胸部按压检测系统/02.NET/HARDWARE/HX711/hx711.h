#ifndef __HX711_H
#define __HX711_H
 
#include "sys.h"
#include "git.h"
 
 
#define SCK_GPIO_PORT    	GPIOA          /* GPIO端口 */
#define SCK_GPIO_CLK 	    RCC_APB2Periph_GPIOA	/* GPIO端口时钟 */
#define SCK_GPIO_PIN			GPIO_Pin_0

#define DOUT_GPIO_PORT    	GPIOA          /* GPIO端口 */
#define DOUT_GPIO_CLK 	    RCC_APB2Periph_GPIOA	/* GPIO端口时钟 */
#define DOUT_GPIO_PIN			GPIO_Pin_1

#define HX711_SCK PAout(0) // PB0
#define HX711_DOUT PAin(1) // PB1
 
extern void Init_HX711pin(void);
extern u32 HX711_Read(void);
extern void Get_Maopi(void);
extern void Get_Weight(void);
 
extern u32 HX711_Buffer;
extern u32 Weight_Maopi;
extern F32 Weight_Shiwu;
extern u8 Flag_Error;

 
#endif
