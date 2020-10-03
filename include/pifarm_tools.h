#ifndef PIFARM_TOOLS_H
#define PIFARM_TOOLS_H  1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pifarm_commons.h"

void   rolling_buffer_create (rolling_buffer_t* rb, size_t bufsz);
void   rolling_buffer_delete (rolling_buffer_t* rb);
size_t rolling_buffer_read (rolling_buffer_t* rb, void *data, size_t sz);
size_t rolling_buffer_pushback (rolling_buffer_t* rb, const void *data, size_t sz);
size_t rolling_buffer_popfront (rolling_buffer_t* rb, void *data, size_t sz);

#endif
