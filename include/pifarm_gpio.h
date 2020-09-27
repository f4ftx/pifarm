#ifndef PIFARM_GPIO_H
#define PIFARM_GPIO_H      1

#include "pifarm_commons.h"

#include <bcm2835.h>

/* GPIO + relays specific */
uint8_t gpio_init(void)                       ;
void    gpio_setup_pin_as_output(uint8_t * pin) ;
void    gpio_set_pin(uint8_t * relay_id, uint8_t * pin)       ;
void    gpio_clr_pin(uint8_t * relay_id, uint8_t * pin)       ;
void    setup_gpio_relay_ports(void);

#endif
