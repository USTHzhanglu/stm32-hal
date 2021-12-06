#include "motor.h"
void usart_send_str(UART_HandleTypeDef *huart,unsigned char *Data) {
//	while(*Data){//�ж�Dataָ���char�Ƿ�Ϊ��
//		while(HAL_UART_Transmit_IT(huart, Data, 1)!=HAL_OK){};//�ȴ��������
//			printf("ָ�루��ַ����ֵΪ��OX%p\r\n",Data);
//   		printf("������ֵΪ��%c\r\n",*Data);
//		Data++;//ָ����һ��char
//	}
	while(HAL_UART_Transmit_IT(huart, Data,strlen((char *)Data))!=HAL_OK){};
	return;
}
void motor_init(void){
	usart_send_str(&huart3,(unsigned char *)"#255PID001!");
	return;
}

//С�����ƺ���
//���� �����ٶȺ������ٶ� ��Χ -1000 �� 1000
void motor_set(int car_left, int car_right) {
	//�����������
	char cmd_return[100];
	sprintf(cmd_return, "{#001P%04dT0000!#002P%04dT0000!}", (int)(1500+car_left), (int)(1500-car_right));
	usart_send_str(&huart3,(unsigned char *)cmd_return);
	return;
}
