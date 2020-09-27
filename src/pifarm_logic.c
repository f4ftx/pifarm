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
                p_ctx->light_status = status ;
#ifdef DBG_LOGIC
                printf("DBG [LOGIC EVENT]   - Setting p_ctx->light_status to %d\n", p_ctx->light_status);
#endif
            }
            else
            {
                gpio_clr_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->light_status = status ;
#ifdef DBG_LOGIC
                printf("DBG [LOGIC EVENT]   - Setting p_ctx->light_status to %d\n", p_ctx->light_status);
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
                p_ctx->water_status = status ;
#ifdef DBG_LOGIC
                printf("DBG [LOGIC EVENT]   - Setting p_ctx->water_status to %d\n", p_ctx->water_status);
#endif
            }
            else
            {
                gpio_clr_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->water_status = status ;
#ifdef DBG_LOGIC
                printf("DBG [LOGIC EVENT]   - Setting p_ctx->water_status to %d\n", p_ctx->water_status);
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
                p_ctx->fan_status = status ;
#ifdef DBG_LOGIC
                printf("DBG [LOGIC EVENT]   - Setting p_ctx->fan_status to %d\n", p_ctx->fan_status);
#endif
            }
            else
            {
                gpio_clr_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->fan_status = status ;
#ifdef DBG_LOGIC
                printf("DBG [LOGIC EVENT]   - Setting p_ctx->fan_status to %d\n", p_ctx->fan_status);
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
                p_ctx->heat_status = status ;
#ifdef DBG_LOGIC
                printf("DBG [LOGIC EVENT]   - Setting p_ctx->heat_status to %d\n", p_ctx->heat_status);
#endif
            }
            else
            {
                gpio_clr_pin(&id, &p_ctx->cfg->relay_gpio[id]);
                p_ctx->heat_status = status ;
#ifdef DBG_LOGIC
                printf("DBG [LOGIC EVENT]   - Setting p_ctx->heat_status to %d\n", p_ctx->heat_status);
#endif
            }
        }
    }
}

void auto_program(void)
{
    static uint16_t         win_trigger_timeout         = 0     ;
    static uint16_t         win_duration_timeout        = 0     ;
    static uint8_t          win_status                  = OFF   ;
    static uint16_t         watering_duration_timeout   = 0     ;
    static uint8_t          watering_task_done_flag     = 0     ;

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

    strptime(p_ctx->cfg->sunrise_time,  "%H:%M", p_config_sunrise_tm );
    strptime(p_ctx->cfg->sunset_time,   "%H:%M", p_config_sunset_tm  );
    strptime(p_ctx->cfg->watering_time, "%H:%M", p_config_watering_tm);

    /* light */
    if ((p_ctx->light_status == OFF) && \
        (p_now_tm->tm_hour >= p_config_sunrise_tm->tm_hour) && \
        (p_now_tm->tm_hour <= p_config_sunset_tm->tm_hour) )
    {
        if (p_now_tm->tm_min >= p_config_sunrise_tm->tm_min)
        {
            command_light(ON) ;
        }
    }
    if ((p_ctx->light_status == ON) && \
        (p_now_tm->tm_hour >= p_config_sunset_tm->tm_hour))
    {
        if (p_now_tm->tm_min >= p_config_sunset_tm->tm_min)
        {
            command_light(OFF) ;
        }
    }

    /* watering */
    if (p_ctx->water_status == ON) watering_duration_timeout += 1 ;
    if ((p_ctx->water_status == OFF) && \
        (p_now_tm->tm_hour == p_config_watering_tm->tm_hour) && \
        (watering_task_done_flag == 0) )
    {
        if (p_now_tm->tm_min == p_config_watering_tm->tm_min)
        {
            if (watering_task_done_flag == 0)
            {
                printf("--- PUMP ON ---\n");
                command_watering(ON) ;
                watering_task_done_flag   = 1 ;
                watering_duration_timeout = 0 ;
            }
        }
    }

    if ((p_ctx->water_status == ON) && \
        (watering_duration_timeout == p_ctx->cfg->watering_duration ))
    {
            printf("--- PUMP OFF ---\n");
            command_watering(OFF) ;
            watering_duration_timeout = 0 ;
    }
    if (p_now_tm->tm_min == p_config_watering_tm->tm_min + 1 ) watering_task_done_flag = 0 ; 

    /* Wind */
    if ( win_status == ON ) win_duration_timeout += 1 ;
    if ( p_ctx->cfg->win_interval == win_trigger_timeout )
    {
        command_fan(ON) ;
        win_status = ON ;
        win_trigger_timeout = 0 ;
    }
    if (( win_status == ON ) && ( win_duration_timeout == p_ctx->cfg->win_duration ))
    {
        command_fan(OFF) ;
        win_status = OFF ;
        win_duration_timeout = 0 ;
    }

    /* Heating */
    if ( p_ctx->temperature < (float)p_ctx->cfg->low_temp_limit)
    {
        command_heating(ON) ;
    }
    else
    {
        command_heating(OFF) ;
    }

    /* Overheat */
    if ( p_ctx->temperature > (float)p_ctx->cfg->high_temp_limit)
    {
        command_fan(ON) ;
    }
    else
    {
        if ( win_status != ON ) command_fan(OFF) ;
    }

    win_trigger_timeout +=1 ;

    free(p_config_sunrise_tm)   ;
    free(p_config_sunset_tm)    ;
    free(p_config_watering_tm)  ;

    p_config_sunrise_tm  = NULL ;
    p_config_sunset_tm   = NULL ;
    p_config_watering_tm = NULL ;

}
