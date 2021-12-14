#include "motor.h"
#include "sensor.h"
void usart_send_str(UART_HandleTypeDef *huart,unsigned char *Data) {
	while(HAL_UART_Transmit_IT(huart, Data,strlen((char *)Data))!=HAL_OK){};
	return;
}
void motor_init(void){
	usart_send_str(&huart3,(unsigned char *)"#255PID001!");
	return;
}

//小车控制函数
//参数 左轮速度和右轮速度 范围 -1000 到 1000
void motor_set(int car_left, int car_right) {
	//总线马达设置
	char cmd_return[128];
	sprintf(cmd_return, "{#001P%04dT0000!#002P%04dT0000!}", (int)(1500+car_left), (int)(1500-car_right));
	usart_send_str(&huart3,(unsigned char *)cmd_return);
	return;
}
char get_cmd(void){
	uint8_t ch=NULL;
	if(blue_cmd!=NULL)
	{
	usart_send_str(&huart3,&blue_cmd);
	ch=blue_cmd;
	blue_cmd=NULL;
	return ch;
	}
	return 0;
}

void motor_task(){
	char input=NULL;
	while(input!='Q'){
		input=get_cmd();
		if(input){
		switch(input){	 
				case 'B':
				usart_send_str(&huart3,(unsigned char *)"car go\r\n");
				motor_set(1000,1000);
				break;
				case 'C':
				usart_send_str(&huart3,(unsigned char *)"car back\r\n");
				motor_set(-1000,-1000);
				break;
				case 'D':
				usart_send_str(&huart3,(unsigned char *)"car left\r\n");
				motor_set(1000,0);
				break;
				case 'E':
				usart_send_str(&huart3,(unsigned char *)"car right\r\n");
				motor_set(0,1000);
				break;			  
				case 'Z':
				usart_send_str(&huart3,(unsigned char *)"car stop\r\n");
				motor_set(0,0);
				break;
			}
		}
	}
}

