#include "stm32f10x.h"                  // Device header
#include "stdio.h"

void BA63_SetCP(uint16_t cpCode);
void BA63_ClearVFD(void);
void BA63_SetPos(uint8_t _pX, uint8_t _pY);
void BA63_SendString(uint8_t *string);
void BA63_DeleteToEndline(void);

void delayMs(uint16_t ms);

uint8_t charSet866[256] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07, 0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,  // 0x00...0x0F
  0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17, 0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,  // 0x10...0x1F
  0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27, 0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,  // 0x20...0x2F
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37, 0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,  // 0x30...0x3F
  0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47, 0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,  // 0x40...0x4F
  0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57, 0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,  // 0x50...0x5F
  0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67, 0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,  // 0x60...0x6F
  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77, 0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,  // 0x70...0x7F
  0x89,0x81,0x82,0x83,0x84,0x85,0x86,0x87, 0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,  // 0x80...0x8F
  0xCD,0xCC,0xB9,0xDB,0xFB,0xFE,0xFD,0x00, 0x5E,0x7F,0xDB,0xB0,0x00,0x00,0x00,0x00,  // 0x90...0x9F 0x9A(Ль)->0xDB
  0xB5,0xC6,0x00,0x00,0x00,0x00,0x00,0x00, 0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 0xA0...0xAF
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0xF1,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,  // 0xB0...0xBF
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87, 0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,  // 0xC0...0xCF
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97, 0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,  // 0xD0...0xDF
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7, 0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,  // 0xE0...0xEF
  0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7, 0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,  // 0xF0...0xFF
};

//Команды - VT100
char clearCmd[4] = {"\e[2J"};
char cpcode[3] = {0x1B, 0x52, 0x00};
char setCurPos[6] = {"\e[0;0H"};	//Y затем X
char delEndline[4] = {"\e[0K"};

uint8_t device_name_frame1[21] = "      \313\363\367-1-35      ";
uint8_t device_name_frame2[21] = "    \222 \313\363\367-1-35 \221    ";
uint8_t device_name_frame3[21] = "   \220\222 \313\363\367-1-35 \221\220   ";
uint8_t device_name_frame4[21] = "  \220\220\222 \313\363\367-1-35 \221\220\220  ";
uint8_t device_name_frame5[21] = " \220\220\220\222 \313\363\367-1-35 \221\220\220\220 ";
uint8_t device_name_frame6[21] = "\220\220\220\220\222 \313\363\367-1-35 \221\220\220\220\220";

uint8_t projectCode[21] = "RAIDEN MEI[V3]  35mm";
uint8_t fwVer[21] = "FW: REL_1.0  11.2025";
uint8_t author[21] = "VK:zaz128       2025";
uint8_t del[21] = "                     ";

uint8_t ru_rgb_template[21] = "*R:000*G:000*B:000*\225";
uint8_t ru_color_setup[21] = "\315\340\361\362\360\356\351\352\340 \366\342\345\362\340     ";

int main(void)
{
	//Настройка порта до экрана
	USART_InitTypeDef m_ba63;
	
	//Врубаем тактирование
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN, ENABLE);
	
	//Теперь надо настроить ноги контроллера
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef m_gpio;
	
	//Загрузка настроек по умолчанию
	GPIO_StructInit(&m_gpio);
	
	m_gpio.GPIO_Mode = GPIO_Mode_AF_PP;																		//Выводы подключаем к USART
	m_gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;														//Выводы TX, RX
	m_gpio.GPIO_Speed = GPIO_Speed_2MHz;																	//Без разницы, всё равно экран тормознутый
	
	GPIO_Init(GPIOA, &m_gpio);
	
	m_gpio.GPIO_Mode = GPIO_Mode_Out_PP;																	//Выводы подключаем к USART
	m_gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;														//Выводы TX, RX
	m_gpio.GPIO_Speed = GPIO_Speed_10MHz;																	//Без разницы, всё равно экран тормознутый
	
	GPIO_Init(GPIOC, &m_gpio);
	
	//Предварительная загрузка настроек по умолчанию
	USART_StructInit(&m_ba63);
	
	m_ba63.USART_BaudRate = 9600;																					//9600 бит
	m_ba63.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//Без контроля
	m_ba63.USART_Mode = USART_Mode_Tx;																		//Отправляем в экран
	m_ba63.USART_Parity = USART_Parity_No;																//Без проверки чётности
	m_ba63.USART_StopBits = USART_StopBits_1;															//1 остановочный бит
	m_ba63.USART_WordLength = USART_WordLength_8b;												//Длина слова 8 бит
	USART_Init(USART1, &m_ba63);																					//Применяем настройки
	
	USART_Cmd(USART1, ENABLE);
	
	//Таймер задержки
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);									//Врубаем дрочение таймера
	
	TIM_TimeBaseInitTypeDef m_tim;
	
	TIM_TimeBaseStructInit(&m_tim);
	m_tim.TIM_ClockDivision = 0;																					//Без деления
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;														//Считаем в плюс
	m_tim.TIM_Prescaler = 24000;																					//1 КаГацел
	m_tim.TIM_RepetitionCounter = 0;																			//Без повторов
	
	TIM_TimeBaseInit(TIM6, &m_tim);																				//Применяем настройки
	
	//Ждём пока экран раскочегарится, нагреется крч приведёт себя в готовность
	delayMs(466);
	
	BA63_ClearVFD();																											//Чистим экран
	BA63_SetCP(866);																											//Открываем страницу 866 children porn
	
	//Начинаем разворачивание заголовка
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame1);
	delayMs(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame2);
	delayMs(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame3);
	delayMs(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame4);
	delayMs(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame5);
	delayMs(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame6);
	
	//В процессе разработки мне регулярно снилась одна милфа(SR) так что её имя будет кодовым для проекта
	BA63_SetPos(0,1);
	BA63_SendString(projectCode);
	
	delayMs(950);
	
	//Сворачиваем заголовок
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame5);
	delayMs(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame4);
	delayMs(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame3);
	delayMs(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame2);
	delayMs(50);
	BA63_SetPos(0,0);
	BA63_SendString(device_name_frame1);
	
	delayMs(300);
	BA63_SetPos(0,0);
	BA63_DeleteToEndline();
	BA63_SetPos(0,1);
	BA63_SendString(fwVer);
	delayMs(1250);
	BA63_SetPos(0,1);
	BA63_SendString(author);
	delayMs(1250);
	
	BA63_SetPos(0,0);
	BA63_SendString(ru_rgb_template);
	BA63_SetPos(0,1);
	BA63_SendString(ru_color_setup);
	
	uint32_t i;
	while(1)
	{
		//Нихуя не делаем
		GPIOC->BSRR = GPIO_Pin_9;
		for(i = 0; i < 50000; ++i)
		{
			//NOP
		}
		GPIOC->BSRR = GPIO_Pin_8;
		for(i = 0; i < 50000; ++i)
		{
			//NOP
		}
		GPIOC->BRR = GPIO_Pin_9;
		for(i = 0; i < 50000; ++i)
		{
			//NOP
		}
		GPIOC->BRR = GPIO_Pin_8;
		for(i = 0; i < 50000; ++i)
		{
			//NOP
		}
	}
}

//Задаем кодовую страницу дисплея
void BA63_SetCP(uint16_t cpCode)
{
	switch(cpCode)
	{
		case 866:
		{
			cpcode[2]=0x35;
			for(int i = 0; i < 3; ++i)
			{
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);			//Ждём пока передача не завершится
				USART_SendData(USART1, cpcode[i]);
			}
			break;
		}
		default:
		{
			cpcode[2]=0x01;	//Если ничего не подобралось то ставим пиндостан
			for(int i = 0; i < 3; ++i)
			{
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);			//Ждём пока передача не завершится
				USART_SendData(USART1, cpcode[i]);
			}
			break;
		}
	}
}

//Очистка дисплея
void BA63_ClearVFD(void)
{
	for(int i = 0; i < 4; ++i)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);					//Ждём пока передача не завершится
		USART_SendData(USART1, clearCmd[i]);
	}
}

//Задаём позицию курсора
void BA63_SetPos(uint8_t _pX, uint8_t _pY)
{
	char _X[2] = {'1'};
	char _Y[2] = {'1'};
	sprintf(_X, "%d", _pX+1);
	sprintf(_Y, "%d", _pY+1);
	setCurPos[4] = _X[0];
	setCurPos[2] = _Y[0];
	
	for(int i = 0; i < 6; ++i)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);					//Ждём пока передача не завершится
		USART_SendData(USART1, setCurPos[i]);
	}
}

void BA63_SendString(uint8_t *string)
{
	uint8_t counter=0;
	uint8_t charToSend=0;
	while(string[counter]!=0)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);					//Ждём пока передача не завершится
		charToSend = charSet866[string[counter]];
		USART_SendData(USART1, charToSend);
		counter++;
	}
}

void BA63_DeleteToEndline(void)
{
	for(int i = 0; i < 4; ++i)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);					//Ждём пока передача не завершится
		USART_SendData(USART1, delEndline[i]);
	}
}

void delayMs(uint16_t ms)
{
	uint16_t count = 0;
	TIM6->CNT = 0;																												//Пишем 0 в счётчик
	TIM_Cmd(TIM6, ENABLE);																								//Врубаем таймер
	while(ms > TIM6->CNT);																								//Курим бамбук
	TIM_Cmd(TIM6, DISABLE);																								//Вырубаем таймер и сваливаем
}