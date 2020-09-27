#ifndef PIFARM_COMMONS_H
#define PIFARM_COMMONS_H

#include <stdint.h>

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
} config_t ;


/* Struct below handle all runtime context  */
typedef struct
{
    config_t      * cfg ;            /* Pointer to configuration */
    gfx_context_t * gc ;             /* Pointer to graphical context */
    uint8_t relay_status[RELAY_NB] ; /* Relay[n] enum current relay status  */
    float   temperature            ; /* °C  */
    float   pressure               ; /* hPa */
    float   humidity               ; /* %   */
    float   altitude               ; /* m   */
    uint8_t mode                   ; /* MANUAL / AUTO / NONE */
    uint8_t running_status         ; /* OFF / ON */
    uint8_t light_status           ; /* OFF / ON */
    uint8_t water_status           ; /* OFF / ON */
    uint8_t fan_status             ; /* OFF / ON */
    uint8_t heat_status            ; /* OFF / ON */

} context_t ;

#endif
