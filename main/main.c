#include "stm32f10x.h"                  // Device header
#include "main.h"
#include "BA63.h"												//Основной экран отображения
#include "TIM6.h"												//Миллисекундная задержка
#include "TIM15.h"											//Таймер управления приводом ЛПМ
#include "TIM16.h"											//Таймер управления принимающей бобиной
#include "TIM17.h"											//Таймер управления подающей бобины

#include "presets.h"
#include "locale_ru.h"

void welcome(void);

int main(void)
{
	//Начинаем настройку железок
	setup_tim6();
	setup_ttm();
	setup_feed_coil();
	setup_take_coil();
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настраиваем таймеры
	
	//Врубаем тактирование
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//Настраиваем таймеры
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	//Это остаётся везде одинаоково
	m_tim.TIM_ClockDivision = 0;
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;
	m_tim.TIM_RepetitionCounter = 0;
	
	//TIM1 - управление таймером 15
	m_tim.TIM_Prescaler = 0;									//Без делителя, считаем всем импульсы
	m_tim.TIM_Period = TTM_PULSES_THRESHOLD;
	TIM_TimeBaseInit(TIM1, &m_tim);
	
	//TIM2 - управление яркостью подсветки - RGBY_PRESET/RGBY_PWM_RANGE = скважность ШИМ
	m_tim.TIM_Prescaler = RGBY_PSC;
	m_tim.TIM_Period = RGBY_PWM_RANGE;
	TIM_TimeBaseInit(TIM2, &m_tim);
	
	//TIM3 - опрос датчиков натяжения
	m_tim.TIM_Prescaler = TIM3_PSC;
	m_tim.TIM_Period = TIM3_ARR;
	TIM_TimeBaseInit(TIM3, &m_tim);
	
	//TIM4 - опрос датчика камеры
	m_tim.TIM_Prescaler = TIM4_PSC;
	m_tim.TIM_Period = TIM4_ARR;
	TIM_TimeBaseInit(TIM4, &m_tim);
	
	//TIM5 - управление светодиодами и экраном BA63
	m_tim.TIM_Prescaler = TIM5_PSC;
	m_tim.TIM_Period = TIM5_ARR;
	TIM_TimeBaseInit(TIM5, &m_tim);
	
	//TIM7 - опрос кнопок и переключателей
	m_tim.TIM_Prescaler = TIM7_PSC;
	m_tim.TIM_Period = TIM7_ARR;
	TIM_TimeBaseInit(TIM7, &m_tim);
	
	//Настраивае ШИМ где надо
	TIM_OCInitTypeDef m_pwm;
	TIM_OCStructInit(&m_pwm);
	
	//Эти настройки будут везде
	m_pwm.TIM_OCMode = TIM_OCMode_PWM1;
	m_pwm.TIM_OutputState = TIM_OutputState_Enable;
	
	//TIM2 - управление яркостью подсветки
	m_pwm.TIM_Pulse = RGBY_PRESET;
	TIM_OC2Init(TIM2, &m_pwm);		//25%
	TIM_OC3Init(TIM2, &m_pwm);		//25%
	TIM_OC4Init(TIM2, &m_pwm);		//25%
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настраиваем АЦП
	
	//Начинаем продрачивание
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настраиваем USART
	
	//Начинаем дрочение
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	USART_InitTypeDef m_usart;
	USART_StructInit(&m_usart);
	
	m_usart.USART_BaudRate = 9600;																					//9600 хуятин в наносек
	m_usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//Похуй + похуй
	m_usart.USART_Mode = USART_Mode_Tx;																			//Просто отправляем и всё
	m_usart.USART_Parity = USART_Parity_No;																	//Без чётности
	m_usart.USART_StopBits = USART_StopBits_1;															//9600 8N1
	m_usart.USART_WordLength = USART_WordLength_8b;													//Длина слова байт как у всех нормальных людей
	
	USART_Init(USART1, &m_usart);
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Запускаемся
	TIM_Cmd(TIM1, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
	TIM_Cmd(TIM6, ENABLE);
	TIM_Cmd(TIM7, ENABLE);
	USART_Cmd(USART1, ENABLE);
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		
	//Запуски
	start_ttm();
	start_feed_coil();
	start_take_coil();
	
	set_speed_ttm(800);
	set_speed_feed_coil(800);
	set_speed_take_coil(800);
		
	//Ждём пока экран раскочегарится, нагреется крч приведёт себя в готовность
	delay_ms(366);																															//Ждём пока всё загрузится и просрётся
	BA63_Init();																																//Чистим экран от мусора
	welcome();																																	//Здороваемся с челом
}

void welcome(void)
{
	//Начинаем разворачивание заголовка
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame1);
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame2);
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame3);
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame4);
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame5);
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame6);
	
	//В процессе разработки мне регулярно снилась одна милфа(SR) так что её имя будет кодовым для проекта
	BA63_SetPos(0,1);
	BA63_SendString(projectCode);
	
	delay_ms(950);
	
	//Сворачиваем заголовок
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame5);
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame4);
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame3);
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame2);
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame1);
	
	delay_ms(300);
	BA63_SetPos(0,0);
	BA63_DeleteToEndline();
	BA63_SetPos(0,1);
	BA63_SendString(fwVer);
	delay_ms(1250);
	BA63_SetPos(0,1);
	BA63_SendString(author);
	delay_ms(1250);
	
	//Приветствие завершено
}