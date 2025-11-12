#include "stm32f10x.h"                  // Device header
#include "main.h"
#include "BA63.h"												//Основной экран отображения
#include "TIM1.h"												//Таймер, останавливающий двигатель по достижении нужного количества импульсов
#include "TIM4.h"												//Управление светодиодами и экраном
#include "TIM6.h"												//Миллисекундная задержка
#include "TIM7.h"												//Опрос кнопок
#include "TIM15.h"											//Таймер управления приводом ЛПМ
#include "TIM16.h"											//Таймер управления принимающей бобиной
#include "TIM17.h"											//Таймер управления подающей бобины
#include "USART.h"											//Управление экраном отображения
#include "GPIO.h"												//Порты и ноги и проч проч проч

#include "presets.h"
#include "locale_ru.h"

void welcome(void);

int main(void)
{
	//Начинаем настройку железок
	setup_gpio();
	setup_usart();
	
	setup_led_screen_update();
	
	setup_delay_ms();
	setup_key_poller();
	
	setup_ttm();
	setup_ttm_controller();
	setup_feed_coil();
	setup_take_coil();
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настраиваем таймеры
	
	//Врубаем тактирование
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//Настраиваем таймеры
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	//Это остаётся везде одинаоково
	m_tim.TIM_ClockDivision = 0;
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;
	m_tim.TIM_RepetitionCounter = 0;
	
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
	//Запускаемся
	//TIM_Cmd(TIM1, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
	USART_Cmd(USART1, ENABLE);
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		
	//Запуски
	start_key_poller();
	
	start_led_screen_update();
	
	start_ttm();
	start_ttm_controller();
	start_feed_coil();
	start_take_coil();
	
	set_speed_ttm(200);
	set_speed_feed_coil(800);
	set_speed_take_coil(800);
		
	//Ждём пока экран раскочегарится, нагреется крч приведёт себя в готовность
	delay_ms(366);																															//Ждём пока всё загрузится и просрётся
	BA63_Init();																																//Чистим экран от мусора
	welcome();																																	//Здороваемся с челом
	
	//Цiкл в конце обязателен, если конечно хочется чтобы прерывания работали
	while(1)
	{
		//NOP
	}
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