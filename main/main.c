#include "stm32f10x.h"                  // Device header
#include "main.h"
#include "BA63.h"												//Основной экран отображения
#include "TIM1.h"												//Таймер, останавливающий двигатель по достижении нужного количества импульсов
#include "TIM3.h"												//Таймер опроса различных АНАЛоговых датчиков
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
#include "global_vars.h"

void welcome(void);

int main(void)
{
	ba63_fifo.remain = 223;
	ba63_fifo.size = 224;
	ba63_fifo.tx_en = 0;
	ba63_fifo.stop = 0;
	
	//Начинаем настройку железок
	setup_gpio();
	setup_usart();
	
	setup_sensor_poll();
	setup_led_screen_update();
	
	setup_delay_ms();
	setup_key_poller();
	
	setup_ttm();
	setup_ttm_controller();
	setup_feed_coil();
	setup_take_coil();
	
	NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0);
	NVIC_SetPriority(TIM2_IRQn, 1);
	NVIC_SetPriority(TIM3_IRQn, 1);
	NVIC_SetPriority(TIM4_IRQn, 1);
	NVIC_SetPriority(TIM7_IRQn, 1);
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настраиваем таймеры
	
	//Врубаем тактирование
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
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
	
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_ADC1EN, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1EN, ENABLE);
	
	ADC_InitTypeDef m_adc;
	DMA_InitTypeDef m_dma;
	
	ADC_StructInit(&m_adc);
	DMA_StructInit(&m_dma);
	
	m_dma.DMA_BufferSize = 4;
	m_dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	m_dma.DMA_M2M = DMA_M2M_Disable;
	m_dma.DMA_MemoryBaseAddr = (uint32_t)&adc_buffer;
	m_dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	m_dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	m_dma.DMA_Mode = DMA_Mode_Circular;
	m_dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	m_dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	m_dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	m_dma.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel1, &m_dma);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	m_adc.ADC_ContinuousConvMode = ENABLE;
	m_adc.ADC_DataAlign = ADC_DataAlign_Right;
	m_adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	m_adc.ADC_Mode = ADC_Mode_Independent;
	m_adc.ADC_NbrOfChannel = 4;
	m_adc.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &m_adc);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 4, ADC_SampleTime_239Cycles5);
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_TempSensorVrefintCmd(ENABLE);
	
	
	//Калибруемся, в перерывах между калибровками пинаем хуи
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1))
	{
		//NOP
	}
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1))
	{
		//NOP
	}
	
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Запускаемся
	TIM_Cmd(TIM2, ENABLE);
	USART_Cmd(USART1, ENABLE);
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	
		
	//Запуски
	start_key_poller();
	
	start_sensor_poll();
	start_led_screen_update();
	
	start_ttm_controller();
	
	set_speed_ttm(800);
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
	BA63_SendString(device_name_frame1, sizeof(device_name_frame1));
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame2, sizeof(device_name_frame2));
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame3, sizeof(device_name_frame3));
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame4, sizeof(device_name_frame4));
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame5, sizeof(device_name_frame5));
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame6, sizeof(device_name_frame6));
	
	//В процессе разработки мне регулярно снилась одна милфа(SR) так что её имя будет кодовым для проекта
	BA63_SetPos(0,1);
	BA63_SendString(projectCode, sizeof(projectCode));
	
	delay_ms(950);
	
	//Сворачиваем заголовок
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame5, sizeof(device_name_frame5));
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame4, sizeof(device_name_frame4));
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame3, sizeof(device_name_frame3));
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame2, sizeof(device_name_frame2));
	delay_ms(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame1, sizeof(device_name_frame1));
	
	delay_ms(300);
	BA63_SetPos(0,0);
	BA63_DeleteToEndline();
	BA63_SetPos(0,1);
	BA63_SendString(fwVer, sizeof(fwVer));
	delay_ms(1250);
	BA63_SetPos(0,1);
	BA63_SendString(author, sizeof(author));
	delay_ms(1250);
	
	//Приветствие завершено
}