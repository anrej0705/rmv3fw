#ifndef USART_H_
#define USART_H_

#define SPEED		9600

void setup_usart(void);
void USART1_IRQHandler(void);
void usart_start_transmit(void);

#endif
