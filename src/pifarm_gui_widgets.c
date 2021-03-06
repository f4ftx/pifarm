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

#include "pifarm_gui.h"
#include "pifarm_tools.h"

extern context_t    *p_ctx ;
extern uint8_t       REFRESH_1HZ  ;
extern uint8_t       REFRESH_10HZ ;
extern uint16_t      REFRESH_1MIN ;

void draw_toolchest_widget(Ez_event *ev, widget_position_t * position)
{
    uint8_t scale_factor = 1 ;
    widget_position_t btn_graph_pos ;
    widget_position_t btn_relays_pos ;
    widget_position_t btn_infos_pos ;
    widget_position_t btn_shutdown_pos ;

    /* widget background */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x ,
        position->y ,
        position->x + 110,
        position->y + 255 );

    /* main frame */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_rectangle (
        ev->win,
        position->x + 5 ,
        position->y + 5,
        position->x + 105,
        position->y + 250 );

    /* desc */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle( ev->win, position->x + 10, position->y, position->x + 75, position->y + 24 ) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 15, position->y , "Toolchest");

    btn_graph_pos.x = position->x + 10 ;
    btn_graph_pos.y = position->y + 20 ;
    draw_btn_widget(ev, &scale_factor, &btn_graph_pos, 50, 90) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if ( p_ctx->gc->tab_1 == ON ) ez_set_color(GREEN);
    ez_draw_text (ev->win, EZ_TL, position->x + 25, position->y + 40, "GRAPHICS");

    btn_relays_pos.x = position->x + 10 ;
    btn_relays_pos.y = position->y + 75 ;
    draw_btn_widget(ev, &scale_factor, &btn_relays_pos, 50, 90) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if ( p_ctx->gc->tab_2 == ON ) ez_set_color(GREEN);
    ez_draw_text (ev->win, EZ_TL, position->x + 25, position->y + 95, "ACTUATORS");

    btn_infos_pos.x = position->x + 10 ;
    btn_infos_pos.y = position->y + 130 ;
    draw_btn_widget(ev, &scale_factor, &btn_infos_pos, 50, 90) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if ( p_ctx->gc->tab_3 == ON ) ez_set_color(GREEN);
    ez_draw_text (ev->win, EZ_TL, position->x + 25, position->y + 150, "CONFIG");

    btn_shutdown_pos.x = position->x + 10 ;
    btn_shutdown_pos.y = position->y + 190 ;
    draw_btn_widget(ev, &scale_factor, &btn_shutdown_pos, 50, 90) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    //if ( p_ctx->gc->tab_4 == ON ) ez_set_color(GREEN);
    //ez_draw_text (ev->win, EZ_TL, position->x + 25, position->y + 210, "?");

    /* shutdown icon */
    ez_set_color(RED);
    ez_set_thick(3);
    ez_draw_circle(
        ev->win,
        position->x + 44,
        position->y + 204,
        position->x + 68,
        position->y + 228 ) ;

    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle(
        ev->win,
        position->x + 52,
        position->y + 200,
        position->x + 60,
        position->y + 216);
    ez_set_color(RED);
    ez_draw_line(
        ev->win,
        position->x + 56,
        position->y + 202,
        position->x + 56,
        position->y + 214);
    ez_set_thick(1);
}

void draw_graphs_widget(Ez_event *ev, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( position == NULL );

    static rolling_buffer_t cache_rb ;
    static uint16_t         cmp       = 0 ;
    static uint8_t          init      = 0 ;

    static recording_t      data[500] = {};
    recording_t             sample[1] = {} ;
    register uint16_t       iter ;
    struct tm               now_tm  ;
    struct timeval          time_now    ;
    static char             *msg_dry ;
    static char             *msg_wet ;
    static char             *msg_hot ;
    static char             *msg_cold ;

    if ( init == 0 )
    {
        rolling_buffer_create (&cache_rb, 500 * sizeof(recording_t));
        init = 1 ;
        msg_dry = malloc (10 * sizeof(char)) ;
        msg_wet = malloc (10 * sizeof(char)) ;
        msg_hot = malloc (10 * sizeof(char)) ;
        msg_cold = malloc (10 * sizeof(char)) ;
        sprintf(msg_wet, "%d%%",  p_ctx->cfg->wet_limit) ;
        sprintf(msg_dry, "%d%%",  p_ctx->cfg->dry_limit) ;
        sprintf(msg_hot, "%d\260",  p_ctx->cfg->high_temp_limit) ;
        sprintf(msg_cold, "%d\260", p_ctx->cfg->low_temp_limit);
    }

    if (( REFRESH_1MIN == 1 ) || ( REFRESH_1MIN == 3001 ))
    {
        gettimeofday(&time_now, NULL);
        now_tm = *localtime(&time_now.tv_sec);

        sample[0].timestamp    = now_tm ;
        sample[0].mode         = p_ctx->mode ;
        sample[0].temperature  = p_ctx->sensors->temperature;
        sample[0].pressure     = p_ctx->sensors->pressure;
        sample[0].humidity     = p_ctx->sensors->humidity;
        sample[0].altitude     = p_ctx->sensors->altitude;
        sample[0].light_status = p_ctx->actuators->light_status ;
        sample[0].water_status = p_ctx->actuators->water_status ;
        sample[0].fan_status   = p_ctx->actuators->fan_status ;
        sample[0].heat_status  = p_ctx->actuators->heat_status ;

        rolling_buffer_pushback (&cache_rb, &sample[0], sizeof(recording_t));
        if (cmp >= 500)
        {
            rolling_buffer_popfront (&cache_rb, &sample[0], sizeof(recording_t));
        }
        else
        {
            cmp +=1 ;
        }
    }

    if ( p_ctx->gc->tab_1 == OFF ) return;

    rolling_buffer_read (&cache_rb, &data, 500 * sizeof(recording_t));

    /* LIGHT INDICATOR */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 10, 225 , "Lighting");
    for(iter=0; iter<500; iter++)
    {
        if ( iter % 2 ) ez_draw_point(ev->win, position->x + 10 + iter, 240 ) ;
    }
    ez_set_color(YELLOW) ;
    ez_set_thick(4) ;
    for(iter=0; iter<500; iter++)
    {
        if ((data[iter].light_status == ON ) && (iter != 0))
        {
            ez_draw_point(
                ev->win,
                position->x + 10 + iter,
                240);
        }
    }
    ez_set_thick(1) ;

    /* WATERING INDICATOR */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 10, 245 , "Watering");
    for(iter=0; iter<500; iter++)
    {
        if ( iter % 2 ) ez_draw_point(ev->win, position->x + 10 + iter, 260 ) ;
    }
    ez_set_color(BLUE) ;
    ez_set_thick(4) ;
    for(iter=0; iter<500; iter++)
    {
        if ((data[iter].water_status == ON ) && (iter != 0))
        {
            ez_draw_point(
                ev->win,
                position->x + 10 + iter,
                260);
        }
    }
    ez_set_thick(1) ;

    /* COOLING INDICATOR */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 10, 265 , "Cooling");
    for(iter=0; iter<500; iter++)
    {
        if ( iter % 2 ) ez_draw_point(ev->win, position->x + 10 + iter, 280 ) ;
    }
    ez_set_color(ORANGE) ;
    ez_set_thick(4) ;
    for(iter=0; iter<500; iter++)
    {
        if ((data[iter].fan_status == ON ) && (iter != 0))
        {
            ez_draw_point(
                ev->win,
                position->x + 10 + iter,
                280);
        }
    }
    ez_set_thick(1) ;

    /* HEATER INDICATOR */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 10, 285 , "Heater");
    for(iter=0; iter<500; iter++)
    {
        if ( iter % 2 ) ez_draw_point(ev->win, position->x + 10 + iter, 300 ) ;
    }
    ez_set_color(RED) ;
    ez_set_thick(4) ;
    for(iter=0; iter<500; iter++)
    {
        if ((data[iter].heat_status == ON ) && (iter != 0))
        {
            ez_draw_point(
                ev->win,
                position->x + 10 + iter,
                300);
        }
    }
    ez_set_thick(1) ;

    /* TEMPERATURE GRAPH */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 10, 320 , "Temperature");
    ez_set_color (RED);
    for(iter=0; iter<500; iter++)
    {
        if (((uint8_t)data[iter].temperature != 0 ) && (iter != 0))
        {
            ez_draw_line(
                ev->win,
                position->x + 10 + iter - 1,
                400 - 2 * data[iter-1].temperature,
                position->x + 10 + iter,
                400 - 2 * data[iter].temperature);
        }
    }

    for(iter=0; iter<500; iter++)
    {
        ez_set_color (DEFAULT_FOREGROUND_COLOR);
        if ( iter % 4 ) ez_draw_point(ev->win, position->x + 10 + iter, 400 - 2 * p_ctx->cfg->high_temp_limit ) ;
        if ( iter % 4 ) ez_draw_point(ev->win, position->x + 10 + iter, 400 - 2 * p_ctx->cfg->low_temp_limit ) ;
    }
    ez_draw_text (ev->win, EZ_TL, position->x + 495, 400 - p_ctx->cfg->wet_limit - 12 , msg_hot);
    ez_draw_text (ev->win, EZ_TL, position->x + 495, 400 - p_ctx->cfg->dry_limit , msg_cold);

    /* HUMIDITY GRAPH */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 10, 390 , "Humidity");
    ez_set_color (BLUE);
    for(iter=0; iter<500; iter++)
    {
        if (((uint8_t)data[iter].humidity != 0 ) && (iter != 0))
        {
            ez_draw_line(
                ev->win,
                position->x + 10 + iter,
                470 -  (uint8_t)data[iter - 1].humidity,
                position->x + 10 + iter,
                470 -  (uint8_t)data[iter].humidity ) ;
        }
    }

    for(iter=0; iter<500; iter++)
    {
        ez_set_color (DEFAULT_FOREGROUND_COLOR);
        if ( iter % 4 ) ez_draw_point(ev->win, position->x + 10 + iter, 470 - p_ctx->cfg->wet_limit ) ;
        if ( iter % 4 ) ez_draw_point(ev->win, position->x + 10 + iter, 470 - p_ctx->cfg->dry_limit ) ;
    }
    ez_draw_text (ev->win, EZ_TL, position->x + 495, 470 - p_ctx->cfg->wet_limit - 12 , msg_wet);
    ez_draw_text (ev->win, EZ_TL, position->x + 495, 470 - p_ctx->cfg->dry_limit , msg_dry);

    ez_set_color (DEFAULT_BACKGROUND_COLOR);

}

void draw_relays_widget(Ez_event *ev, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( position == NULL );

    if ( p_ctx->gc->tab_2 == OFF ) return;

    int relay, x, y ;
    char str[10] ;

    /* widget background */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x ,
        position->y ,
        position->x + 505,
        position->y + 255 );

    /* main frame */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_rectangle (
        ev->win,
        position->x + 5 ,
        position->y + 5,
        position->x + 500,
        position->y + 250 );

    /* desc */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle( ev->win, position->x + 10, position->y, position->x + 130, position->y + 24 ) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 15, position->y , "Relay board status");


    x = position->x + 12 ;
    y = position->y + 20 ;
    for (relay=0 ; relay <16 ; relay ++)
    {
        if ( relay < 8 )
        {
            if (relay != 0) x += 60 ;
            y = position->y + 149;
        }

        if ( relay > 8 )
        {
            x -= 60 ;
        }

        if ( relay >= 8 )
        {
            y = position->y + 25 ;
        }

        ez_set_color (DEFAULT_BACKGROUND_COLOR);

        /* frame */
        ez_fill_rectangle (
            ev->win,
            x ,
            y ,
            x + 54,
            y + 44 );

        if ( strcmp (p_ctx->cfg->relay_usage[relay], "UNUSED"      ) == 0 )
        {
            ez_set_color (DEFAULT_FOREGROUND_COLOR);
        }
        else
        {
            ez_set_color (ez_white);
        }

        ez_draw_rectangle (
            ev->win,
            x + 2 ,
            y + 2 ,
            x + 52,
            y + 82 );

        /* STR : V */
        sprintf(str, "%3dV", p_ctx->cfg->relay_voltage[relay] ) ;
        ez_draw_text (ev->win, EZ_TL, x + 25, y + 4, str);

        /* STR : ID */
        sprintf(str, "R #%02d", relay) ;
        ez_draw_text (ev->win, EZ_TL, x + 16, y + 32, str);

        /* STR : CFG */
        if      ( strcmp (p_ctx->cfg->relay_usage[relay], "LED_DRIVER"  ) == 0 ) {sprintf(str, "LEDS" ) ;}
        else if ( strcmp (p_ctx->cfg->relay_usage[relay], "FAN_DRIVER"  ) == 0 ) {sprintf(str, "FAN" ) ;}
        else if ( strcmp (p_ctx->cfg->relay_usage[relay], "HEAT_DRIVER" ) == 0 ) {sprintf(str, "HEAT" ) ;}
        else if ( strcmp (p_ctx->cfg->relay_usage[relay], "PUMP_DRIVER" ) == 0 ) {sprintf(str, "PUMP" ) ;}
        else if ( strcmp (p_ctx->cfg->relay_usage[relay], "UNUSED"      ) == 0 ) {sprintf(str, "----" ) ;}
        ez_draw_text (ev->win, EZ_TL, x + 16, y + 60, str);

        ez_set_color (ez_black);
        ez_draw_rectangle (
            ev->win,
            x + 6,
            y + 6,
            x + 24,
            y + 12 );

        /* led color */
        if ( p_ctx->relay_status[relay] == RELAY_STATUS_UNKNOWN ) ez_set_color(ez_red);
        if ( p_ctx->relay_status[relay] == RELAY_STATUS_ON )      ez_set_color(RED);
        if ( p_ctx->relay_status[relay] == RELAY_STATUS_OFF )     ez_set_color(DEFAULT_FOREGROUND_COLOR);

        ez_fill_rectangle (
            ev->win,
            x + 7,
            y + 7,
            x + 23,
            y + 11 );
    }
}

void reverse(char* str, uint8_t len)
{
    DEBUG_ASSERT( str == NULL );

    int8_t  i    ;
    int8_t  j    ;
    char    temp ;

    i = 0 ;
    j = len - 1 ;
    temp = 0 ;

    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

uint16_t intToStr(uint16_t x, char str[], uint8_t d)
{
    uint8_t i = 0;
    while (x)
    {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    /* If number of digits required is more, then
       add 0s at the beginning */
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

void float_to_string(float n, char* res, uint8_t afterpoint)
{
    DEBUG_ASSERT( res == NULL );

    /* Extract integer part */
    uint16_t ipart = (uint16_t)n;

    /* Extract floating part */
    float fpart = n - (float)ipart;

    /* convert integer part to string */
    uint16_t i = intToStr(ipart, res, 0);

    /* check for display option after point */
    if (afterpoint != 0) {
        res[i] = '.'; /* add dot */

        /* Get the value of fraction part upto given no.
           of points after dot. The third parameter
           is needed to handle cases like 233.007 */
        fpart = fpart * pow(10, afterpoint);

        intToStr((uint16_t)fpart, res + i + 1, afterpoint);
    }
}

void draw_segment(Ez_event *ev, uint8_t * scale_factor, widget_position_t *position, float points[12])
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    ez_set_color (ez_black);
    ez_set_thick (2 * *scale_factor);

    /* seven segments */
    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[0],
        position->y + *scale_factor * points[1],
        position->x + *scale_factor * points[2],
        position->y + *scale_factor * points[3]);

    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[2],
        position->y + *scale_factor * points[3],
        position->x + *scale_factor * points[4],
        position->y + *scale_factor * points[5]);

    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[4],
        position->y + *scale_factor * points[5],
        position->x + *scale_factor * points[6],
        position->y + *scale_factor * points[7]);

    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[6],
        position->y + *scale_factor * points[7],
        position->x + *scale_factor * points[8],
        position->y + *scale_factor * points[9]);

    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[8],
        position->y + *scale_factor * points[9],
        position->x + *scale_factor * points[10],
        position->y + *scale_factor * points[11]);

    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[10],
        position->y + *scale_factor * points[11],
        position->x + *scale_factor * points[0],
        position->y + *scale_factor * points[1]);

    /* dot */
    ez_draw_circle(
        ev->win,
        position->x + *scale_factor * 20,
        position->y + *scale_factor * 32,
        position->x + *scale_factor * 24,
        position->y + *scale_factor * 36);

    ez_set_thick (1);
}

void fill_segment(Ez_event *ev, uint8_t * scale_factor, widget_position_t *position, float points[12], uint8_t dot )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( position == NULL );

    ez_set_color (DEFAULT_ACTIVE_COLOR);
    ez_set_thick (2 * *scale_factor);

    /* seven segments */
    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[0] + 2 * *scale_factor ,
        position->y + *scale_factor * points[1] ,
        position->x + *scale_factor * points[2] ,
        position->y + *scale_factor * points[3] + 2 * *scale_factor );

    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[2] ,
        position->y + *scale_factor * points[3] + 2 * *scale_factor ,
        position->x + *scale_factor * points[4] - 2 * *scale_factor ,
        position->y + *scale_factor * points[5] + 2 * *scale_factor );

    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[4] - 2 * *scale_factor ,
        position->y + *scale_factor * points[5] + 2 * *scale_factor ,
        position->x + *scale_factor * points[6] - 2 * *scale_factor ,
        position->y + *scale_factor * points[7]      );

    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[6] - 2 * *scale_factor ,
        position->y + *scale_factor * points[7]      ,
        position->x + *scale_factor * points[8]      ,
        position->y + *scale_factor * points[9] - 2 * *scale_factor );

    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[8]      ,
        position->y + *scale_factor * points[9]  - 2 * *scale_factor ,
        position->x + *scale_factor * points[10] + 2 * *scale_factor ,
        position->y + *scale_factor * points[11] - 2 * *scale_factor );

    ez_draw_line (
        ev->win,
        position->x + *scale_factor * points[10] + 2 * *scale_factor ,
        position->y + *scale_factor * points[11] - 2 * *scale_factor ,
        position->x + *scale_factor * points[0]  + 2 * *scale_factor ,
        position->y + *scale_factor * points[1]      );

    /* dot */
    if ( dot == 1 )
    {
        ez_draw_circle(
            ev->win,
            position->x + *scale_factor * 21,
            position->y + *scale_factor * 32,
            position->x + *scale_factor * 23,
            position->y + *scale_factor * 36);
    }

    ez_set_thick (1);
}

/* SEVEN SEGMENTS WIDGET
 * ---------------------
 */

void draw_seven_segment_widget(Ez_event *ev, uint8_t*  scale_factor, widget_position_t * position, char data, uint8_t dot)
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    static float segA[12] = {3.0,  5,  5.2,  3,  7.0,  5,  6.0,  15, 3.8,  17, 2.0,  15} ;
    static float segB[12] = {6.3,  2,  8.5,  0,  18.5, 0,  20.3, 2,  18.1, 4 , 8.1,  4 } ;
    static float segC[12] = {19.0, 5,  21.2, 3,  23.0, 5,  22.0, 15, 19.8, 17, 18.0, 15} ;
    static float segD[12] = {17.4, 21, 19.6, 19, 21.4, 21, 20.4, 31, 18.2, 33, 16.4, 31} ;
    static float segE[12] = {3.1,  34, 5.3,  32, 15.3, 32, 17.1, 34, 14.9, 36, 4.9,  36} ;
    static float segF[12] = {1.4,  21, 3.6,  19, 5.4,  21, 4.4,  31, 2.2,  33, 0.4,  31} ;
    static float segG[12] = {4.7,  18, 6.9,  16, 16.9, 16, 18.7, 18, 16.5, 20, 6.5,  20} ;

    /* BACKGROUND */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x - *scale_factor * 2,
        position->y - *scale_factor * 2,
        position->x + *scale_factor * 26,
        position->y + *scale_factor * 38 );

    /* ILUMINATED SEGMENTS */
    switch (data)
    {
        case 0x30 :                      /* 0 */
            fill_segment(ev, scale_factor, position, segA, dot);
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segC, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segF, dot);
            break ;
        case 0x01 :
        case 0x31 :                      /* 1 */
            fill_segment(ev, scale_factor, position, segC, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            break ;
        case 0x02 :
        case 0x32 :                      /* 2 */
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segC, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segF, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x03 :
        case 0x33 :                      /* 3 */
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segC, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x04 :
        case 0x34 :                      /* 4 */
            fill_segment(ev, scale_factor, position, segA, dot);
            fill_segment(ev, scale_factor, position, segC, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x05 :
        case 0x35 :                      /* 5 */
            fill_segment(ev, scale_factor, position, segA, dot);
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x06 :
        case 0x36 :                      /* 6 */
            fill_segment(ev, scale_factor, position, segA, dot);
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segF, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x07 :
        case 0x37 :                      /* 7 */
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segC, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            break ;
        case 0x08 :
        case 0x38 :                      /* 8 */
            fill_segment(ev, scale_factor, position, segA, dot);
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segC, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segF, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x09 :
        case 0x39 :                      /* 9 */
            fill_segment(ev, scale_factor, position, segA, dot);
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segC, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x41 :
        case 0x61 :                      /* A */
            fill_segment(ev, scale_factor, position, segA, dot);
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segC, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            fill_segment(ev, scale_factor, position, segF, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x42 :
        case 0x62 :                      /* B */
            fill_segment(ev, scale_factor, position, segD, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segF, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x43 :
        case 0x63 :                      /* C */
            fill_segment(ev, scale_factor, position, segA, dot);
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segF, dot);
            break ;
        case 0x44 :
        case 0x64 :                      /* D */
            fill_segment(ev, scale_factor, position, segC, dot);
            fill_segment(ev, scale_factor, position, segD, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segF, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x45 :
        case 0x65 :                      /* E */
            fill_segment(ev, scale_factor, position, segA, dot);
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segE, dot);
            fill_segment(ev, scale_factor, position, segF, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x46 :
        case 0x66 :                      /* F */
            fill_segment(ev, scale_factor, position, segA, dot);
            fill_segment(ev, scale_factor, position, segB, dot);
            fill_segment(ev, scale_factor, position, segF, dot);
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case 0x2d :                      /* - */
            fill_segment(ev, scale_factor, position, segG, dot);
            break ;
        case ' '  :
        case 0x00 :
            break ;
        default :
            fill_segment(ev, scale_factor, position, segG, dot);
#ifdef DBG_GUI_WIDGET_LCD
            DEBUG_PRINT("DEBUG GUI 7SEG : data out of range : %2x %c", data, data);
#endif
    } ;

    /* SEGMENTS */
    draw_segment(ev, scale_factor, position, segA);
    draw_segment(ev, scale_factor, position, segB);
    draw_segment(ev, scale_factor, position, segC);
    draw_segment(ev, scale_factor, position, segD);
    draw_segment(ev, scale_factor, position, segE);
    draw_segment(ev, scale_factor, position, segF);
    draw_segment(ev, scale_factor, position, segG);

}

void draw_seven_segment_grid_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t size, char * data )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    uint8_t cmp ;
    uint8_t dot ;

    dot = 0 ;
    for (cmp = 0 ; cmp < size ; cmp++ )
    {

        if ((data[cmp + 1] == '.') || (data[cmp + 1] == ','))
        {
            dot = 1 ;
        }
        else
        {
            dot = 0 ;
        }

        draw_seven_segment_widget(ev, scale_factor, position, data[cmp], dot) ;
        position->x += (*scale_factor * 26) ;
        if (dot == 1){cmp++;}
    }

}

void draw_double_dot_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position)
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    /* BACKGROUND */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x - *scale_factor * 2,
        position->y - *scale_factor * 2,
        position->x + *scale_factor * 16,
        position->y + *scale_factor * 38 );

    /* LED FRAME */
    ez_set_color (ez_black);
    ez_draw_circle(
        ev->win,
        position->x + *scale_factor * 4,
        position->y + *scale_factor * 8,
        position->x + *scale_factor * 10,
        position->y + *scale_factor * 14);

    ez_draw_circle(
        ev->win,
        position->x + *scale_factor * 4,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 10,
        position->y + *scale_factor * 26);

    /* ILLUMINATION */
    if ( REFRESH_1HZ < 50 )
    {
        ez_set_color (DEFAULT_BACKGROUND_COLOR);
    }
    else
    {
        ez_set_color (DEFAULT_ACTIVE_COLOR);
    }

    ez_fill_circle(
        ev->win,
        position->x + *scale_factor * 4,
        position->y + *scale_factor * 8,
        position->x + *scale_factor * 8,
        position->y + *scale_factor * 12);

    ez_fill_circle(
        ev->win,
        position->x + *scale_factor * 4,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 8,
        position->y + *scale_factor * 24);
}

void draw_clock_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    struct timeval time_now;
    struct tm * time_str_tm ;
    char data[3] ;

    /* widget background */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x ,
        position->y ,
        position->x + *scale_factor * 280,
        position->y + *scale_factor * 65 );

    /* main frame */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_rectangle (
        ev->win,
        position->x + *scale_factor * 5 ,
        position->y + *scale_factor * 5,
        position->x + *scale_factor * 275,
        position->y + *scale_factor * 60 );

    /* desc */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle( ev->win, position->x + *scale_factor * 10, position->y, position->x + *scale_factor * 80, position->y + *scale_factor * 24 ) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + *scale_factor * 15, position->y , "Local time");

    /* get current time */
    gettimeofday(&time_now, NULL);
    time_str_tm = localtime(&time_now.tv_sec);

    /* draw segments and separator */
    position->x += *scale_factor * 10 ;
    position->y += *scale_factor * 15 ;
    intToStr(time_str_tm->tm_hour, data, 2);
    draw_seven_segment_grid_widget(ev, scale_factor, position, 2, data ) ;
    draw_double_dot_widget(ev, scale_factor, position);
    position->x += *scale_factor * 15 ;
    intToStr(time_str_tm->tm_min, data, 2);
    draw_seven_segment_grid_widget(ev, scale_factor, position, 2, data ) ;
    draw_double_dot_widget(ev, scale_factor, position);
    position->x += *scale_factor * 15 ;
    intToStr(time_str_tm->tm_sec, data, 2);
    draw_seven_segment_grid_widget(ev, scale_factor, position, 2, data ) ;
    draw_double_dot_widget(ev, scale_factor, position);
    position->x += *scale_factor * 15 ;
    intToStr(time_now.tv_usec, data, 2);
    draw_seven_segment_grid_widget(ev, scale_factor, position, 2, data ) ;

}

void draw_temperature_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t size, char * data )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );
    DEBUG_ASSERT( data == NULL );

    widget_position_t lcd_position ;

    /* widget background */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x ,
        position->y ,
        position->x + 5 * 26 + 10 ,
        position->y +  38 + 25 );

    /* main frame */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_rectangle (
        ev->win,
        position->x + 5 ,
        position->y + 5,
        position->x + 5 + 5 * 26,
        position->y + 5 + 38 + 15 );

    /* desc */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle( ev->win, position->x + 10, position->y, position->x + 115, position->y + 24 ) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 15, position->y , "Temperature (\260C)");

    /* lcd */
    lcd_position.x = position->x + 15 ;
    lcd_position.y = position->y + 15 ;
    draw_seven_segment_grid_widget(
        ev,
        scale_factor,
        &lcd_position,
        size,
        data ) ;

}

void draw_humidity_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t size, char * data )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );
    DEBUG_ASSERT( data == NULL );

    widget_position_t lcd_position ;

    /* widget background */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x ,
        position->y ,
        position->x + 5 * 26 + 10 ,
        position->y +  38 + 25 );

    /* main frame */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_rectangle (
        ev->win,
        position->x + 5 ,
        position->y + 5,
        position->x + 5 + 5 * 26,
        position->y + 5 + 38 + 15 );

    /* desc */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle( ev->win, position->x + 10, position->y, position->x + 90, position->y + 24 ) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 15, position->y , "Humidity (%%)");

    /* lcd */
    lcd_position.x = position->x + 15 ;
    lcd_position.y = position->y + 15 ;
    draw_seven_segment_grid_widget(
        ev,
        scale_factor,
        &lcd_position,
        size,
        data ) ;
}

void draw_pressure_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t size, char * data )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );
    DEBUG_ASSERT( data == NULL );

    widget_position_t lcd_position ;

    /* widget background */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x ,
        position->y ,
        position->x + 5 * 26 + 10 ,
        position->y +  38 + 25 );

    /* main frame */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_rectangle (
        ev->win,
        position->x + 5 ,
        position->y + 5,
        position->x + 5 + 5 * 26 ,
        position->y + 5 + 38 + 15 );

    /* desc */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle( ev->win, position->x + 10, position->y, position->x + 100, position->y + 24 ) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 15, position->y , "Pressure (hPa)");

    /* lcd */
    lcd_position.x = position->x + 15 ;
    lcd_position.y = position->y + 15 ;
    draw_seven_segment_grid_widget(
        ev,
        scale_factor,
        &lcd_position,
        size,
        data ) ;

}

void draw_altitude_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t size, char * data )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );
    DEBUG_ASSERT( data == NULL );

    widget_position_t lcd_position ;

    /* widget background */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x ,
        position->y ,
        position->x + 5 * 26 + 10 ,
        position->y +  38 + 25 );

    /* main frame */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_rectangle (
        ev->win,
        position->x + 5 ,
        position->y + 5,
        position->x + 5 + 5 * 26 ,
        position->y + 5 + 38 + 15 );

    /* desc */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle( ev->win, position->x + 10, position->y, position->x + 90, position->y + 24 ) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + 15, position->y , "Altitude (m)");

    /* lcd */
    lcd_position.x = position->x + 15 ;
    lcd_position.y = position->y + 15 ;
    draw_seven_segment_grid_widget(
        ev,
        scale_factor,
        &lcd_position,
        size,
        data ) ;

}

/* CONTROL PANEL WIDGET */
void draw_btn_widget (Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t height, uint8_t width )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    /* widget background */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x ,
        position->y ,
        position->x + *scale_factor * width,
        position->y + *scale_factor * height );

    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x + *scale_factor * 4 ,
        position->y + *scale_factor * 4,
        position->x + *scale_factor * (width - 8),
        position->y + *scale_factor * (height - 8) );

    ez_set_color (ez_black);
    ez_fill_rectangle (
        ev->win,
        position->x + *scale_factor * 8,
        position->y + *scale_factor * 8,
        position->x + *scale_factor * (width - 2),
        position->y + *scale_factor * (height - 4) );

    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x + *scale_factor * 7,
        position->y + *scale_factor * 7,
        position->x + *scale_factor * (width - 7),
        position->y + *scale_factor * (height - 7) );

}

void draw_play_btn_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    static int status = 0 ;

    status = p_ctx->running_status ;

    draw_btn_widget(ev, scale_factor, position, 75, 150) ;

    /* ICON */
    if (status != STATE_RUNNING)
    {
        ez_set_color (DEFAULT_FOREGROUND_COLOR);
    }
    else
    {
        if ( REFRESH_1HZ < 50 )
        {
            ez_set_color (ez_green);
        }
        else
        {
            ez_set_color (DEFAULT_FOREGROUND_COLOR);
        }
    }

    ez_fill_triangle (
        ev->win,
        position->x + *scale_factor * 57,
        position->y + *scale_factor * 18,
        position->x + *scale_factor * 57,
        position->y + *scale_factor * 58,
        position->x + *scale_factor * 98,
        position->y + *scale_factor * 38 );

}

void draw_pause_btn_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    /* BTN */
    draw_btn_widget(ev, scale_factor, position, 75, 75) ;

    /* ICON */
    if (p_ctx->running_status != STATE_PAUSED)
    {
        ez_set_color (DEFAULT_FOREGROUND_COLOR);
    }
    else
    {
        if ( REFRESH_1HZ < 50 )
        {
            ez_set_color (ORANGE);
        }
        else
        {
            ez_set_color (DEFAULT_FOREGROUND_COLOR);
        }
    }

    ez_fill_rectangle (
        ev->win,
        position->x + *scale_factor * 22,
        position->y + *scale_factor * 15,
        position->x + *scale_factor * 32,
        position->y + *scale_factor * 60);

    ez_fill_rectangle (
        ev->win,
        position->x + *scale_factor * 42,
        position->y + *scale_factor * 15,
        position->x + *scale_factor * 52,
        position->y + *scale_factor * 60);

}

void draw_stop_btn_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    /* BTN */
    draw_btn_widget(ev, scale_factor, position, 75, 75) ;

    /* ICON */
    if (p_ctx-> running_status != STATE_STOPPED)
    {
        ez_set_color (DEFAULT_FOREGROUND_COLOR);
    }
    else
    {
        ez_set_color (RED);
    }

    ez_fill_rectangle (
        ev->win,
        position->x + *scale_factor * 25,
        position->y + *scale_factor * 25,
        position->x + *scale_factor * 50,
        position->y + *scale_factor * 50);

}

void draw_light_btn_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    /* BTN */
    draw_btn_widget(ev, scale_factor, position, 75, 75) ;

    /* ICON */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if ( REFRESH_1HZ > 50 )
    {
        if (( p_ctx->actuators->light_status ) && (p_ctx->mode == MANUAL)) ez_set_color (YELLOW);
        if (( p_ctx->actuators->light_status ) && (p_ctx->mode == AUTO))   ez_set_color (GREEN);
    }

    if (p_ctx->actuators->light_status == OFF) ez_set_color (DEFAULT_FOREGROUND_COLOR);

    /* rays */
    ez_set_thick(*scale_factor * 2) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 20,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 55,
        position->y + *scale_factor * 55) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 20,
        position->y + *scale_factor * 55,
        position->x + *scale_factor * 55,
        position->y + *scale_factor * 20) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 38,
        position->y + *scale_factor * 15,
        position->x + *scale_factor * 38,
        position->y + *scale_factor * 60) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 15,
        position->y + *scale_factor * 38,
        position->x + *scale_factor * 60,
        position->y + *scale_factor * 38) ;

    /* center mask */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_circle(
        ev->win,
        position->x + *scale_factor * 20,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 55,
        position->y + *scale_factor * 55) ;

    /* sun center */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if ( REFRESH_1HZ > 50 )
    {
        if ((p_ctx->actuators->light_status == ON ) && (p_ctx->mode == MANUAL)) ez_set_color (YELLOW);
        if ((p_ctx->actuators->light_status == ON ) && (p_ctx->mode == AUTO))   ez_set_color (GREEN);
    }

    if (p_ctx->actuators->light_status == OFF) ez_set_color (DEFAULT_FOREGROUND_COLOR);

    ez_fill_circle(
        ev->win,
        position->x + *scale_factor * 30,
        position->y + *scale_factor * 30,
        position->x + *scale_factor * 45,
        position->y + *scale_factor * 45) ;

    /* reinit thicker */
    ez_set_thick(1) ;
}

void draw_water_btn_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    /* BTN */
    draw_btn_widget(ev, scale_factor, position, 75, 75) ;

    /* ICON */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if ( REFRESH_1HZ > 50 )
    {
        if ((p_ctx->actuators->water_status == ON ) && (p_ctx->mode == AUTO))   ez_set_color (GREEN);
        if ((p_ctx->actuators->water_status == ON ) && (p_ctx->mode == MANUAL)) ez_set_color (BLUE);
    }

    if (p_ctx->actuators->water_status == OFF) ez_set_color (DEFAULT_FOREGROUND_COLOR);

    /* CLOUD */
    ez_fill_circle(
        ev->win,
        position->x + *scale_factor * 15,
        position->y + *scale_factor * 15,
        position->x + *scale_factor * 35,
        position->y + *scale_factor * 35) ;
    ez_fill_circle(
        ev->win,
        position->x + *scale_factor * 25,
        position->y + *scale_factor * 10,
        position->x + *scale_factor * 55,
        position->y + *scale_factor * 40) ;
    ez_fill_circle(
        ev->win,
        position->x + *scale_factor * 45,
        position->y + *scale_factor * 15,
        position->x + *scale_factor * 65,
        position->y + *scale_factor * 35) ;

    /* RAIN */
    ez_set_thick(*scale_factor * 2) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 25,
        position->y + *scale_factor * 45,
        position->x + *scale_factor * 20,
        position->y + *scale_factor * 60) ;

    ez_draw_line(ev->win,
        position->x + *scale_factor * 35,
        position->y + *scale_factor * 45,
        position->x + *scale_factor * 30,
        position->y + *scale_factor * 60) ;

    ez_draw_line(ev->win,
        position->x + *scale_factor * 45,
        position->y + *scale_factor * 45,
        position->x + *scale_factor * 40,
        position->y + *scale_factor * 60) ;

    ez_draw_line(ev->win,
        position->x + *scale_factor * 55,
        position->y + *scale_factor * 45,
        position->x + *scale_factor * 50,
        position->y + *scale_factor * 60) ;

    /* RAIN MASK */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_draw_line(ev->win,
        position->x + *scale_factor * 15,
        position->y + *scale_factor * 50,
        position->x + *scale_factor * 60,
        position->y + *scale_factor * 50) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 15,
        position->y + *scale_factor * 55,
        position->x + *scale_factor * 60,
        position->y + *scale_factor * 55) ;

    /* reinit thicker */
    ez_set_thick(1) ;
}

void draw_fan_btn_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    /* BTN */
    draw_btn_widget(ev, scale_factor, position, 75, 75) ;

    /* ICON */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if ( REFRESH_1HZ > 50 )
    {
        if ((p_ctx->actuators->fan_status == ON ) && (p_ctx->mode == MANUAL)) ez_set_color (ORANGE);
        if ((p_ctx->actuators->fan_status == ON ) && (p_ctx->mode == AUTO))   ez_set_color (GREEN);
    }

    if (p_ctx->actuators->fan_status == OFF) ez_set_color (DEFAULT_FOREGROUND_COLOR);

    ez_set_thick(4 * *scale_factor) ;
    ez_draw_rectangle(
        ev->win,
        position->x + *scale_factor * 15,
        position->y + *scale_factor * 15,
        position->x + *scale_factor * 60,
        position->y + *scale_factor * 60) ;
    ez_set_thick(1) ;

    ez_fill_circle(
        ev->win,
        position->x + *scale_factor * 20,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 53,
        position->y + *scale_factor * 53) ;

    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_circle(
        ev->win,
        position->x + *scale_factor * 30,
        position->y + *scale_factor * 30,
        position->x + *scale_factor * 43,
        position->y + *scale_factor * 43) ;

    ez_set_thick(*scale_factor * 2) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 25,
        position->y + *scale_factor * 25,
        position->x + *scale_factor * 50,
        position->y + *scale_factor * 50) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 25,
        position->y + *scale_factor * 50,
        position->x + *scale_factor * 50,
        position->y + *scale_factor * 20) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 38,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 38,
        position->y + *scale_factor * 55) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 20,
        position->y + *scale_factor * 38,
        position->x + *scale_factor * 55,
        position->y + *scale_factor * 38) ;

    /* reinit thicker */
    ez_set_thick(1) ;
}

void draw_heat_btn_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    /* BTN */
    draw_btn_widget(ev, scale_factor, position, 75, 75) ;

    /* ICON */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if ( REFRESH_1HZ > 50 )
    {
        if ((p_ctx->actuators->heat_status == ON ) && (p_ctx->mode == MANUAL)) ez_set_color (RED);
        if ((p_ctx->actuators->heat_status == ON ) && (p_ctx->mode == AUTO))   ez_set_color (GREEN);
    }

    if (p_ctx->actuators->heat_status == OFF) ez_set_color (DEFAULT_FOREGROUND_COLOR);

    /* 3 triangles */
    ez_fill_triangle (
        ev->win,
        position->x + *scale_factor * 15,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 25,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 20,
        position->y + *scale_factor * 10 );

    ez_fill_triangle (
        ev->win,
        position->x + *scale_factor * 35,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 45,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 40,
        position->y + *scale_factor * 10 );

    ez_fill_triangle (
        ev->win,
        position->x + *scale_factor * 55,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 65,
        position->y + *scale_factor * 20,
        position->x + *scale_factor * 60,
        position->y + *scale_factor * 10 );

    /* Heat */
    ez_set_thick(4 * *scale_factor) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 20,
        position->y + *scale_factor * 45,
        position->x + *scale_factor * 25,
        position->y + *scale_factor * 35) ;

    ez_draw_line(ev->win,
        position->x + *scale_factor * 25,
        position->y + *scale_factor * 35,
        position->x + *scale_factor * 20,
        position->y + *scale_factor * 25) ;

    ez_draw_line(ev->win,
        position->x + *scale_factor * 40,
        position->y + *scale_factor * 45,
        position->x + *scale_factor * 45,
        position->y + *scale_factor * 35) ;

    ez_draw_line(ev->win,
        position->x + *scale_factor * 45,
        position->y + *scale_factor * 35,
        position->x + *scale_factor * 40,
        position->y + *scale_factor * 25) ;

    ez_draw_line(ev->win,
        position->x + *scale_factor * 60,
        position->y + *scale_factor * 45,
        position->x + *scale_factor * 65,
        position->y + *scale_factor * 35) ;

    ez_draw_line(ev->win,
        position->x + *scale_factor * 65,
        position->y + *scale_factor * 35,
        position->x + *scale_factor * 60,
        position->y + *scale_factor * 25) ;

    /* Bottom */
    ez_set_thick(4 * *scale_factor) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 15,
        position->y + *scale_factor * 60,
        position->x + *scale_factor * 65,
        position->y + *scale_factor * 60) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 15,
        position->y + *scale_factor * 60,
        position->x + *scale_factor * 15,
        position->y + *scale_factor * 50) ;
    ez_draw_line(ev->win,
        position->x + *scale_factor * 65,
        position->y + *scale_factor * 60,
        position->x + *scale_factor * 65,
        position->y + *scale_factor * 50) ;

    /* reinit thicker */
    ez_set_thick(1) ;
}

void draw_controlpanel_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );
    DEBUG_ASSERT( position == NULL );

    static widget_position_t btn_play_position ;
    static widget_position_t btn_pause_position ;
    static widget_position_t btn_stop_position ;
    static widget_position_t btn_light_position ;
    static widget_position_t btn_water_position ;
    static widget_position_t btn_fan_position ;
    static widget_position_t btn_heat_position ;

    /* widget background */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        position->x ,
        position->y ,
        position->x + *scale_factor * 320,
        position->y + *scale_factor * 203 );

    /* main frame */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_rectangle (
        ev->win,
        position->x + *scale_factor * 5 ,
        position->y + *scale_factor * 5,
        position->x + *scale_factor * 315,
        position->y + *scale_factor * 198 );

    /* desc */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle( ev->win, position->x + *scale_factor * 10, position->y, position->x + *scale_factor * 100, position->y + *scale_factor * 24 ) ;
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    ez_draw_text (ev->win, EZ_TL, position->x + *scale_factor * 15, position->y , "Control panel");

    /* infos */
    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if (p_ctx->mode == AUTO) ez_set_color (GREEN);
    ez_draw_rectangle (
        ev->win,
        position->x + *scale_factor * 16 ,
        position->y + *scale_factor * 21,
        position->x + *scale_factor * 46,
        position->y + *scale_factor * 37 );
    ez_draw_text (ev->win, EZ_TL, position->x + *scale_factor * 20, position->y +23 , "AUTO");

    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if (p_ctx->mode == MANUAL) ez_set_color (GREEN);
    ez_draw_rectangle (
        ev->win,
        position->x + *scale_factor * 56 ,
        position->y + *scale_factor * 21,
        position->x + *scale_factor * 98,
        position->y + *scale_factor * 37 );
    ez_draw_text (ev->win, EZ_TL, position->x + *scale_factor * 60, position->y +23 , "MANUAL");
    ez_set_color (DEFAULT_FOREGROUND_COLOR);

    ez_set_color (DEFAULT_FOREGROUND_COLOR);
    if (p_ctx->sensors->temperature > (float)p_ctx->cfg->high_temp_limit ) ez_set_color (RED);
    ez_draw_rectangle (
        ev->win,
        position->x + *scale_factor * 108 ,
        position->y + *scale_factor * 21,
        position->x + *scale_factor * 155,
        position->y + *scale_factor * 37 );
    ez_draw_text (ev->win, EZ_TL, position->x + *scale_factor * 112, position->y +23 , "OVERHAT");
    ez_set_color (DEFAULT_FOREGROUND_COLOR);

    /* btn*/
    btn_play_position.x    = position->x + *scale_factor * 10 ;
    btn_play_position.y    = position->y + *scale_factor * 45 ;
    draw_play_btn_widget(ev, scale_factor, &btn_play_position ) ;

    btn_pause_position.x   = position->x + *scale_factor * 160 ;
    btn_pause_position.y   = position->y + *scale_factor * 45 ;
    draw_pause_btn_widget(ev, scale_factor, &btn_pause_position ) ;

    btn_stop_position.x    = position->x + *scale_factor * 235 ;
    btn_stop_position.y    = position->y + *scale_factor * 45 ;
    draw_stop_btn_widget(ev, scale_factor, &btn_stop_position ) ;

    btn_light_position.x   = position->x + *scale_factor * 10 ;
    btn_light_position.y   = position->y + *scale_factor * 120 ;
    draw_light_btn_widget(ev, scale_factor, &btn_light_position ) ;

    btn_water_position.x   = position->x + *scale_factor * 85 ;
    btn_water_position.y   = position->y + *scale_factor * 120 ;
    draw_water_btn_widget(ev, scale_factor, &btn_water_position ) ;

    btn_fan_position.x     = position->x + *scale_factor * 160 ;
    btn_fan_position.y     = position->y + *scale_factor * 120 ;
    draw_fan_btn_widget(ev, scale_factor, &btn_fan_position ) ;

    btn_heat_position.x    = position->x + *scale_factor * 235 ;
    btn_heat_position.y    = position->y + *scale_factor * 120 ;
    draw_heat_btn_widget(ev, scale_factor, &btn_heat_position ) ;

}
