#include "sensor.h"
#include "motor.h"
/**
 * @brief  ����ȡ������������ֵ����ִ�ж�Ӧ����
 * @param  ��None
 * @retval None
 **/
 void sound_task(void)
{
	 if(!sound_vol){
		 HAL_Delay(10);//����
		 if(!sound_vol)
		 {	//������Ҫִ�еĴ���
			usart_send_str(&huart3,(unsigned char *)"Warning,is ringing\r\n");
			}
		 }
	 return;
 }