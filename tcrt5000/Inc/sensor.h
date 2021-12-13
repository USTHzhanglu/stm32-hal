#include "stdint.h"
#include "color.h"
#define sound_vol HAL_GPIO_ReadPin(sound_GPIO_Port,sound_Pin)

#define Trig(x) HAL_GPIO_WritePin(Trig_GPIO_Port,Trig_Pin,(GPIO_PinState)x)
#define Echo() HAL_GPIO_ReadPin(Echo_GPIO_Port,Echo_Pin)

#define YSSB_LED(x) TCS34725_LedON(x); //颜色识别的LED灯

#define D1() HAL_GPIO_ReadPin(D1_GPIO_Port,D1_Pin)
#define D2() HAL_GPIO_ReadPin(D2_GPIO_Port,D2_Pin)
#define XJ_ON 0
#define XJ_OFF 1


void sound_task(void);
void hcsr_task(void);
void color_task(void);
uint16_t get_csb_value(void);
uint8_t get_color(void);
int xunji_task(void);