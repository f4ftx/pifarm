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

#ifndef PIFARM_LOGIC_H
#define PIFARM_LOGIC_H     1

#define _XOPEN_SOURCE
#include <time.h>
#include <sys/time.h>

#include "pifarm_commons.h"
#include "pifarm_gpio.h"

/* Logic command functions */
void shutdown_all_relays(void)                                   ;
void command_function(const char* function, uint8_t status) ;
void command_light(uint8_t status);
void command_watering(uint8_t status);
void command_fan(uint8_t status);
void command_heating(uint8_t status);
void auto_program(void);

#endif
