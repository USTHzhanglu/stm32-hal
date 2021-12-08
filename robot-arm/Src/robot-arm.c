#include "robot-arm.h"

/* 定义一个数组，说明舵机运动方向，如果实际转向相反与输入相反，则修改对应位为-1
*/
const int arm_toggle[6]={1,1,1,1,1,1};

/* 关节初始状态*/
const int arm_begin[6]={0,85,-135,-54,0,0};
/**
 * @brief  ：初始化舵机0到舵机5，均初始化到中间位置。
 * @param  ：void
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
 * @brief  ：控制舵机0到舵机5，左上转为正，右下转为负，DJ0-5 +-135°，DJ6+-90°
 * @param  ：rad[6]，分别储存每个舵机转动角度
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
 * @brief  打印当前关节角度
 * @param  ：arm_rad[6]，分别储存每个舵机转动角度
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
 * @brief  ：控制舵机0到舵机5，并打印当前角度
 * @param  ：None
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
