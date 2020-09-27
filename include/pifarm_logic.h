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
