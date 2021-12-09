#include "sensor.h"
#include "motor.h"
/**
 * @brief  ：获取声音传感器的值，并执行对应操作
 * @param  ：None
 * @retval None
 **/
 void sound_task(void)
{
	 if(!sound_vol){
		 HAL_Delay(10);//消抖
		 if(!sound_vol)
		 {	//放入需要执行的代码
			usart_send_str(&huart3,(unsigned char *)"Warning,is ringing\r\n");
			}
		 }
	 return;
 }