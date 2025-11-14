#include "NVIC.h"
#include "presets.h"

void setup_nvic(void)
{
	NVIC_InitTypeDef m_nvic;
	
	//Таймер отслеживающий количество импульсов
	m_nvic.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn;
	m_nvic.NVIC_IRQChannelCmd = ENABLE;
	m_nvic.NVIC_IRQChannelPreemptionPriority = TIM1_IRQ_PRIORITY;
	m_nvic.NVIC_IRQChannelSubPriority = TIM1_IRQ_SUBPRIORITY;
	NVIC_Init(&m_nvic);
	
	//Таймер опроса датчиков
	m_nvic.NVIC_IRQChannel = TIM3_IRQn;
	m_nvic.NVIC_IRQChannelCmd = ENABLE;
	m_nvic.NVIC_IRQChannelPreemptionPriority = TIM3_IRQ_PRIORITY;
	m_nvic.NVIC_IRQChannelSubPriority = TIM3_IRQ_SUBPRIORITY;
	NVIC_Init(&m_nvic);
	
	//Таймер управления светодиодами и индикацией в целом
	m_nvic.NVIC_IRQChannel = TIM4_IRQn;
	m_nvic.NVIC_IRQChannelCmd = ENABLE;
	m_nvic.NVIC_IRQChannelPreemptionPriority = TIM4_IRQ_PRIORITY;
	m_nvic.NVIC_IRQChannelSubPriority = TIM4_IRQ_SUBPRIORITY;
	NVIC_Init(&m_nvic);
	
	//Задержка, хз зачем пусть будет
	m_nvic.NVIC_IRQChannel = TIM6_DAC_IRQn;
	m_nvic.NVIC_IRQChannelCmd = ENABLE;
	m_nvic.NVIC_IRQChannelPreemptionPriority = TIM6_IRQ_PRIORITY;
	m_nvic.NVIC_IRQChannelSubPriority = TIM6_IRQ_SUBPRIORITY;
	NVIC_Init(&m_nvic);
	
	//Таймер опроса кнопок
	m_nvic.NVIC_IRQChannel = TIM7_IRQn;
	m_nvic.NVIC_IRQChannelCmd = ENABLE;
	m_nvic.NVIC_IRQChannelPreemptionPriority = TIM7_IRQ_PRIORITY;
	m_nvic.NVIC_IRQChannelSubPriority = TIM7_IRQ_SUBPRIORITY;
	NVIC_Init(&m_nvic);
	
	//Передатчик инфы на экран отображения
	m_nvic.NVIC_IRQChannel = USART1_IRQn;
	m_nvic.NVIC_IRQChannelCmd = ENABLE;
	m_nvic.NVIC_IRQChannelPreemptionPriority = USART_IRQ_PRIORITY;
	m_nvic.NVIC_IRQChannelSubPriority = USART_IRQ_SUBPRIORITY;
	NVIC_Init(&m_nvic);
	
	//NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0);
	//NVIC_SetPriority(TIM2_IRQn, 1);
	//NVIC_SetPriority(TIM3_IRQn, 1);
	//NVIC_SetPriority(TIM4_IRQn, 1);
	//NVIC_SetPriority(TIM7_IRQn, 1);
}
