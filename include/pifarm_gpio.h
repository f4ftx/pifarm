/*
    piFarm
    Copyright (C) 2020 Erwan STIPON (F4FTX)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
