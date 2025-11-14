#ifndef TIM3_H_
#define TIM3_H_

void setup_sensor_poll(void);
void start_sensor_poll(void);
void stop_sensor_poll(void);
void TIM3_IRQHandler(void);

#endif