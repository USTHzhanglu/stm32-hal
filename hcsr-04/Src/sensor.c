#include "sensor.h"
#include "motor.h"
#include "tim.h"
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
 * @param  �������ַ��Ԫ�ظ���
 * @retval �����ĳ���������  
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