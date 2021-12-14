#include "color.h"	

COLOR_RGBC rgb;
COLOR_HSL  hsl;

/******************************************************************************/
void delay_s(uint32_t i)
{
	while(i--);
}
/******************************************************************************/
void TCS34725_I2C_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = TCS_SDA_Pin|TCS_SCL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, TCS_SDA_Pin|TCS_SCL_Pin, GPIO_PIN_SET);
	
}
/*********************************************/
void TCS34725_I2C_Start()
{
	TCS_SDA_OUT();
	TCS_SDA_H;
	TCS_SCL_H;
	delay_s(40);//delay_us(4);
	TCS_SDA_L;
	delay_s(40);//delay_us(4);
	TCS_SCL_L;
}
/*********************************************/
void TCS34725_I2C_Stop()
{
	TCS_SDA_OUT();
	TCS_SCL_L;
	TCS_SDA_L;
	delay_s(40);//delay_us(4);
	TCS_SCL_H;
	TCS_SDA_H;
	delay_s(40);//delay_us(4);							   	
}
/*********************************************/
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t TCS34725_I2C_Wait_ACK()
{
	uint32_t t=0;
	
	TCS_SDA_IN();//SDA����Ϊ����  
	TCS_SDA_H; 
	delay_s(10);//delay_us(1);
	TCS_SCL_H; 
	delay_s(10);//delay_us(1);
	while(TCS_SDA_READ)
	{
		t++;
		if(t > 250)
		{
			TCS34725_I2C_Stop();
			return 1;
		}
	}
	TCS_SCL_L;
	return 0;	
}
/*********************************************/
//����ACKӦ��
void TCS34725_I2C_ACK()
{
	TCS_SCL_L;
	TCS_SDA_OUT();//sda�����
	TCS_SDA_L;
	delay_s(20);//delay_us(2);
	TCS_SCL_H;
	delay_s(20);//delay_us(2);
	TCS_SCL_L;
}
/*********************************************/
//������ACKӦ��		    
void TCS34725_I2C_NACK()
{
	TCS_SCL_L;
	TCS_SDA_OUT();//sda�����
	TCS_SDA_H;
	delay_s(20);//delay_us(2);
	TCS_SCL_H;
	delay_s(20);//delay_us(2);
	TCS_SCL_L;
}
/*********************************************/
//I2C����һ���ֽ�		  
void TCS34725_I2C_Send_Byte(uint8_t byte)
{
	uint8_t i;
	
	TCS_SDA_OUT();//sda�����
	TCS_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
	for(i = 0; i < 8; i++)
	{
		if(((byte&0x80)>>7)==1)TCS_SDA_H;
		else
			TCS_SDA_L;
		byte <<= 1;
		
		delay_s(20);//delay_us(2);
		TCS_SCL_H;
		delay_s(20);//delay_us(2);
		TCS_SCL_L;
		delay_s(20);//delay_us(2);
	} 
}
/*********************************************/
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t TCS34725_I2C_Read_Byte(uint8_t ack)
{
	uint8_t i,receive = 0;
	
	TCS_SDA_IN();
	for(i = 0; i < 8; i++)
	{
		TCS_SCL_L;
		delay_s(20);//delay_us(2);
		TCS_SCL_H;
		receive <<= 1;
		if(TCS_SDA_READ) receive++;
		delay_s(10);//delay_us(1);
	}
	if (!ack) TCS34725_I2C_NACK();//����nACK
	else TCS34725_I2C_ACK(); //����ACK 
	
	return receive;
}
/*********************************************/
/*******************************************************************************
 * @brief Writes data to a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer - Pointer to a buffer storing the transmission data.
 * @param bytesNumber - Number of bytes to write.
 * @param stopBit - Stop condition control.
 *                  Example: 0 - A stop condition will not be sent;
 *                           1 - A stop condition will be sent.
*******************************************************************************/
void TCS34725_I2C_Write(uint8_t slaveAddress, uint8_t* dataBuffer,uint8_t bytesNumber, uint8_t stopBit)
{
	uint8_t i = 0;
	
	TCS34725_I2C_Start();
	TCS34725_I2C_Send_Byte((slaveAddress << 1) | 0x00);	   //���ʹӻ���ַд����
	TCS34725_I2C_Wait_ACK();
	for(i = 0; i < bytesNumber; i++)
	{
		TCS34725_I2C_Send_Byte(*(dataBuffer + i));
		TCS34725_I2C_Wait_ACK();
	}
	if(stopBit == 1) TCS34725_I2C_Stop();
}
/*******************************************************************************
 * @brief Reads data from a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer - Pointer to a buffer that will store the received data.
 * @param bytesNumber - Number of bytes to read.
 * @param stopBit - Stop condition control.
 *                  Example: 0 - A stop condition will not be sent;
 *                           1 - A stop condition will be sent.
*******************************************************************************/
void TCS34725_I2C_Read(uint8_t slaveAddress, uint8_t* dataBuffer, uint8_t bytesNumber, uint8_t stopBit)
{
	uint8_t i = 0;
	
	TCS34725_I2C_Start();
	TCS34725_I2C_Send_Byte((slaveAddress << 1) | 0x01);	   //���ʹӻ���ַ������
	TCS34725_I2C_Wait_ACK();
	for(i = 0; i < bytesNumber; i++)
	{
		if(i == bytesNumber - 1)
		{
			*(dataBuffer + i) = TCS34725_I2C_Read_Byte(0);//��ȡ�����һ���ֽڷ���NACK
		}
		else
		{
			*(dataBuffer + i) = TCS34725_I2C_Read_Byte(1);
		}
	}
	if(stopBit == 1) TCS34725_I2C_Stop();
}
/*******************************************************************************
 * @brief Writes data into TCS34725 registers, starting from the selected
 *        register address pointer.
 *
 * @param subAddr - The selected register address pointer.
 * @param dataBuffer - Pointer to a buffer storing the transmission data.
 * @param bytesNumber - Number of bytes that will be sent.
 *
 * @return None.
*******************************************************************************/
void TCS34725_Write(uint8_t subAddr, uint8_t* dataBuffer, uint8_t bytesNumber)
{
    uint8_t sendBuffer[10] = {0, };
    uint8_t byte = 0;
    
    sendBuffer[0] = subAddr | TCS34725_COMMAND_BIT;
    for(byte = 1; byte <= bytesNumber; byte++)
    {
        sendBuffer[byte] = dataBuffer[byte - 1];
    }
	TCS34725_I2C_Write(TCS34725_ADDRESS, sendBuffer, bytesNumber + 1, 1);
}
/*******************************************************************************
 * @brief Reads data from TCS34725 registers, starting from the selected
 *        register address pointer.
 *
 * @param subAddr - The selected register address pointer.
 * @param dataBuffer - Pointer to a buffer that will store the received data.
 * @param bytesNumber - Number of bytes that will be read.
 *
 * @return None.
*******************************************************************************/
void TCS34725_Read(uint8_t subAddr, uint8_t* dataBuffer, uint8_t bytesNumber)
{
	subAddr |= TCS34725_COMMAND_BIT;
	
	TCS34725_I2C_Write(TCS34725_ADDRESS, (uint8_t*)&subAddr, 1, 0);
	TCS34725_I2C_Read(TCS34725_ADDRESS, dataBuffer, bytesNumber, 1);
}
/*******************************************************************************
 * @brief TCS34725���û���ʱ��
 *
 * @return None
*******************************************************************************/
void TCS34725_SetIntegrationTime(uint8_t time)
{
	TCS34725_Write(TCS34725_ATIME, &time, 1);
}
/*******************************************************************************
 * @brief TCS34725��������
 *
 * @return None
*******************************************************************************/
void TCS34725_SetGain(uint8_t gain)
{
	TCS34725_Write(TCS34725_CONTROL, &gain, 1);
}
/*******************************************************************************
 * @brief TCS34725ʹ��
 *
 * @return None
*******************************************************************************/
void TCS34725_Enable(void)
{
	uint8_t cmd = TCS34725_ENABLE_PON;
	
	TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
	cmd = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
	TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
	//delay_s(600000);//delay_ms(3);//��ʱӦ�÷�������AEN֮��
}
/*******************************************************************************
 * @brief TCS34725ʧ��
 *
 * @return None
*******************************************************************************/
void TCS34725_Disable(void)
{
	uint8_t cmd = 0;
	
	TCS34725_Read(TCS34725_ENABLE, &cmd, 1);
	cmd = cmd & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
	TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
}

void TCS34725_LedON(uint8_t enable) {
	uint8_t cmd = 0;
	TCS34725_Read(TCS34725_ENABLE, &cmd, 1);
	if(enable) {
		cmd |= TCS34725_ENABLE_AIEN;
	} else {
		cmd &= ~TCS34725_ENABLE_AIEN;
	}
	TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
}

/*******************************************************************************
 * @brief TCS34725��ʼ��
 *
 * @return ID - ID�Ĵ����е�ֵ
*******************************************************************************/
uint8_t TCS34725_Init(uint8_t time,uint8_t gain)
{
	uint8_t id=0;
	uint8_t cmd_return[100];
	TCS34725_I2C_Init(); 
	TCS34725_Read(TCS34725_ID, &id, 1);  //TCS34725 �� ID �� 0x44 ���Ը���������ж��Ƿ�ɹ�����
	sprintf((char *)cmd_return, "{id=%x\r\n}", id);
	usart_send_str(&huart3,cmd_return);
	if(id==0x44)
		{
			TCS34725_SetIntegrationTime(time);
			TCS34725_SetGain(gain);
			TCS34725_Enable();
			return 1;
		}
	return 0;
}
/*******************************************************************************
 * @brief TCS34725��ȡ����ͨ������
 *
 * @return data - ��ͨ����ת��ֵ
*******************************************************************************/
uint16_t TCS34725_GetChannelData(uint8_t reg)
{
	uint8_t tmp[2] = {0,0};
	uint16_t data;
	
	TCS34725_Read(reg, tmp, 2);
	data = (tmp[1] << 8) | tmp[0];
	
	return data;
}
/*******************************************************************************
 * @brief TCS34725��ȡ����ͨ������
 *
 * @return 1 - ת����ɣ����ݿ���
 *   	   0 - ת��δ��ɣ����ݲ�����
*******************************************************************************/
uint8_t TCS34725_GetRawData(COLOR_RGBC *rgbc)
{
	uint8_t status = TCS34725_STATUS_AVALID;
	
	TCS34725_Read(TCS34725_STATUS, &status, 1);
	
	if(status & TCS34725_STATUS_AVALID)
	{
		rgbc->c = TCS34725_GetChannelData(TCS34725_CDATAL);	
		rgbc->r = TCS34725_GetChannelData(TCS34725_RDATAL);	
		rgbc->g = TCS34725_GetChannelData(TCS34725_GDATAL);	
		rgbc->b = TCS34725_GetChannelData(TCS34725_BDATAL);
		return 1;
	}
	return 0;
}

uint16_t TCS34725_GetR(void) {
//	COLOR_RGBC rgbc;
	uint8_t status = TCS34725_STATUS_AVALID;
	
	TCS34725_Read(TCS34725_STATUS, &status, 1);
	
	if(status & TCS34725_STATUS_AVALID){
		return TCS34725_GetChannelData(TCS34725_RDATAL);	
	}
	return 0;
}

uint16_t TCS34725_GetG(void) {
//	COLOR_RGBC rgbc;
	uint8_t status = TCS34725_STATUS_AVALID;
	
	TCS34725_Read(TCS34725_STATUS, &status, 1);
	
	if(status & TCS34725_STATUS_AVALID){
		return TCS34725_GetChannelData(TCS34725_GDATAL);	
	}
	return 0;
}

uint16_t TCS34725_GetB(void) {
//	COLOR_RGBC rgbc;
	uint8_t status = TCS34725_STATUS_AVALID;
	
	TCS34725_Read(TCS34725_STATUS, &status, 1);
	
	if(status & TCS34725_STATUS_AVALID){
		return TCS34725_GetChannelData(TCS34725_BDATAL);
	}
	return 0;
}

uint16_t TCS34725_GetC(void) {
//	COLOR_RGBC rgbc;
	uint8_t status = TCS34725_STATUS_AVALID;
	
	TCS34725_Read(TCS34725_STATUS, &status, 1);
	
	if(status & TCS34725_STATUS_AVALID){
		return TCS34725_GetChannelData(TCS34725_CDATAL);	
	}
	return 0;
}


/******************************************************************************/
//RGBתHSL
void RGBtoHSL(COLOR_RGBC *Rgb, COLOR_HSL *Hsl)
{
	uint8_t maxVal,minVal,difVal;
	uint8_t r = Rgb->r*100/Rgb->c;   //[0-100]
	uint8_t g = Rgb->g*100/Rgb->c;
	uint8_t b = Rgb->b*100/Rgb->c;
	
	maxVal = max3v(r,g,b);
	minVal = min3v(r,g,b);
	difVal = maxVal-minVal;
	
	//��������
	Hsl->l = (maxVal+minVal)/2;
	
	if(maxVal == minVal)//��r=g=b,�Ҷ�
	{
		Hsl->h = 0; 
		Hsl->s = 0;
	}
	else
	{
		//����ɫ��
		if(maxVal==r)
		{
			if(g>=b)
				Hsl->h = 60*(g-b)/difVal;
			else
				Hsl->h = 60*(g-b)/difVal+360;
		}
		else
			{
				if(maxVal==g)Hsl->h = 60*(b-r)/difVal+120;
				else
					if(maxVal==b)Hsl->h = 60*(r-g)/difVal+240;
			}
		
		//���㱥�Ͷ�
		if(Hsl->l<=50)Hsl->s=difVal*100/(maxVal+minVal);  //[0-100]
		else
			Hsl->s=difVal*100/(200-(maxVal+minVal));
	}
}

