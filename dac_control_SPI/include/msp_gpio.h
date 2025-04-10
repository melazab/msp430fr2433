#ifndef MSP_GPIO_H
#define MSP_GPIO_H

#define LED_PORT P1OUT
#define LED_PIN BIT0
#define LED_ON() (LED_PORT |= LED_PIN)
#define LED_OFF() (LED_PORT &= ~LED_PIN)
#define LED_TOGGLE() (LED_PORT ^= LED_PIN)

void init_gpio(void);

#endif
