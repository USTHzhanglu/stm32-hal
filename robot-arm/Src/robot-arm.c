#include "robot-arm.h"

/* ����һ�����飬˵������˶��������ʵ��ת���෴�������෴�����޸Ķ�ӦλΪ-1
*/
const int arm_toggle[6]={1,1,1,1,1,1};

/* �ؽڳ�ʼ״̬*/
const int arm_begin[6]={0,85,-135,-54,0,0};
/**
 * @brief  ����ʼ�����0�����5������ʼ�����м�λ�á�
 * @param  ��void
 * @retval None
 **/
void arm_init(void)
{
	arm_set(arm_begin);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);//DJ0,PB3
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);//DJ1,PB8
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);//DJ2,PB9
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);//DJ3,PB6
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);//DJ4,PB7
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);//DJ5,PB4
	return;
}
/**
 * @brief  �����ƶ��0�����5������תΪ��������תΪ����DJ0-5 +-135�㣬DJ6+-90��
 * @param  ��rad[6]���ֱ𴢴�ÿ�����ת���Ƕ�
 * @retval None
 **/
void arm_set(const int rad[6])
{
	int compare[6]={
		1500+rad[0]*arm_toggle[0]*1000/135,
		1500+rad[1]*arm_toggle[1]*1000/135,
		1500+rad[2]*arm_toggle[2]*1000/135,
		1500+rad[3]*arm_toggle[3]*1000/135,
		1500+rad[4]*arm_toggle[4]*1000/135,
		1500+rad[5]*arm_toggle[5]*1000/90,
		};
__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,compare[0]);
__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_3,compare[1]);
__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_4,compare[2]);
__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,compare[3]);
__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,compare[4]);
__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,compare[5]);
		return;
}

/**
 * @brief  ��ӡ��ǰ�ؽڽǶ�
 * @param  ��arm_rad[6]���ֱ𴢴�ÿ�����ת���Ƕ�
 * @retval None
 **/
void printf_rad(const int arm_rad[6])
{  
	char cmd_return[128];
	sprintf(cmd_return, "{rad:%d,%d,%d,%d,%d,%d}",arm_rad[0],arm_rad[1],arm_rad[2],arm_rad[3],arm_rad[4],arm_rad[5]);
	usart_send_str(&huart3,(unsigned char *)cmd_return);
	return;
}

/**
 * @brief  �����ƶ��0�����5������ӡ��ǰ�Ƕ�
 * @param  ��None
 * @retval None
 **/
void get_rad(void)
{
	char menu;
	int arm_status=1;
	int rad[6]={0,0,0,0,0,0};
	while( arm_status)
	{menu=get_cmd();
		switch(menu)
		{
			  case 'A':rad[0]++;break;
			  case 'B':rad[0]--;break;
			  case 'C':rad[1]++;break;
			  case 'D':rad[1]--;break;
			  case 'E':rad[2]++;break;			  
			  case 'F':rad[2]--;break;
			  case 'G':rad[3]++;break;			  
			  case 'H':rad[3]--;break;
			  case 'I':rad[4]++;break;			  
			  case 'J':rad[4]--;break;
			  case 'K':rad[5]++;break;			  
			  case 'L':rad[5]--;break;		
			  case 'Q':arm_status=0;break;
		}
	printf_rad(rad);
	arm_set(rad);
	menu=NULL;
	}
return;
}	
