#ifndef TIM4_H_
#define TIM4_H_

void setup_led_screen_update(void);
void start_led_screen_update(void);
void stop_led_screen_update(void);
void TIM4_IRQHandler(void);
void update_screen(void);

#endif