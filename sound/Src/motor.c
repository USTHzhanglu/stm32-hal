#include "motor.h"
#include "sensor.h"
void usart_send_str(UART_HandleTypeDef *huart,unsigned char *Data) {
//	while(*Data){//判断Data指向的char是否为空
//		while(HAL_UART_Transmit_IT(huart, Data, 1)!=HAL_OK){};//等待传输完毕
//			printf("指针（地址）的值为：OX%p\r\n",Data);
//   		printf("变量的值为：%c\r\n",*Data);
//		Data++;//指向下一个char
//	}
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
char get_cmd(void)
{
  uint8_t ch =0;
	//HAL_UART_Receive(&huart1, &ch, 1, 0xffff);//轮询阻塞方式
	while(HAL_UART_Receive_IT(&huart3, &ch, 1)!=HAL_OK)
	{sound_task();};
  return ch;
}
