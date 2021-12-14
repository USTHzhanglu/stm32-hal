#include "sensor.h"
#include "motor.h"
#include "tim.h"
#include "robot-arm.h"
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
			usart_send_str(&huart3,(unsigned char *)"Warning,is Ringing\r\n");
			}
		 }
	 return;
 }

void csb_Delay_Us(uint16_t time)  //��ʱ����
{ 
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<9;j++);
}
/**
 * @brief  �����ڸ���Ԫ�س������飬��С�����������
 * @param  �������ַ��Ԫ�ظ���
 * @retval None
**/
void selection_sort(int *a, int len) {
    int i,j,mi,t;
    for(i=0;i<len-1;i++) {
        mi = i;
        for(j=i+1;j<len;j++) {
            if(a[mi] > a[j]) {
                mi = j;    
            }    
        }    
		
        if(mi != i) {
            t = a[mi];
            a[mi] = a[i];
            a[i] = t;    
        }
    }
}
 /**
 * @brief  ���ɼ�����������
 * @param  ��None
 * @retval �ɼ�������  
**/
uint16_t get_csb_value(void) {
	uint16_t csb_t;
	Trig(1);
	csb_Delay_Us(20);
	Trig(0);
	while(Echo() == 0);      //�ȴ����տڸߵ�ƽ���
	__HAL_TIM_SetCounter(&htim2,0);
	while(Echo() == 1);     
	csb_t =__HAL_TIM_GetCounter(&htim2);//��ȡʱ��,�ֱ���Ϊ1US
	//340m/s = 0.017cm/us
	if(csb_t < 20000) {
		//sprintf((char *)cmd_return, "csb_time=%d\r\n", (int)(csb_t*0.017));
		//usart_send_str(&huart3,cmd_return);
		csb_t = csb_t*0.017;
		return csb_t;
	}
	return 0;
}

/**
 * @brief  �����������ɼ��������ݣ�ȡ�ɼ������м�ֵ
 * @param  �������ַ��Ԫ�ظ���
 * @retval �����ĳ���������  
**/  
int get_adc_csb_middle() {
	
	uint8_t i;
	static int ad_value[5] = {0}, myvalue;// ad_value_bak[5] = {0}, 
	for(i=0;i<5;i++)ad_value[i] = get_csb_value();
	selection_sort(ad_value, 5);
	myvalue = ad_value[2];
	return myvalue;  
}

/**
 * @brief  : ���ݳ��������ص�����ִ���Զ����Ϲ��ܡ�
 * @param  ����
 * @retval ��
**/  
void hcsr_task(void)
{
	static uint32_t systick_ms_bak = 0;
	uint8_t cmd_return[100];
	int speed = 500, adc_csb;
	int millis=HAL_GetTick();//��ȡϵͳʱ��
	if(millis - systick_ms_bak > 10) {
		systick_ms_bak = HAL_GetTick();
		adc_csb = get_adc_csb_middle();//��ȡa0��adֵ�����������
		sprintf((char *)cmd_return, "adc_csb = %d\r\n", adc_csb);
		usart_send_str(&huart3,cmd_return);
		if(adc_csb < 30) {//�������20cm����ת
			motor_set(speed+500, -speed-500);
			HAL_Delay(500);
		}else if(adc_csb>80){
			motor_set(1000, 1000);
		}else{
			motor_set(speed,speed);
		}
	}
}

/**
 * @brief  : ʶ��ľ����ɫ,������
 * @param  ����
 * @retval   ʶ�����ɫ��R G B
**/  
COLOR_RGBC color_rgbc;
uint8_t get_color() {
	TCS34725_GetRawData(&color_rgbc);//��ȡRGB
	if(color_rgbc.c>400){return 0;}
      if (color_rgbc.r > color_rgbc.g && color_rgbc.r  > color_rgbc.b ) {
		//usart_send_str(&huart3,(uint8_t *)"May RED\r\n");
		  return 'R';
      } else if (color_rgbc.g > color_rgbc.r && color_rgbc.g  > color_rgbc.b) {
		  //usart_send_str(&huart3,(uint8_t *)"May GREEN\r\n");
		  return 'G';
      } else if (color_rgbc.b > color_rgbc.g && color_rgbc.b  > color_rgbc.r) {
		  //usart_send_str(&huart3,(uint8_t *)"May BULE\r\n");
		  return 'B';
      }
	return 0;
}

/**
 * @brief  ��������ɫģ��ɼ��������ݣ�ȡ�ɼ������м�ֵ
 * @param  �������ַ��Ԫ�ظ���
 * @retval ��������ɫֵ  
**/  
char get_adc_color_middle() {
	YSSB_LED(1);
	uint8_t i;
	static int ad_value[5] = {0}, myvalue;// ad_value_bak[5] = {0}, 
	for(i=0;i<5;i++){ad_value[i] = get_color();HAL_Delay(200);};
	selection_sort(ad_value, 5);
	myvalue = ad_value[2];
	YSSB_LED(0);
	return myvalue;  
}

/**
 * @brief  ����ɫ��������Ӧ����
 * @param  ����
 * @retval ��
**/ 
void color_task(){
	static uint32_t systick_ms_yanse = 0;
	int millis=HAL_GetTick();//��ȡϵͳʱ��
	uint8_t cmd_return[128];
	char color_value;
	if (millis - systick_ms_yanse > 20) {
		systick_ms_yanse = HAL_GetTick();
		color_value = get_adc_color_middle();//��ȡa0��adֵ�����������
		if(color_value){
			sprintf((char *)cmd_return, "Color = [%c]\r\n", color_value);
			usart_send_str(&huart3,cmd_return);
			if(color_value=='R'){left_task();}
			else if(color_value=='G'){right_task();}
			else if(color_value=='B'){back_task();}
			color_value=NULL;
		}else
		{usart_send_str(&huart3,(uint8_t *)"no wood block \r\n");
		}	  
	}
}

/**
 * @brief  ��Ѳ�ߴ�������Ӧ����
 * @param  ����
 * @retval 1 or 0����ӦѲ�߼�����ֹͣѲ��
**/ 

int xunji_task(void) {
	static uint32_t systick_ms_bak = 0;
	int speed = 500;
	int millis=HAL_GetTick();//��ȡϵͳʱ��
	if(millis - systick_ms_bak < 50) return 1;
	systick_ms_bak = HAL_GetTick();
	
	if((D2() == XJ_OFF) && (D1() == XJ_ON)) {
		motor_set(-speed-300, speed+300);
	} else if((D2() == XJ_ON) && (D1() == XJ_ON)) {
		motor_set(speed, speed);
	} else if((D2() == XJ_ON) && (D1() == XJ_OFF)) {
		
		motor_set(speed+300,-speed-300);
	} else if((D2() == XJ_OFF) && (D1() == XJ_OFF)){
		motor_set(0,0);
		return 0;
	}
	return 1;
}
