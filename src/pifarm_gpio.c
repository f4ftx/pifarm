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

#include "pifarm_gpio.h"

extern context_t     *      p_ctx ;

/* GPIO init function */
uint8_t gpio_init(void)
{
    if (!bcm2835_init())
    {
        return 1;
    }
    return 0 ;
}

/* GPIO pin as output function */
void gpio_setup_pin_as_output(uint8_t * pin)
{
    bcm2835_gpio_fsel(*pin, BCM2835_GPIO_FSEL_OUTP);
}

/* 3.3V to pin*/
void gpio_set_pin(uint8_t * relay_id, uint8_t * pin)
{
    /* set pin to low */
#ifdef DBG_ELECTRONIC
    printf ("DBG [ELECTRONIC]    - Setting relay #%02d (GPIO pin #%02d) to LOW\n",*relay_id, *pin);
#endif
    bcm2835_gpio_write(*pin, LOW);
    /* register state in context */
    p_ctx->relay_status[*relay_id] = RELAY_STATUS_ON ;
}

/* 0V to pin */
void gpio_clr_pin(uint8_t * relay_id, uint8_t * pin)
{
    /* set pin to high */
#ifdef DBG_ELECTRONIC
    printf ("DBG [ELECTRONIC]    - Setting relay #%02d (GPIO pin #%02d) to HIGH\n",*relay_id, *pin);
#endif
    bcm2835_gpio_write(*pin, HIGH);
    /* register state in context */
    p_ctx->relay_status[*relay_id] = RELAY_STATUS_OFF ;
}

/* All relay involved gpio ports set as output */
void setup_gpio_relay_ports(void)
{
    uint8_t relay_id ;

    for (relay_id = 0 ; relay_id < RELAY_NB ; relay_id++ )
    {
#ifdef DBG_ELECTRONIC
    printf ("DBG [ELECTRONIC]    - Setting up GPIO pin #%02d as output (relay #%2d)\n",p_ctx->cfg->relay_gpio[relay_id], relay_id );
#endif
        gpio_setup_pin_as_output(&p_ctx->cfg->relay_gpio[relay_id] );
    }
}
