#include "key_convert.h"

uint8_t convert_key(uint8_t key_code)
{
	//Конвертируем кнопку в цифру, крч говоря если нажата кнопка "1" - нужно получить цифру 1
	
	switch(key_code)
	{
		case 1:
		{
			return 1;
		}
		case 2:
		{
			return 2;
		}
		case 3:
		{
			return 3;
		}
		case 5:
		{
			return 4;
		}
		case 6:
		{
			return 5;
		}
		case 7:
		{
			return 6;
		}
		case 9:
		{
			return 7;
		}
		case 10:
		{
			return 8;
		}
		case 11:
		{
			return 9;
		}
		case 14:
		{
			return 0;
		}
		default:
		{	//Если нажата не та кнопка, то возвращаем ошибку
			return 255;
		}
	}
	
}
