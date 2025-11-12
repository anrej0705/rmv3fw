#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "GPIO.h"

void setup_gpio(void)
{	
	//Теперь самый сок - порты блять
	
	//Дрочим всё до чего дотянемся - ну тут на самом деле немного
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitTypeDef m_gpio;
	GPIO_StructInit(&m_gpio);
	
	//Эти параметры будут везде
	m_gpio.GPIO_Speed = GPIO_Speed_2MHz;
	
	//Настройка пинов которые будут обычными пинами
	m_gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	
	//GPIOA
	m_gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8 | GPIO_Pin_11;
	GPIO_Init(GPIOA, &m_gpio);
	
	//GPIOB
	m_gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &m_gpio);
	
	//GPIOC
	m_gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &m_gpio);
	
	//GPIOD
	m_gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &m_gpio);
	
	//Настройка блатных чертей
	m_gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	
	//GPIOA
	m_gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &m_gpio);
	
	//GPIOB
	m_gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &m_gpio);
	
	//GPIOC
	m_gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &m_gpio);
}
