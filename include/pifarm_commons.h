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

#ifndef PIFARM_COMMONS_H
#define PIFARM_COMMONS_H

#include <stdint.h>
#include <time.h>

#include "pifarm_debug.h"

/* ENUMS -------------------------------------------------------------------- */
enum { OFF, ON } ;
enum { AUTO, MANUAL, NONE } ;
enum { STATE_RUNNING, STATE_PAUSED, STATE_STOPPED } ;
enum { RELAY_STATUS_UNKNOWN, RELAY_STATUS_ON, RELAY_STATUS_OFF } ;
enum { UNUSED, LED_DRIVER, FAN_DRIVER, HEAT_DRIVER, PUMP_DRIVER } ;
enum { TYPE_UINT8, TYPE_UINT16, TYPE_UINT32, TYPE_STRING };

/* CONST -------------------------------------------------------------------- */
#define RELAY_NB 16                 /* Using a 16 relays board */
typedef struct
{
    uint8_t  *buf;
    size_t   bufsize;
    uint32_t head;
    uint32_t used;
} rolling_buffer_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
} widget_position_t ;

typedef struct
{
    const char * desc ;
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} click_zone_t ;

typedef struct
{
    click_zone_t * cz_btn_play  ;
    click_zone_t * cz_btn_pause ;
    click_zone_t * cz_btn_stop  ;
    click_zone_t * cz_btn_light ;
    click_zone_t * cz_btn_fan   ;
    click_zone_t * cz_btn_water ;
    click_zone_t * cz_btn_heat  ;
    click_zone_t * cz_btn_tab_1 ;
    click_zone_t * cz_btn_tab_2 ;
    click_zone_t * cz_btn_tab_3 ;
    click_zone_t * cz_btn_tab_4 ;
    uint8_t tab_1 ;
    uint8_t tab_2 ;
    uint8_t tab_3 ;
    uint8_t tab_4 ;
} gfx_context_t ;

/* Config file data structure */
typedef struct
{
   const char * const   key;
   void                 *data;
   uint8_t              type;
} config_syntax_t ;

/* Struct below handle all software configuration  */
typedef struct
{
    char     records_file_path[256]      ;
    uint8_t  relay_gpio[RELAY_NB]        ; /* Corresponding gpio pin => relay id */
    uint8_t  relay_voltage[RELAY_NB]     ; /* U */
    char     relay_usage[RELAY_NB][256]  ;
    uint8_t  high_temp_limit             ; /* °C from cfg */
    uint8_t  low_temp_limit              ; /* °C from cfg */
    uint16_t win_interval                ; /* s */
    uint16_t win_duration                ; /* s */
    char     sunrise_time[256]           ;
    char     sunset_time[256]            ;
    char     watering_time[256]          ;
    uint16_t watering_duration           ;
    uint8_t  wet_limit                   ;
    uint8_t  dry_limit                   ;
} config_t ;

/* sensors status */
typedef struct
{
    float   temperature            ; /* °C  */
    float   pressure               ; /* hPa */
    float   humidity               ; /* %   */
    float   altitude               ; /* m   */
} sensors_t ;

/* actuators status */
typedef struct
{
    uint8_t light_status           ; /* OFF / ON */
    uint8_t water_status           ; /* OFF / ON */
    uint8_t fan_status             ; /* OFF / ON */
    uint8_t heat_status            ; /* OFF / ON */
} actuators_t ;

typedef struct
{
    struct tm  timestamp   ;
    uint8_t    mode        ;
    float      temperature ; /* °C  */
    float      pressure    ; /* hPa */
    float      humidity    ; /* %   */
    float      altitude    ; /* m   */
    uint8_t    light_status; /* OFF / ON */
    uint8_t    water_status; /* OFF / ON */
    uint8_t    fan_status  ; /* OFF / ON */
    uint8_t    heat_status ; /* OFF / ON */
} recording_t ;

/* Struct below handle all runtime context  */
typedef struct
{
    config_t      * cfg            ; /* Pointer to configuration */
    gfx_context_t * gc             ; /* Pointer to graphical context */
    sensors_t     * sensors        ;
    actuators_t   * actuators      ;
    uint8_t relay_status[RELAY_NB] ; /* Relay[n] enum current relay status  */
    uint8_t mode                   ; /* MANUAL / AUTO / NONE */
    uint8_t running_status         ; /* OFF / ON */
} context_t ;

void display_help(void) ;

#endif
