#ifndef TIM7_H_
#define TIM7_H_

void setup_key_poller(void);
void start_key_poller(void);
void stop_key_poller(void);
void TIM7_IRQHandler(void);

#endif
