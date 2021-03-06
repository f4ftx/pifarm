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

#include "pifarm_logic.h"

extern context_t     *      p_ctx ;

/* All relay remotes set to 0V */
void shutdown_all_relays(void)
{
    uint8_t relay_id ;

    for (relay_id = 0 ; relay_id < RELAY_NB ; relay_id++ )
    {
        gpio_clr_pin(&relay_id, &p_ctx->cfg->relay_gpio[relay_id]);
    }
}

void command_function(const char* function, uint8_t status)
{
    if (strcmp(function, "LED_DRIVER"  ) == 0 ) command_light(status)   ;
    if (strcmp(function, "FAN_DRIVER"  ) == 0 ) command_fan(status)     ;
    if (strcmp(function, "PUMP_DRIVER" ) == 0 ) command_watering(status);
    if (strcmp(function, "HEAT_DRIVER" ) == 0 ) command_heating(status) ;
}

void command_light(uint8_t status)
{
    uint8_t id ;

    for (id = 0 ; id < RELAY_NB ; id ++ )
    {
        if ( strcmp(p_ctx->cfg->relay_usage[id], "LED_DRIVER") == 0 )
        {
            if ( status == ON )
            {
                gpio_set_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->actuators->light_status = status ;
#ifdef DBG_LOGIC
                DEBUG_PRINT("[LOGIC EVENT]   - Setting p_ctx->actuators->light_status to %d\n", p_ctx->actuators->light_status);
#endif
            }
            else
            {
                gpio_clr_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->actuators->light_status = status ;
#ifdef DBG_LOGIC
                DEBUG_PRINT("[LOGIC EVENT]   - Setting p_ctx->actuators->light_status to %d\n", p_ctx->actuators->light_status);
#endif
            }
        }
    }
}

void command_watering(uint8_t status)
{
    uint8_t id ;

    for (id = 0 ; id < RELAY_NB ; id ++ )
    {
        if ( strcmp(p_ctx->cfg->relay_usage[id], "PUMP_DRIVER") == 0 )
        {
            if ( status == ON )
            {
                gpio_set_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->actuators->water_status = status ;
#ifdef DBG_LOGIC
                DEBUG_PRINT("[LOGIC EVENT]   - Setting p_ctx->actuators->water_status to %d\n", p_ctx->actuators->water_status);
#endif
            }
            else
            {
                gpio_clr_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->actuators->water_status = status ;
#ifdef DBG_LOGIC
                DEBUG_PRINT("[LOGIC EVENT]   - Setting p_ctx->actuators->water_status to %d\n", p_ctx->actuators->water_status);
#endif
            }
        }
    }
}

void command_fan(uint8_t status)
{
    uint8_t id ;

    for (id = 0 ; id < RELAY_NB ; id ++ )
    {
        if ( strcmp(p_ctx->cfg->relay_usage[id], "FAN_DRIVER") == 0 )
        {
            if ( status == ON )
            {
                gpio_set_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->actuators->fan_status = status ;
#ifdef DBG_LOGIC
                DEBUG_PRINT("[LOGIC EVENT]   - Setting p_ctx->actuators->fan_status to %d\n", p_ctx->actuators->fan_status);
#endif
            }
            else
            {
                gpio_clr_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->actuators->fan_status = status ;
#ifdef DBG_LOGIC
                DEBUG_PRINT("[LOGIC EVENT]   - Setting p_ctx->actuators->fan_status to %d\n", p_ctx->actuators->fan_status);
#endif
            }
        }
    }
}

void command_heating(uint8_t status)
{
    uint8_t id ;

    for (id = 0 ; id < RELAY_NB ; id ++ )
    {
        if ( strcmp(p_ctx->cfg->relay_usage[id], "HEAT_DRIVER") == 0 )
        {
            if ( status == ON )
            {
                gpio_set_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->actuators->heat_status = status ;
#ifdef DBG_LOGIC
                DEBUG_PRINT("[LOGIC EVENT]   - Setting p_ctx->actuators->heat_status to %d\n", p_ctx->actuators->heat_status);
#endif
            }
            else
            {
                gpio_clr_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->actuators->heat_status = status ;
#ifdef DBG_LOGIC
                DEBUG_PRINT("[LOGIC EVENT]   - Setting p_ctx->actuators->heat_status to %d\n", p_ctx->actuators->heat_status);
#endif
            }
        }
    }
}

void auto_program(void)
{
    static uint16_t         win_trigger_timeout       = 0     ;
    static uint16_t         win_duration_timeout      = 0     ;
    static uint8_t          win_status                = OFF   ;
    static uint16_t         watering_duration_timeout = 0     ;
    static uint16_t         heater_duration_timeout   = 0     ;
    static uint8_t          watering_task_done_flag   = 0     ;

    static struct tm *      p_config_sunrise_tm   ;
    static struct tm *      p_config_sunset_tm    ;
    static struct tm *      p_config_watering_tm  ;
    struct        tm *      p_now_tm              ;
    struct        timeval   time_now ;

    gettimeofday(&time_now, NULL);
    p_now_tm = localtime(&time_now.tv_sec);

    p_config_sunrise_tm  = (struct tm *) malloc(sizeof(struct tm) ) ;
    DEBUG_ASSERT( p_config_sunrise_tm == NULL);

    p_config_sunset_tm   = (struct tm *) malloc(sizeof(struct tm) ) ;
    DEBUG_ASSERT( p_config_sunset_tm == NULL);

    p_config_watering_tm = (struct tm *) malloc(sizeof(struct tm) ) ;
    DEBUG_ASSERT( p_config_watering_tm == NULL);

    /* keep same timestamp as now */
    memcpy (p_config_sunrise_tm,  p_now_tm, sizeof (struct tm));//*p_now_tm ;
    memcpy (p_config_sunset_tm,   p_now_tm, sizeof (struct tm));//*p_now_tm ;
    memcpy (p_config_watering_tm, p_now_tm, sizeof (struct tm));//*p_now_tm ;

    /* just chang H M */
    strptime(p_ctx->cfg->sunrise_time,  "%H:%M", p_config_sunrise_tm );
    strptime(p_ctx->cfg->sunset_time,   "%H:%M", p_config_sunset_tm  );
    strptime(p_ctx->cfg->watering_time, "%H:%M", p_config_watering_tm);

    /* match interval */
    if ((difftime(mktime(p_config_sunrise_tm), mktime(p_now_tm)) <= 0) &&\
        (difftime(mktime(p_now_tm), mktime(p_config_sunset_tm)) <= 0))
    {
        if ( p_ctx->actuators->light_status == OFF )
        {
            command_light(ON) ;
#ifdef DBG_LOGIC_PRG
            DEBUG_PRINT("[LOGIC PROGRAM] - Sunlight event, lights enabled.\n");
#endif
        }
    }
    else
    {
        if ( p_ctx->actuators->light_status == ON )
        {
            command_light(OFF) ;
#ifdef DBG_LOGIC_PRG
            DEBUG_PRINT("[LOGIC PROGRAM] - Sunset event, lights disabled.\n");
#endif
        }
    }

    /* watering */
    if (p_ctx->actuators->water_status == ON) watering_duration_timeout += 1 ;
    if ((p_ctx->actuators->water_status == OFF) && \
        (p_now_tm->tm_hour == p_config_watering_tm->tm_hour) && \
        (watering_task_done_flag == 0) )
    {
        if (p_now_tm->tm_min == p_config_watering_tm->tm_min)
        {
            if (watering_task_done_flag == 0)
            {
                command_watering(ON) ;
                watering_task_done_flag   = 1 ;
                watering_duration_timeout = 0 ;
#ifdef DBG_LOGIC_PRG
            DEBUG_PRINT("[LOGIC PROGRAM] - Watering task started.\n");
#endif
            }
        }
    }

    if ((p_ctx->actuators->water_status == ON) && \
        (watering_duration_timeout == p_ctx->cfg->watering_duration ))
    {
            command_watering(OFF) ;
            watering_duration_timeout = 0 ;
#ifdef DBG_LOGIC_PRG
            DEBUG_PRINT("[LOGIC PROGRAM] - Watering task timeout reached.\n");
#endif
    }

    /* Task done flag reinit */
    if (p_now_tm->tm_min == p_config_watering_tm->tm_min + 1 )
    {
        watering_task_done_flag = 0 ;
    }

    /* Wind */
    if ( win_status == ON ) win_duration_timeout += 1 ;
    if ( p_ctx->cfg->win_interval == win_trigger_timeout )
    {
        command_fan(ON) ;
        win_status = ON ;
        win_trigger_timeout = 0 ;
#ifdef DBG_LOGIC_PRG
        DEBUG_PRINT("[LOGIC PROGRAM] - Win simulation started.\n");
#endif
    }
    if (( win_status == ON ) &&\
        ( win_duration_timeout == p_ctx->cfg->win_duration ))
    {
        command_fan(OFF) ;
        win_status = OFF ;
        win_duration_timeout = 0 ;
#ifdef DBG_LOGIC_PRG
        DEBUG_PRINT("[LOGIC PROGRAM] - Win timeout reached, fans stopped.\n");
#endif
    }

    /* Heating */
    if ( p_ctx->actuators->heat_status == ON ) heater_duration_timeout += 1 ;
    if ( p_ctx->sensors->temperature < (float)p_ctx->cfg->low_temp_limit)
    {
        command_heating(ON) ;
#ifdef DBG_LOGIC_PRG
        DEBUG_PRINT("[LOGIC PROGRAM] - Cold temperature reached, heater enabled.\n");
#endif
    }


    if (( p_ctx->actuators->heat_status == ON ) &&\
        (heater_duration_timeout == p_ctx->cfg->heater_duration) )
    {
        command_heating(OFF) ;
        heater_duration_timeout = 0 ;
#ifdef DBG_LOGIC_PRG
        DEBUG_PRINT("[LOGIC PROGRAM] - Heater reached timeout, stopping.\n");
#endif
    }

    /* Overheat */
    if ( p_ctx->sensors->temperature > (float)p_ctx->cfg->high_temp_limit)
    {
        command_fan(ON) ;
#ifdef DBG_LOGIC_PRG
        DEBUG_PRINT("[LOGIC PROGRAM] - Overheat detected, starting fans.\n");
#endif
    }
    else
    {
        if ( win_status != ON )
        {
            command_fan(OFF) ;
#ifdef DBG_LOGIC_PRG
            DEBUG_PRINT("[LOGIC PROGRAM] - Overheat event ended, fans stopped.\n");
#endif
        }
    }

    win_trigger_timeout +=1 ;

    free(p_config_sunrise_tm)   ;
    free(p_config_sunset_tm)    ;
    free(p_config_watering_tm)  ;

    p_config_sunrise_tm  = NULL ;
    p_config_sunset_tm   = NULL ;
    p_config_watering_tm = NULL ;

}
