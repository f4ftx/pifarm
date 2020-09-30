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

#ifndef PIFARM_GUI_H
#define PIFARM_GUI_H

#include "pifarm_commons.h"
#include "pifarm_debug.h"
#include "pifarm_recordings.h"
#include "pifarm_sensors.h"
#include "pifarm_gpio.h"
#include "pifarm_logic.h"

#include <stdint.h>
#include <math.h>
#include <ez-draw.h>


/* DEFAULT WINDOWS SIZE */
#define DEFAULT_WINDOW_WIDTH  640
#define DEFAULT_WINDOW_HEIGHT 480

/* GUI PARAMETERS */
#define REFRESH_INTERVAL      10   /* 10ms 100Hz */

/* COLORS USED BY PROGRAM */
#define DEFAULT_BACKGROUND_COLOR    ez_get_RGB (0x05, 0x05, 0x05) /* dark grey  */
#define DEFAULT_FOREGROUND_COLOR    ez_get_RGB (0x50, 0x50, 0x50) /* light grey */
#define DEFAULT_ACTIVE_COLOR        ez_get_RGB (0x00, 0xff, 0x99) /* green      */
#define RED                         ez_get_RGB (0xff, 0x33, 0x33)
#define ORANGE                      ez_get_RGB (0xff, 0x75, 0x1A)
#define GREEN                       ez_get_RGB (0x00, 0xff, 0x99)
#define YELLOW                      ez_get_RGB (0xff, 0xd6, 0x33)
#define BLUE                        ez_get_RGB (0x4d, 0x79, 0xff)

/* ACTIVE CLIC ZONES */
#define CZ_PLAY       "play ",  325, 65,  455, 120
#define CZ_PAUSE      "pause",  475, 65,  530, 120
#define CZ_STOP       "stop ",  550, 65,  605, 120
#define CZ_LIGHT      "light",  325, 140, 380, 195
#define CZ_WATER      "water",  400, 140, 455, 195
#define CZ_FAN        "fan  ",  475, 140, 530, 195
#define CZ_HEAT       "heat ",  550, 140, 605, 195

/* GUI : GRAPHS WIDGET */
void draw_graphs_widget(Ez_event *ev, widget_position_t * position ) ;

/*  GUI : RELAYS STATUS WIDGET */
void draw_relays_widget(Ez_event *ev, widget_position_t * position ) ;

/*  GUI : 7 SEG DISPLAY ( T P H A ) */
void reverse(char* str, uint8_t len);
uint16_t intToStr(uint16_t x, char str[], uint8_t d);
void float_to_string(float n, char* res, uint8_t afterpoint);
void draw_segment(Ez_event *ev, uint8_t * scale_factor, widget_position_t *position, float points[12]) ;
void fill_segment(Ez_event *ev, uint8_t * scale_factor, widget_position_t *position, float points[12], uint8_t dot) ;
void draw_seven_segment_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t *position, char data, uint8_t dot) ;
void draw_seven_segment_grid_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t size, char * data ) ;
void draw_temperature_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t size, char * data ) ;
void draw_altitude_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t size, char * data ) ;
void draw_double_dot_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position) ;
void draw_clock_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position ) ;
void draw_pressure_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t size, char * data ) ;
void draw_humidity_widget(Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t size, char * data ) ;

/* GUI CONTROL PANEL WIDGET */
void draw_btn_widget          (Ez_event *ev, uint8_t * scale_factor, widget_position_t * position, uint8_t height, uint8_t width) ;
void draw_play_btn_widget     (Ez_event *ev, uint8_t * scale_factor, widget_position_t * position ) ;
void draw_pause_btn_widget    (Ez_event *ev, uint8_t * scale_factor, widget_position_t * position ) ;
void draw_stop_btn_widget     (Ez_event *ev, uint8_t * scale_factor, widget_position_t * position ) ;
void draw_light_btn_widget    (Ez_event *ev, uint8_t * scale_factor, widget_position_t * position ) ;
void draw_water_btn_widget    (Ez_event *ev, uint8_t * scale_factor, widget_position_t * position ) ;
void draw_fan_btn_widget      (Ez_event *ev, uint8_t * scale_factor, widget_position_t * position ) ;
void draw_heat_btn_widget     (Ez_event *ev, uint8_t * scale_factor, widget_position_t * position ) ;
void draw_controlpanel_widget (Ez_event *ev, uint8_t * scale_factor, widget_position_t * position ) ;

/* GUI GENERAL FCTS */
void win_on_button_press     (Ez_event *ev); /* Mouse button pressed      */
void win_on_button_release   (Ez_event *ev); /* Mouse button released     */
void win_on_motion_notify    (Ez_event *ev); /* Mouse moved               */
void win_on_timer_notify     (Ez_event *ev); /* The timer has expired     */
void win_on_expose           (Ez_event *ev); /* We must redraw everything */
void win_on_key_press        (Ez_event *ev); /* Key pressed               */
void win_on_key_release      (Ez_event *ev); /* Key released              */
void win_on_configure_notify (Ez_event *ev); /* Window size changed       */
void win_on_window_close     (Ez_event *ev); /* Close button pressed      */
void win_on_event            (Ez_event *ev);

uint8_t  click_match         (Ez_event *ev, click_zone_t * zone) ;

/* GUI DEBUG */
void debug_draw_click_zone(Ez_event *ev, uint8_t * scale_factor ) ;

#endif
