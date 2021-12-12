#include "sensor.h"
#include "motor.h"
#include "tim.h"
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

void csb_Delay_Us(uint16_t time)  //延时函数
{ 
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<9;j++);
}
/**
 * @brief  ：对于给定元素长度数组，从小到大进行排序
 * @param  ：数组地址，元素个数
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
 * @brief  ：采集超声波数据
 * @param  ：None
 * @retval 采集的数据  
**/
uint16_t get_csb_value(void) {
	uint16_t csb_t;
	Trig(1);
	csb_Delay_Us(20);
	Trig(0);
	while(Echo() == 0);      //等待接收口高电平输出
	__HAL_TIM_SetCounter(&htim2,0);
	while(Echo() == 1);     
	csb_t =__HAL_TIM_GetCounter(&htim2);//获取时间,分辨率为1US
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
 * @brief  ：处理超声波采集到的数据，取采集到的中间值
 * @param  ：数组地址，元素个数
 * @retval 处理后的超声波数据  
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
 * @brief  : 根据超声波返回的数据执行自动避障功能。
 * @param  ：数组地址，元素个数
 * @retval 处理后的超声波数据  
**/  
void hcsr_task(void)
{
	static uint32_t systick_ms_bak = 0;
	uint8_t cmd_return[100];
	int speed = 500, adc_csb;
	int millis=HAL_GetTick();//获取系统时间
	if(millis - systick_ms_bak > 10) {
		systick_ms_bak = HAL_GetTick();
		adc_csb = get_adc_csb_middle();//获取a0的ad值，计算出距离
		sprintf((char *)cmd_return, "adc_csb = %d\r\n", adc_csb);
		usart_send_str(&huart3,cmd_return);
		if(adc_csb < 30) {//距离低于20cm就右转
			motor_set(speed+500, -speed-500);
			HAL_Delay(500);
		}else if(adc_csb>80){
			motor_set(1000, 1000);
		}else{
			motor_set(speed,speed);
		}
	}
}