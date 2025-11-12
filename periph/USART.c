#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "USART.h"

void setup_usart(void)
{
	//Начинаем дрочение
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	USART_InitTypeDef m_usart;
	USART_StructInit(&m_usart);
	
	m_usart.USART_BaudRate = SPEED;																					//9600 хуятин в наносек
	m_usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//Похуй + похуй
	m_usart.USART_Mode = USART_Mode_Tx;																			//Просто отправляем и всё
	m_usart.USART_Parity = USART_Parity_No;																	//Без чётности
	m_usart.USART_StopBits = USART_StopBits_1;															//9600 8N1
	m_usart.USART_WordLength = USART_WordLength_8b;													//Длина слова байт как у всех нормальных людей
	
	USART_Init(USART1, &m_usart);
}
