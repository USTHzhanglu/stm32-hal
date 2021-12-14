#include "robot-arm.h"

/* 定义一个数组，说明舵机运动方向，如果实际转向相反与输入相反，则修改对应位为-1
*/
const int arm_toggle[6]={1,1,1,1,1,1};
/* 关节当前状态*/
int arm_state[6]={0,0,0,0,0,0};

/* 关节初始状态*/
const int arm_begin[6]={0,85,-135,-54,0,0};
/* 夹住物体*/
const int arm_clamp[6]={0,-35,-70,-59,0,0};
/* 运输物体*/
const int arm_doing[6]={0,-42,-37,0,0,0};
/**
 * @brief  ：初始化舵机0到舵机5，均初始化到中间位置。
 * @param  ：void
 * @retval None
 **/
void arm_init(void)
{
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
void rad2compare(const int rad[6])
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
 * @brief  ：控制舵机0到舵机5，左上转为正，右下转为负，DJ0-5 +-135°，DJ6+-90°
 * @param  ：rad[6]，分别储存每个舵机转动角度
 * @retval None
 **/
void arm_set(const int rad[6])
{	int set_status=1;//定义标志符，判断是否需要继续循环
	int diff[6]={
		rad[0]-arm_state[0],
		rad[1]-arm_state[1],
		rad[2]-arm_state[2],
		rad[3]-arm_state[3],
		rad[4]-arm_state[4],
		rad[5]-arm_state[5],
		};//定义一个数组，计算需要转到的角度于当前角度的差值
	while(set_status){
		if(0 > diff[0])diff[0]++,arm_state[0]--;//如果差值小于0，说明当前位置需要逆向运动，当前状态值--
		else if(diff[0] > 0)diff[0]--,arm_state[0]++;//如果差值大于0，说明当前位置需要正向运动，当前状态值++
		
		if(0 > diff[1])diff[1]++,arm_state[1]--;
		else if(diff[1] > 0)diff[1]--,arm_state[1]++;
		
		if(0 > diff[2])diff[2]++,arm_state[2]--;
		else if(diff[2] > 0)diff[2]--,arm_state[2]++;
		
		if(0 > diff[3])diff[3]++,arm_state[3]--;
		else if(diff[3] > 0)diff[3]--,arm_state[3]++;
		
		if(0 > diff[4])diff[4]++,arm_state[4]--;
		else if(diff[4] > 0)diff[4]--,arm_state[4]++;
		
		if(0 > diff[5])diff[5]++,arm_state[5]--;
		else if(diff[5] > 0)diff[5]--,arm_state[5]++;
		
		if( (diff[0]==0) && (diff[1]==0) && (diff[2]==0) && 
			(diff[3]==0) && (diff[4]==0) && (diff[5]==0))set_status=0;//如果差值为0后，跳出循环
		rad2compare(arm_state);
		HAL_Delay(10);//舵机调节间隔，越长速度越慢
	}
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
	//usart_send_str(&huart3,(unsigned char *)"while begin\r\n");
	while(arm_status==1)
	{//usart_send_str(&huart3,(unsigned char *)"get_cmd\r\n");
		menu=get_cmd();
		//usart_send_str(&huart3,(unsigned char *)"begin switch\r\n");
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
			default:usart_send_str(&huart3,(unsigned char *)"Error cmd\r\n");break;
		}
		//usart_send_str(&huart3,(unsigned char *)"end switch\r\n");
	printf_rad(rad);
	arm_set(rad);
	menu=NULL;
	}
	//usart_send_str(&huart3,(unsigned char *)"end while\r\n");
return;
}	

/**
 * @brief  ：手动控制舵机0到舵机5
 * @param  ：None
 * @retval None
 **/
void arm_task(void)
{
	char menu=NULL;
	char menu_before=NULL;
	int rad[6]={0,0,0,0,0,0};
	while(menu!='Q'){
		menu=get_cmd();
		if(menu){menu_before=menu;}//如果有命令传来，备份命令到menu_before
		if(!menu){menu=menu_before;}//如果没有命令传来，还原上一个命令
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
			case 'Z':break;//空过
		}
		HAL_Delay(50);//两次修改间隔，调节速度
		arm_set(rad);
		}
	printf_rad(rad);
	return;
}

void left_task(){
	arm_set(arm_doing);//切换到运输模式
	HAL_Delay(1000);
	arm_set(arm_clamp);//准备夹取
	
	HAL_Delay(1000);
	int arm_clamped[6];
	memcpy(arm_clamped,arm_clamp,sizeof(arm_clamp));
	arm_clamped[5]+=64;
	arm_set(arm_clamped);//夹取
	
	HAL_Delay(1000);
	memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
	arm_clamped[5]+=64;//切回到运输模式
	arm_set(arm_clamped);
	
	HAL_Delay(1000);
	arm_clamped[0]+=90;//左转
	arm_set(arm_clamped);
	
	HAL_Delay(1000);
	arm_clamped[1]=arm_clamp[1];
	arm_clamped[2]=arm_clamp[2];
	arm_clamped[3]=arm_clamp[3];
	arm_clamped[4]=arm_clamp[4];
	arm_set(arm_clamped);//准备放下
	
	HAL_Delay(1000);
	arm_clamped[5]=0;
	arm_set(arm_clamped);//松开爪子
	
	HAL_Delay(1000);
	memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
	arm_clamped[0]+=90;//切回到运输模式
	HAL_Delay(1000);
	arm_set(arm_clamped);
}

void right_task(){
	arm_set(arm_doing);//切换到运输模式
	HAL_Delay(1000);
	arm_set(arm_clamp);//准备夹取
	
	HAL_Delay(1000);
	int arm_clamped[6];
	memcpy(arm_clamped,arm_clamp,sizeof(arm_clamp));
	arm_clamped[5]+=64;
	arm_set(arm_clamped);//夹取
	
	HAL_Delay(1000);
	memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
	arm_clamped[5]+=64;//切回到运输模式
	arm_set(arm_clamped);
	
	HAL_Delay(1000);
	arm_clamped[0]-=90;//右转
	arm_set(arm_clamped);
	
	HAL_Delay(1000);
	arm_clamped[1]=arm_clamp[1];
	arm_clamped[2]=arm_clamp[2];
	arm_clamped[3]=arm_clamp[3];
	arm_clamped[4]=arm_clamp[4];
	arm_set(arm_clamped);//准备放下
	
	HAL_Delay(1000);
	arm_clamped[5]=0;
	arm_set(arm_clamped);//松开爪子
	
	HAL_Delay(1000);
	memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
	arm_clamped[0]-=90;//切回到运输模式
	HAL_Delay(1000);
	arm_set(arm_clamped);
	HAL_Delay(1000);
	arm_set(arm_begin);//切回待机模式
}
void back_task(){

	arm_set(arm_begin);//切回待机模式
		arm_set(arm_doing);//切换到运输模式
	HAL_Delay(1000);
	arm_set(arm_clamp);//准备夹取
	
	HAL_Delay(1000);
	int arm_clamped[6];
	memcpy(arm_clamped,arm_clamp,sizeof(arm_clamp));
	arm_clamped[5]+=64;
	arm_set(arm_clamped);//夹取
	
	HAL_Delay(1000);
	memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
	arm_clamped[5]+=64;//切回到运输模式
	arm_set(arm_clamped);
	
	HAL_Delay(1000);
	arm_clamped[1]=-arm_clamped[1];
	arm_clamped[2]=-arm_clamped[2];
	arm_clamped[3]=-arm_clamped[3];
	arm_clamped[4]=-arm_clamped[4];
	arm_set(arm_clamped);//镜像到后面
	
	HAL_Delay(1000);
	arm_clamped[5]=0;
	arm_set(arm_clamped);//松开爪子
	
	HAL_Delay(1000);
	arm_set(arm_doing);
	
	HAL_Delay(1000);
	arm_set(arm_begin);//切回待机模式
}