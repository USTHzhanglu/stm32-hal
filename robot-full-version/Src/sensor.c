#include "sensor.h"
#include "motor.h"
#include "tim.h"
#include "robot-arm.h"
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
			usart_send_str(&huart3,(unsigned char *)"Warning,is Ringing\r\n");
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
 * @param  ：无
 * @retval 无
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

/**
 * @brief  : 识别木块颜色,红绿蓝
 * @param  ：无
 * @retval   识别的颜色，R G B
**/  
COLOR_RGBC color_rgbc;
uint8_t get_color() {
	TCS34725_GetRawData(&color_rgbc);//获取RGB
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
 * @brief  ：处理颜色模块采集到的数据，取采集到的中间值
 * @param  ：数组地址，元素个数
 * @retval 处理后的颜色值  
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
 * @brief  ：颜色传感器对应任务
 * @param  ：无
 * @retval 无
**/ 
void color_task(){
	static uint32_t systick_ms_yanse = 0;
	int millis=HAL_GetTick();//获取系统时间
	uint8_t cmd_return[128];
	char color_value;
	if (millis - systick_ms_yanse > 20) {
		systick_ms_yanse = HAL_GetTick();
		color_value = get_adc_color_middle();//获取a0的ad值，计算出距离
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
 * @brief  ：巡线传感器对应任务
 * @param  ：无
 * @retval 1 or 0，对应巡线继续和停止巡线
**/ 

int xunji_task(void) {
	static uint32_t systick_ms_bak = 0;
	int speed = 500;
	int millis=HAL_GetTick();//获取系统时间
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
