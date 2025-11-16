#ifndef PRESETS_H_
#define PRESETS_H_

//TIM1 - управление таймером 15
#define TTM_PULSES_THRESHOLD				3200		//Количество импульсов для того чтобы сменить кадр

//TIM2 - управление яркостью подсветки - RGBY_PRESET/RGBY_PWM_RANGE = скважность ШИМ
#define RGBY_PSC										24			//Делитель до 1 микросекунды
#define RGBY_PWM_RANGE							1024		//Значение для 100% ШИМ
#define RGBY_PRESET									256			//Предварительно установленное значение, 25% ШИМ
#define RGBY_PWM_MUL								3				//Множитель вводимого пользователем значения - ввод * RGBY_PWM_MUL

//TIM3 - опрос датчиков натяжения и датчика камеры
#define TIM3_PSC										24000		//Делитель до 1 микросекунды
#define TIM3_ARR										1				//Длительность паузы между прерываниями в микросекундах

//TIM4 - управление светодиодами и экраном BA63
#define TIM4_PSC										24000		//Делитель до 1 микросекунды
#define TIM4_ARR										100			//Длительность паузы между прерываниями в микросекундах

//TIM6 - генерация миллисекундной задержки
#define TIM6_PSC										24000		//Делитель до 1 миллисекунды

//TIM7 - опрос кнопок и переключателей
#define TIM7_PSC										24000		//Делитель до 1 микросекунды
#define TIM7_ARR										21			//Длительность паузы между прерываниями в микросекундах

//TIM15 - управление скоростью лпм
#define TIM15_PSC										24			//Делитель до 1 микросекунды
#define TIM15_PWM_MAX								1400		//Значение для 100% ШИМ, но это формально, ШИМ всегда будет 50%

//TIM16 - управление скоростью принимающей бобины
#define TIM16_PSC										24			//Делитель до 1 микросекунды
#define TIM16_PWM_MAX								1400		//Значение для 100% ШИМ, но это формально, ШИМ всегда будет 50%

//TIM17 - управление скоростью подающей бобины
#define TIM17_PSC										24			//Делитель до 1 микросекунды
#define TIM17_PWM_MAX								1400		//Значение для 100% ШИМ, но это формально, ШИМ всегда будет 50%

#define DRIVER_DEFAULT_PWM					700			//Значение для 50% ШИМ

#define TIM1_IRQ_PRIORITY						0				//Приоритет таймера отслеживающий количество импульсов
#define TIM3_IRQ_PRIORITY						2				//Приоритет таймера опроса датчиков
#define TIM4_IRQ_PRIORITY						4				//Приоритет таймера управления светодиодами и индикацией в целом
#define TIM6_IRQ_PRIORITY						3				//Приоритет задержки, хз зачем пусть будет
#define TIM7_IRQ_PRIORITY						3				//Приоритет таймера опроса кнопок
#define USART_IRQ_PRIORITY					6				//Приоритет передатчика инфы на экран отображения

#define TIM1_IRQ_SUBPRIORITY				1				//Хз пусть будет
#define TIM3_IRQ_SUBPRIORITY				6				
#define TIM4_IRQ_SUBPRIORITY				6				
#define TIM6_IRQ_SUBPRIORITY				6				
#define TIM7_IRQ_SUBPRIORITY				6				
#define USART_IRQ_SUBPRIORITY				9				

#define COIL_AA_SAMPLES							16			//Количество показаний из которых будет посчитано среднее - сглаженное значение

#define FEED_COIL_START_THRESHOLD		820			//Порог после которого двигатель запускается
#define TAKE_COIL_START_THRESHOLD		720			//Порог для остановки двигателя

#define FEED_COIL_STOP_THRESHOLD		700
#define TAKE_COIL_STOP_THRESHOLD		600

#define FEED_COIL										1
#define TAKE_COIL										0

#endif
