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

#ifndef PIFARM_DEBUG_H
#define PIFARM_DEBUG_H 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* UNCOMMENT TO GET TRACES */
#ifdef DEBUG
#define DBG_ASSERTION        1
#define DBG_GUI_EVENTS       1
#define DBG_GUI_CLICK_ZONES  1
#define DBG_CONFIGFILE_PARSE 1
#define DBG_ELECTRONIC       1
#define DBG_LOGIC            1
#endif

/* DEBUG MACROS */
#define  DEBUG_DUMP(a,b,c) debug_dump(a, (char *)b, c)
#define  DEBUG_ASSERT(a)   debug_assert(__PRETTY_FUNCTION__, __FILE__, __LINE__, __STRING(a), a)

/* PROTOTYPES */
void debug_assert(const char *p_fct_name, const char *p_filename, int line, const char *assertion, int res) ;
void debug_dump(char *p_info, char *p_mem, uint32_t lenght);

#endif
