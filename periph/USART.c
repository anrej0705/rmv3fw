#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "USART.h"
#include "stdbool.h"
#include "global_vars.h"

bool led_switch1 = 0;

//Есть такая тема - в контроллере баг, когда настраиваешь таймер при запуске он сразу же даёт 
//прерывание которое никто не ждал. Чтобы такой лабуды не было нужно первый вызов прерывания скипать
bool usart_irq_bugfix = 0;

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
	m_usart.USART_StopBits = USART_StopBits_1;															//1 бита хватит для понимания базара
	m_usart.USART_WordLength = USART_WordLength_8b;													//Длина слова байт как у всех нормальных людей
	
	USART_Init(USART1, &m_usart);
	
	SET_BIT(USART1->CR1, USART_CR1_TCIE);
	
	NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler()
{
	if(!usart_irq_bugfix)
	{	//Фиксим ложное срабатывание прерывания
		usart_irq_bugfix = 1;
		USART_ClearITPendingBit(USART1, USART_IT_TC);
		return;
	}
	
	if((USART1->SR & USART_SR_TC) != 0)
	{
		led_switch1 = 1;
		if(!fifo_is_empty())
		{
			led_switch1 = 0;
			
			ba63_fifo.tx_en = 1;
			USART_SendData(USART1, fifo_read());																		//Начинаем педерачу
		}
	}
	
	led_switch1 == 0 ? (GPIOB->ODR &= ~GPIO_Pin_11) : (GPIOB->ODR |= GPIO_Pin_11);
	
	USART_ClearITPendingBit(USART1, USART_IT_TC);
}

void usart_start_transmit()
{
	if(fifo_is_empty() || ba63_fifo.tx_en)
	{
		return;																																//Если буфера пустые, то нам доски не нужны, ну или если идёт активная отправка негров
	}
	
	ba63_fifo.tx_en = 1;
	USART_SendData(USART1, fifo_read());																		//Начинаем педерачу
}
