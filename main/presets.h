#ifndef PRESETS_H_
#define PRESETS_H_

//TIM1 - управление таймером 15
#define TTM_PULSES_THRESHOLD				3200		//Количество импульсов для того чтобы сменить кадр

//TIM2 - управление яркостью подсветки - RGBY_PRESET/RGBY_PWM_RANGE = скважность ШИМ
#define RGBY_PSC										1				//Делитель(если менять делитель, будет меняться цветовая температура!)
#define RGBY_PWM_RANGE							768			//Значение для 100% ШИМ
#define RGBY_PRESET									128			//Предварительно установленное значение, 25% ШИМ
#define RGBY_PWM_MUL								1				//Множитель вводимого пользователем значения - ввод * RGBY_PWM_MUL

//TIM3 - опрос датчиков натяжения и датчика камеры
#define TIM3_PSC										24000		//Делитель до 1 микросекунды
#define TIM3_ARR										1				//Длительность паузы между прерываниями в миллисекундах

//TIM4 - управление светодиодами и экраном BA63
#define TIM4_PSC										24000		//Делитель до 1 микросекунды
#define TIM4_ARR										93			//Длительность паузы между прерываниями в миллисекундах

//TIM6 - генерация миллисекундной задержки
#define TIM6_PSC										24000		//Делитель до 1 миллисекунды

//TIM7 - опрос кнопок и переключателей
#define TIM7_PSC										24000		//Делитель до 1 миллисекунды
#define TIM7_ARR										21			//Длительность паузы между прерываниями в миллисекундах

//TIM15 - управление скоростью лпм
#define TIM15_PSC										24			//Делитель до 1 микросекунды
#define TIM15_PWM_MAX								1400		//Значение для 100% ШИМ, но это формально, ШИМ всегда будет 50%

//TIM16 - управление скоростью принимающей бобины
#define TIM16_PSC										24			//Делитель до 1 микросекунды
#define TIM16_PWM_MAX								1400		//Значение для 100% ШИМ, но это формально, ШИМ всегда будет 50%

//TIM17 - управление скоростью подающей бобины
#define TIM17_PSC										24			//Делитель до 1 микросекунды
#define TIM17_PWM_MAX								1400		//Значение для 100% ШИМ, но это формально, ШИМ всегда будет 50%

#define DRIVER_DEFAULT_ARR					1400		//Значение для 100% ШИМ
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

#define FEED_COIL_START_THRESHOLD		1800		//Порог после которого двигатель запускается
#define TAKE_COIL_START_THRESHOLD		720

#define FEED_COIL_STOP_THRESHOLD		1920		//Порог для остановки двигателя
#define TAKE_COIL_STOP_THRESHOLD		1166

#define FEED_COIL										1
#define TAKE_COIL										0

#define FILM_DIRECTION_DEFAULT			0
#define FILM_DIRECTION_REVERSE			1

#define COILS_ENGINE_ENABLE					1
#define COILS_ENGINE_DISABLE				0

#define TTM_ENGINE_ENABLE						1
#define TTM_ENGINE_DISABLE					0

#define ENGINE_COOLER_ENABLE				1
#define ENGINE_COOLER_DISABLE				0

#define TTM_TARGET_SPEED						80			//Максимальная скорость двигателя привода ЛПМ
#define TTM_SPEED_DV								18			//Крутизна графика ускорения и замедления

#define THRESHOLD_LOW								40			//Пороги
#define THRESHOLD_MED								80
#define THRESHOLD_HIG								120

#define TTM_START_SPEED							500			//Начальная скорость двигателя привода ЛПМ
#define TTM_SLOWDOWN_THRESHOLD			2900		//Порог импульсов после которого двигатель ЛПМ будет замедляться
#define TTM_CORRECTION_SPEED				450			//Скорость во время коррекции кадра
#define TTM_CORRECTION_SPEED_HIGH		180			//Высокая скорость

#define CALLBACK_THRESHOLD_EN				1400		//Порог переключения флага занятой камеры в 1
#define CALLBACK_THRESHOLD_DIS			800			//Порог переключения флага занятой камеры в 0

#define RED_MAX											1590		//Максимумы, для ограничения по току
#define GREEN_MAX										773
#define BLUE_MAX										1034
#define YELLOW_MAX									1500

#define RED_MIN											54			//Минимумы
#define	GREEN_MIN										5
#define BLUE_MIN										10
#define YELLOW_MIN									24

#endif
