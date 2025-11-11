#ifndef BA63_H_
#define BA63_H_

#include "stdint.h"

//Порт к которому подключён экран
#define USART_PORT	USART1

void BA63_Init(void);												//Настройка экрана перед использованием
void BA63_DeleteToEndline(void);						//Почистить до конца строки
void BA63_SetCP(uint16_t cpCode);						//Задаем кодовую страницу
void BA63_ClearVFD(void);										//Партийная чистка
void BA63_SetPos(uint8_t _pX, uint8_t _pY);	//Задаем позицию курсора
void BA63_SendString(uint8_t *string);			//Шлем строку

#endif
