#include "robot-arm.h"

/* ����һ�����飬˵������˶��������ʵ��ת���෴�������෴�����޸Ķ�ӦλΪ-1
*/
const int arm_toggle[6]={1,1,1,1,1,1};
/* �ؽڵ�ǰ״̬*/
int arm_state[6]={0,0,0,0,0,0};

/* �ؽڳ�ʼ״̬*/
const int arm_begin[6]={0,85,-135,-54,0,0};
/* ��ס����*/
const int arm_clamp[6]={0,-35,-70,-59,0,0};
/* ��������*/
const int arm_doing[6]={0,-42,-37,0,0,0};
/**
 * @brief  ����ʼ�����0�����5������ʼ�����м�λ�á�
 * @param  ��void
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
 * @brief  �����ƶ��0�����5������תΪ��������תΪ����DJ0-5 +-135�㣬DJ6+-90��
 * @param  ��rad[6]���ֱ𴢴�ÿ�����ת���Ƕ�
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
 * @brief  �����ƶ��0�����5������תΪ��������תΪ����DJ0-5 +-135�㣬DJ6+-90��
 * @param  ��rad[6]���ֱ𴢴�ÿ�����ת���Ƕ�
 * @retval None
 **/
void arm_set(const int rad[6])
{	int set_status=1;//�����־�����ж��Ƿ���Ҫ����ѭ��
	int diff[6]={
		rad[0]-arm_state[0],
		rad[1]-arm_state[1],
		rad[2]-arm_state[2],
		rad[3]-arm_state[3],
		rad[4]-arm_state[4],
		rad[5]-arm_state[5],
		};//����һ�����飬������Ҫת���ĽǶ��ڵ�ǰ�ǶȵĲ�ֵ
	while(set_status){
		if(0 > diff[0])diff[0]++,arm_state[0]--;//�����ֵС��0��˵����ǰλ����Ҫ�����˶�����ǰ״ֵ̬--
		else if(diff[0] > 0)diff[0]--,arm_state[0]++;//�����ֵ����0��˵����ǰλ����Ҫ�����˶�����ǰ״ֵ̬++
		
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
			(diff[3]==0) && (diff[4]==0) && (diff[5]==0))set_status=0;//�����ֵΪ0������ѭ��
		rad2compare(arm_state);
		HAL_Delay(10);//������ڼ����Խ���ٶ�Խ��
	}
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
 * @brief  ���ֶ����ƶ��0�����5
 * @param  ��None
 * @retval None
 **/
void arm_task(void)
{
	char menu=NULL;
	char menu_before=NULL;
	int rad[6]={0,0,0,0,0,0};
	while(menu!='Q'){
		menu=get_cmd();
		if(menu){menu_before=menu;}//�������������������menu_before
		if(!menu){menu=menu_before;}//���û�����������ԭ��һ������
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
			case 'Z':break;//�չ�
		}
		HAL_Delay(50);//�����޸ļ���������ٶ�
		arm_set(rad);
		}
	printf_rad(rad);
	return;
}

void left_task(){
	arm_set(arm_doing);//�л�������ģʽ
	HAL_Delay(1000);
	arm_set(arm_clamp);//׼����ȡ
	
	HAL_Delay(1000);
	int arm_clamped[6];
	memcpy(arm_clamped,arm_clamp,sizeof(arm_clamp));
	arm_clamped[5]+=64;
	arm_set(arm_clamped);//��ȡ
	
	HAL_Delay(1000);
	memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
	arm_clamped[5]+=64;//�лص�����ģʽ
	arm_set(arm_clamped);
	
	HAL_Delay(1000);
	arm_clamped[0]+=90;//��ת
	arm_set(arm_clamped);
	
	HAL_Delay(1000);
	arm_clamped[1]=arm_clamp[1];
	arm_clamped[2]=arm_clamp[2];
	arm_clamped[3]=arm_clamp[3];
	arm_clamped[4]=arm_clamp[4];
	arm_set(arm_clamped);//׼������
	
	HAL_Delay(1000);
	arm_clamped[5]=0;
	arm_set(arm_clamped);//�ɿ�צ��
	
	HAL_Delay(1000);
	memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
	arm_clamped[0]+=90;//�лص�����ģʽ
	HAL_Delay(1000);
	arm_set(arm_clamped);
}

void right_task(){
	arm_set(arm_doing);//�л�������ģʽ
	HAL_Delay(1000);
	arm_set(arm_clamp);//׼����ȡ
	
	HAL_Delay(1000);
	int arm_clamped[6];
	memcpy(arm_clamped,arm_clamp,sizeof(arm_clamp));
	arm_clamped[5]+=64;
	arm_set(arm_clamped);//��ȡ
	
	HAL_Delay(1000);
	memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
	arm_clamped[5]+=64;//�лص�����ģʽ
	arm_set(arm_clamped);
	
	HAL_Delay(1000);
	arm_clamped[0]-=90;//��ת
	arm_set(arm_clamped);
	
	HAL_Delay(1000);
	arm_clamped[1]=arm_clamp[1];
	arm_clamped[2]=arm_clamp[2];
	arm_clamped[3]=arm_clamp[3];
	arm_clamped[4]=arm_clamp[4];
	arm_set(arm_clamped);//׼������
	
	HAL_Delay(1000);
	arm_clamped[5]=0;
	arm_set(arm_clamped);//�ɿ�צ��
	
	HAL_Delay(1000);
	memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
	arm_clamped[0]-=90;//�лص�����ģʽ
	HAL_Delay(1000);
	arm_set(arm_clamped);
	HAL_Delay(1000);
	arm_set(arm_begin);//�лش���ģʽ
}
void back_task(){

	arm_set(arm_begin);//�лش���ģʽ
		arm_set(arm_doing);//�л�������ģʽ
	HAL_Delay(1000);
	arm_set(arm_clamp);//׼����ȡ
	
	HAL_Delay(1000);
	int arm_clamped[6];
	memcpy(arm_clamped,arm_clamp,sizeof(arm_clamp));
	arm_clamped[5]+=64;
	arm_set(arm_clamped);//��ȡ
	
	HAL_Delay(1000);
	memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
	arm_clamped[5]+=64;//�лص�����ģʽ
	arm_set(arm_clamped);
	
	HAL_Delay(1000);
	arm_clamped[1]=-arm_clamped[1];
	arm_clamped[2]=-arm_clamped[2];
	arm_clamped[3]=-arm_clamped[3];
	arm_clamped[4]=-arm_clamped[4];
	arm_set(arm_clamped);//���񵽺���
	
	HAL_Delay(1000);
	arm_clamped[5]=0;
	arm_set(arm_clamped);//�ɿ�צ��
	
	HAL_Delay(1000);
	arm_set(arm_doing);
	
	HAL_Delay(1000);
	arm_set(arm_begin);//�лش���ģʽ
}