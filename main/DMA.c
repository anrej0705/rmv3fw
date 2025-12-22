#include "DMA.h"

#include "stm32f10x_dma.h"
#include "stm32f10x_rcc.h"

#include "global_vars.h"

void setup_dma(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1EN, ENABLE);
	
	DMA_InitTypeDef m_dma;
	DMA_StructInit(&m_dma);
	
	m_dma.DMA_BufferSize = 6;
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
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void DMA1_Channel1_IRQHandler(void)
{
	DMA1->IFCR = DMA1_IT_TC1;															//Сбрасываем флаг
	
	//Небольшое выравнивание
	feed_coil_tension_sensor += adc_buffer[0];
	feed_coil_tension_sensor = feed_coil_tension_sensor / 2;
	
	take_coil_tension_sensor += adc_buffer[1];
	take_coil_tension_sensor = take_coil_tension_sensor / 2;
	
	callback_sensor += adc_buffer[2];
	callback_sensor = callback_sensor / 2;
	
	led_calibration += adc_buffer[3];
	led_calibration = led_calibration / 2;

	cpu_temp_sensor += adc_buffer[4];
	cpu_temp_sensor = cpu_temp_sensor / 2;
	
	feed_coil_tension_sensor >= 2000 ? (feed_coil_tension_sensor = 1999) : (feed_coil_tension_sensor = feed_coil_tension_sensor);
	take_coil_tension_sensor >= 2000 ? (take_coil_tension_sensor = 1999) : (take_coil_tension_sensor = take_coil_tension_sensor);
}
