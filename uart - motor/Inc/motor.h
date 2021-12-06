#include "usart.h"
#include "stdio.h"
#include "string.h"
void usart_send_str(UART_HandleTypeDef *huart,unsigned char *Data);
void motor_init(void);
void motor_set(int car_left, int car_right);
