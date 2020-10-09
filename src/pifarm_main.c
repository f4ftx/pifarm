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

/* Standard */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

/* Program */
#include "pifarm_debug.h"
#include "pifarm_configfile.h"
#include "pifarm_gui.h"
#include "pifarm_commons.h"
#include "pifarm_gpio.h"
#include "pifarm_logic.h"
#include "pifarm_tools.h"

/* DEFAULT PARAMETERS ------------------------------------------------------- */


/* GLOBALS ------------------------------------------------------------------ */
uint8_t             REFRESH_1HZ   = 0 ;
uint8_t             REFRESH_10HZ  = 0 ;
uint16_t            REFRESH_1MIN  = 0 ;
context_t           * p_ctx ;
config_t            cfg ;
static const char   * user_config_filename = ".pifarmrc" ;
static const char   * system_config_filename = "pifarmrc" ;

void display_help(void)
{
    const char  *usage_str=\
"\nUsage :\n\
tpifarm [option]\n\
Valid options are :\n\
\t-h --help               show this message and exit\n\
\t-c --config=<file>      load custom configuration file\n\
\n\
Without --config argument, program try to open first ~/.pifarmrc.\n\
If it fail, program will try to open /etc/pifarmrc\n" ;

    printf("%s", usage_str);
}

/* MAIN PROGRAM ------------------------------------------------------------- */
int main(int argc, char **argv)
{
    /* Decl */
    Ez_window       win;              /* Main window*/
    gfx_context_t   * p_gc ;          /* Store some gfx context parts */
    uint8_t         relay_id ;        /* Used for relay status init */
    uint8_t         force_config ;    /* Used if configuration is provided as arg */
    sensors_t       * p_sensors ;
    actuators_t     * p_actuators;
    char            * p_config_path ; /* Absolute  configuration file path */
    char            * p_home_path ;   /* point to $HOME */
    int             c;                /* Used to store getopt result */
    int             option_index;     /*  */
    static struct   option long_options[] = {
      {"help",         no_argument, 0, 'h'},
      {"config", required_argument, 0, 'c'},
      {0, 0, 0, 0 }
    };

    /* Init */
    force_config = 0 ;
    p_config_path = (char *)malloc(256 * sizeof(char));
    DEBUG_ASSERT( p_config_path == NULL );

    /* Option parser */
    while (1)
    {
        option_index = 0;
        c = getopt_long (argc, argv, ":hc:", long_options, &option_index);
        if (c == -1) break;
        switch (c)
        {
            case 'c':
                if (optarg == NULL)
                {
                    printf("Invalid argument for param --config\n\n") ;
                    display_help() ; return 1;
                }
                sprintf(p_config_path, "%s", optarg);
                force_config = 1 ;
                break;
            case 'h':
            case '?':
                display_help() ; return 0;
                break;
            default:
                break;
        }
    }

    /* 1st try : path is provided as arg */
    if ( force_config == 1 )
    {
#ifdef DBG_CONFIGFILE_PARSE
        DEBUG_PRINT("DBG [CONFIG PARSER] - Custom configuration file: %s", p_config_path ) ;
#endif
        if ( read_config_file(p_config_path) == NULL)
        {
#ifdef DBG_CONFIGFILE_PARSE
            DEBUG_PRINT("DBG [CONFIG PARSER] - Unable to open %s", p_config_path ) ;
#endif
            exit(1) ;
        }

    }
    else
    {
        /* 2nd try : dafault in $HOME*/
        p_home_path = getenv("HOME") ;
        if (p_home_path == NULL)
        {
#ifdef DBG_CONFIGFILE_PARSE
            DEBUG_PRINT("DBG [CONFIG PARSER] - Unable to expand $HOME" ) ;
#endif
        }
        else
        {
            sprintf(p_config_path, "%s/%s", p_home_path, user_config_filename);
            /* 3rd try : in system path */
            if ( read_config_file(p_config_path) == NULL)
            {
#ifdef DBG_CONFIGFILE_PARSE
                DEBUG_PRINT("DBG [CONFIG PARSER] - Unable to open %s", p_config_path ) ;
#endif
                sprintf(p_config_path, "/etc/%s", system_config_filename);
                if ( read_config_file(p_config_path) == NULL)
                {
#ifdef DBG_CONFIGFILE_PARSE
                DEBUG_PRINT("DBG [CONFIG PARSER] - Unable to open %s", p_config_path ) ;
#endif
                    return 1 ;
                }
            }
        }
    }

    /* Init context */
    p_ctx = (context_t *)   malloc(sizeof(context_t)) ;
    DEBUG_ASSERT( p_ctx == NULL );

    p_gc  = (gfx_context_t *) malloc(sizeof(gfx_context_t)) ;
    DEBUG_ASSERT( p_gc == NULL );

    p_sensors = (sensors_t *) malloc(sizeof(sensors_t));
    DEBUG_ASSERT( p_sensors == NULL );

    p_actuators = (actuators_t *) malloc(sizeof(actuators_t));
    DEBUG_ASSERT( p_sensors == NULL );

    p_ctx->cfg = &cfg ;
    p_ctx->gc = p_gc ;
    p_ctx->sensors = p_sensors ;
    p_ctx->actuators = p_actuators ;

    /* status */
    p_ctx->mode                     = NONE ;
    p_ctx->running_status           = STATE_STOPPED ;
    p_ctx->actuators->light_status  = OFF ;
    p_ctx->actuators->water_status  = OFF ;
    p_ctx->actuators->fan_status    = OFF ;
    p_ctx->actuators->heat_status   = OFF ;
    p_ctx->sensors->temperature     = 0.;
    p_ctx->sensors->humidity        = 0 ;
    p_ctx->sensors->pressure        = 0 ;
    p_ctx->sensors->altitude        = 0 ;
    p_ctx->gc->tab_1                = ON ;
    p_ctx->gc->tab_2                = OFF ;
    p_ctx->gc->tab_3                = OFF ;
    p_ctx->gc->tab_4                = OFF ;
    p_ctx->shutdown_request         = OFF ;

    /* click zones */
    click_zone_t cz_btn_play   = { CZ_PLAY } ;
    click_zone_t cz_btn_pause  = { CZ_PAUSE } ;
    click_zone_t cz_btn_stop   = { CZ_STOP } ;
    click_zone_t cz_btn_light  = { CZ_LIGHT } ;
    click_zone_t cz_btn_fan    = { CZ_FAN } ;
    click_zone_t cz_btn_water  = { CZ_WATER } ;
    click_zone_t cz_btn_heat   = { CZ_HEAT } ;
    click_zone_t cz_btn_tab_1  = { CZ_T1 } ;
    click_zone_t cz_btn_tab_2  = { CZ_T2 } ;
    click_zone_t cz_btn_tab_3  = { CZ_T3 } ;
    click_zone_t cz_btn_tab_4  = { CZ_T4 } ;
    p_ctx->gc->cz_btn_play     = &cz_btn_play  ;
    p_ctx->gc->cz_btn_pause    = &cz_btn_pause ;
    p_ctx->gc->cz_btn_stop     = &cz_btn_stop  ;
    p_ctx->gc->cz_btn_light    = &cz_btn_light ;
    p_ctx->gc->cz_btn_fan      = &cz_btn_fan   ;
    p_ctx->gc->cz_btn_water    = &cz_btn_water ;
    p_ctx->gc->cz_btn_heat     = &cz_btn_heat  ;
    p_ctx->gc->cz_btn_tab_1    = &cz_btn_tab_1 ;
    p_ctx->gc->cz_btn_tab_2    = &cz_btn_tab_2 ;
    p_ctx->gc->cz_btn_tab_3    = &cz_btn_tab_3 ;
    p_ctx->gc->cz_btn_tab_4    = &cz_btn_tab_4 ;

    for (relay_id = 0 ; relay_id < RELAY_NB ; relay_id++ )
    {
        p_ctx->relay_status[relay_id] = RELAY_STATUS_UNKNOWN ;
    }

    /* Init hardware */
    gpio_init();
    setup_gpio_relay_ports();
    shutdown_all_relays();       /* relays always start off */
    acquire_bme280();

    /* Init GUI */
    if (ez_init() < 0) exit(1);
    win = ez_window_create (DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "piFarm", win_on_event);
    ez_window_dbuf(win, 1);

    ez_start_timer(win, REFRESH_INTERVAL) ;
    ez_main_loop();

    /* Leave relays OFF */
    shutdown_all_relays();

    if ( p_ctx->shutdown_request == ON )
    {
        run_system_command("sudo poweroff") ;
    }

    /* Memory clean */
    free(p_gc) ;
    free(p_sensors);
    free(p_actuators);
    free(p_ctx) ;
    free(p_config_path);

    p_gc = NULL ;
    p_sensors = NULL ;
    p_actuators = NULL ;
    p_ctx = NULL ;
    p_home_path   = NULL ;
    p_config_path = NULL ;


    return 0 ;
}
