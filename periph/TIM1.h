#ifndef TIM1_H_
#define TIM1_H_

void setup_ttm_controller(void);
void start_ttm_controller(void);
void stop_ttm_controller(void);
void TIM1_UP_TIM16_IRQHandler(void);

#endif