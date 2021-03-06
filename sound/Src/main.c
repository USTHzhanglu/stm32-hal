/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "motor.h"
#include "robot-arm.h"
#include "string.h"
#include "sensor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int fputc(int ch, FILE *stream)    //重写fputc函数
{
 /*
    huart1是工具生成代码定义的UART1结构体，
    如果以后要使用其他串口打印，只需要把这个结构体改成其他UART结构体。
	使用HAL_UART_Transmit时，可以直接调用，使用IT,DMA,VCP方式时，需要对返回值判断，不然会出现只能发送一个字节的情况。
	安全起见，建议全部使用while判断。
*/
	//HAL_UART_Transmit(&huart1, (unsigned char *)&ch, 1,1000);//轮询阻塞方式
	while(HAL_UART_Transmit_IT(&huart1, (unsigned char *)&ch, 1)!=HAL_OK){};//中断方式
    return 1;
}
int fgetc(FILE *f)
{
  uint8_t ch =0;
	//HAL_UART_Receive(&huart1, &ch, 1, 0xffff);//轮询阻塞方式
	while(HAL_UART_Receive_IT(&huart1, &ch, 1)!=HAL_OK){};
  return ch;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	motor_init();
	arm_init();
	HAL_Delay(2000);
	arm_set(arm_begin);
	HAL_GPIO_WritePin(led_GPIO_Port,led_Pin,GPIO_PIN_RESET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  char input;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  input=get_cmd();
	  if(input){
		  switch(input)
		  {	  
			  case '0':get_rad();break;
			  case 'B':
				  usart_send_str(&huart3,(unsigned char *)"car go\r\n");
				  printf("car go\r\n");
				  motor_set(1000,1000);
			  break;
			  case 'C':
				  usart_send_str(&huart3,(unsigned char *)"car back\r\n");
			  printf("car back\r\n");
			  motor_set(-1000,-1000);
			  break;
			  case 'D':
				  usart_send_str(&huart3,(unsigned char *)"car left\r\n");
			  printf("car left\r\n");
			  motor_set(1000,0);
			  break;
			  case 'E':
				  usart_send_str(&huart3,(unsigned char *)"car right\r\n");
			  printf("car right\r\n");
			  motor_set(0,1000);
			  break;			  
			  case 'F':
				  usart_send_str(&huart3,(unsigned char *)"car stop\r\n");
			  printf("car stop\r\n");
			  motor_set(0,0);
			  break;
			  case 'G':
				arm_set(arm_doing);
				HAL_Delay(1000);
				arm_set(arm_clamp);
				HAL_Delay(500);
				int arm_clamped[6];
				memcpy(arm_clamped,arm_clamp,sizeof(arm_clamp));
				arm_clamped[5]+=68;
				arm_set(arm_clamped);
				HAL_Delay(1000);
				usart_send_str(&huart3,(unsigned char *)"夹住物体\r\n");
				memcpy(arm_clamped,arm_doing,sizeof(arm_clamp));
			  	arm_clamped[5]+=68;
				arm_set(arm_clamped);
			  break;
	  }	
	  input=NULL;
  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
