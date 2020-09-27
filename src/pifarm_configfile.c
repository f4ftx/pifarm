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

#include "pifarm_configfile.h"

/* Configuration */
extern config_t     cfg ;

static config_syntax_t      syntax[] =
{
    { "RELAY00.GPIO",                   &cfg.relay_gpio[0],     TYPE_UINT8 },
    { "RELAY01.GPIO",                   &cfg.relay_gpio[1],     TYPE_UINT8 },
    { "RELAY02.GPIO",                   &cfg.relay_gpio[2],     TYPE_UINT8 },
    { "RELAY03.GPIO",                   &cfg.relay_gpio[3],     TYPE_UINT8 },
    { "RELAY04.GPIO",                   &cfg.relay_gpio[4],     TYPE_UINT8 },
    { "RELAY05.GPIO",                   &cfg.relay_gpio[5],     TYPE_UINT8 },
    { "RELAY06.GPIO",                   &cfg.relay_gpio[6],     TYPE_UINT8 },
    { "RELAY07.GPIO",                   &cfg.relay_gpio[7],     TYPE_UINT8 },
    { "RELAY08.GPIO",                   &cfg.relay_gpio[8],     TYPE_UINT8 },
    { "RELAY09.GPIO",                   &cfg.relay_gpio[9],     TYPE_UINT8 },
    { "RELAY10.GPIO",                   &cfg.relay_gpio[10],    TYPE_UINT8 },
    { "RELAY11.GPIO",                   &cfg.relay_gpio[11],    TYPE_UINT8 },
    { "RELAY12.GPIO",                   &cfg.relay_gpio[12],    TYPE_UINT8 },
    { "RELAY13.GPIO",                   &cfg.relay_gpio[13],    TYPE_UINT8 },
    { "RELAY14.GPIO",                   &cfg.relay_gpio[14],    TYPE_UINT8 },
    { "RELAY15.GPIO",                   &cfg.relay_gpio[15],    TYPE_UINT8 },

    { "RELAY00.VOLTAGE",                &cfg.relay_voltage[0],  TYPE_UINT8 },
    { "RELAY01.VOLTAGE",                &cfg.relay_voltage[1],  TYPE_UINT8 },
    { "RELAY02.VOLTAGE",                &cfg.relay_voltage[2],  TYPE_UINT8 },
    { "RELAY03.VOLTAGE",                &cfg.relay_voltage[3],  TYPE_UINT8 },
    { "RELAY04.VOLTAGE",                &cfg.relay_voltage[4],  TYPE_UINT8 },
    { "RELAY05.VOLTAGE",                &cfg.relay_voltage[5],  TYPE_UINT8 },
    { "RELAY06.VOLTAGE",                &cfg.relay_voltage[6],  TYPE_UINT8 },
    { "RELAY07.VOLTAGE",                &cfg.relay_voltage[7],  TYPE_UINT8 },
    { "RELAY08.VOLTAGE",                &cfg.relay_voltage[8],  TYPE_UINT8 },
    { "RELAY09.VOLTAGE",                &cfg.relay_voltage[9],  TYPE_UINT8 },
    { "RELAY10.VOLTAGE",                &cfg.relay_voltage[10], TYPE_UINT8 },
    { "RELAY11.VOLTAGE",                &cfg.relay_voltage[11], TYPE_UINT8 },
    { "RELAY12.VOLTAGE",                &cfg.relay_voltage[12], TYPE_UINT8 },
    { "RELAY13.VOLTAGE",                &cfg.relay_voltage[13], TYPE_UINT8 },
    { "RELAY14.VOLTAGE",                &cfg.relay_voltage[14], TYPE_UINT8 },
    { "RELAY15.VOLTAGE",                &cfg.relay_voltage[15], TYPE_UINT8 },

    { "RELAY00.USAGE",                  &cfg.relay_usage[0],    TYPE_STRING },
    { "RELAY01.USAGE",                  &cfg.relay_usage[1],    TYPE_STRING },
    { "RELAY02.USAGE",                  &cfg.relay_usage[2],    TYPE_STRING },
    { "RELAY03.USAGE",                  &cfg.relay_usage[3],    TYPE_STRING },
    { "RELAY04.USAGE",                  &cfg.relay_usage[4],    TYPE_STRING },
    { "RELAY05.USAGE",                  &cfg.relay_usage[5],    TYPE_STRING },
    { "RELAY06.USAGE",                  &cfg.relay_usage[6],    TYPE_STRING },
    { "RELAY07.USAGE",                  &cfg.relay_usage[7],    TYPE_STRING },
    { "RELAY08.USAGE",                  &cfg.relay_usage[8],    TYPE_STRING },
    { "RELAY09.USAGE",                  &cfg.relay_usage[9],    TYPE_STRING },
    { "RELAY10.USAGE",                  &cfg.relay_usage[10],   TYPE_STRING },
    { "RELAY11.USAGE",                  &cfg.relay_usage[11],   TYPE_STRING },
    { "RELAY12.USAGE",                  &cfg.relay_usage[12],   TYPE_STRING },
    { "RELAY13.USAGE",                  &cfg.relay_usage[13],   TYPE_STRING },
    { "RELAY14.USAGE",                  &cfg.relay_usage[14],   TYPE_STRING },
    { "RELAY15.USAGE",                  &cfg.relay_usage[15],   TYPE_STRING },

    { "PROGRAM_OVERHEAT_TEMPERATURE",   &cfg.high_temp_limit,   TYPE_UINT8  },
    { "PROGRAM_TOO_COLD_TEMPERATEUR",   &cfg.low_temp_limit,    TYPE_UINT8  },
    { "PROGRAM_WIN_TIME_INTERVAL",      &cfg.win_interval,      TYPE_UINT16 },
    { "PROGRAM_WIN_TIME_DURATION",      &cfg.win_duration,      TYPE_UINT16 },
    { "PROGRAM_SUNRISE_TIME",           &cfg.sunrise_time,      TYPE_STRING },
    { "PROGRAM_SUNSET_TIME",            &cfg.sunset_time,       TYPE_STRING },
    { "PROGRAM_WATERING_TIME",          &cfg.watering_time,     TYPE_STRING },
    { "PROGRAM_WATERING_DURATION",      &cfg.watering_duration, TYPE_UINT16 },

    { NULL,              NULL,                   0                          }
};

void parse_keyword (const char * const key, const char * const val)
{
    register uint8_t    iter   ;
    uint32_t            ival   ;
    char                *endptr;

    if ((syntax == NULL) || (key == NULL) || (val == NULL)) return;

    iter = 0 ;

    while (syntax[iter].key != NULL) /* Search key */
    {
        if (strcmp (key, syntax[iter].key) == 0)
        {
#ifdef DBG_CONFIGFILE_PARSE
            DEBUG_PRINT("DBG [CONFIG PARSER] - Key found:%-16s", key) ;
#endif
            switch (syntax[iter].type) /* key found => extract value */
            {
                case TYPE_UINT8:
                    ival = (uint8_t) strtol (val, &endptr, 0);
                    if ((*val != 0) && (*endptr == 0))
                    {
                        memcpy (syntax[iter].data, &ival, sizeof (uint8_t));
#ifdef DBG_CONFIGFILE_PARSE
                        DEBUG_PRINT("                      Type:UINT8T Value:%d", ival) ;
#endif
                    }
                break;

                case TYPE_UINT16:
                    ival = (uint16_t) strtol (val, &endptr, 0);
                    if ((*val != 0) && (*endptr == 0))
                    {
                        memcpy (syntax[iter].data, &ival, sizeof (uint16_t));
#ifdef DBG_CONFIGFILE_PARSE
                        DEBUG_PRINT("                      Type:UINT16T Value:%d", ival) ;
#endif
                    }
                break;

                case TYPE_UINT32:
                    ival = strtol (val, &endptr, 0);
                    if ((*val != 0) && (*endptr == 0))
                    {
                        memcpy (syntax[iter].data, &ival, sizeof (uint32_t));
#ifdef DBG_CONFIGFILE_PARSE
                        DEBUG_PRINT("                      Type:UINT32T Value:%d", ival) ;
#endif
                    }
                break;
                case TYPE_STRING:
                    {
                        strncpy (syntax[iter].data, val, 256 * sizeof(char));
#ifdef DBG_CONFIGFILE_PARSE
                        DEBUG_PRINT("                      Type:STRING  Value:%s", val) ;
#endif
                    }
                    break;

                default:
                    break;
            }

            return;
        }
        iter++;
    }
}

config_t *read_config_file (const char * const config_filename)
{
    FILE *              config_fd ;
    char                buf[512]  ;
    char *              key       ;
    char *              val       ;
    register uint8_t    iter      ;

    /* init conf */
    memset (&cfg, 0, sizeof(cfg));

    /* open config */
#ifdef DBG_CONFIGFILE_PARSE
    DEBUG_PRINT ("DBG [CONFIG PARSER] - Opening file: '%s'", config_filename);
#endif
    config_fd = fopen (config_filename, "r");
    if (config_fd == NULL)
    {
#ifdef DBG_CONFIGFILE_PARSE
        DEBUG_PRINT ("DBG [CONFIG PARSER] - Failed to open file '%s'", config_filename);
#endif
        perror (config_filename);
        return NULL;
    }

    /* quick & dirty parser */
    while (NULL != fgets (buf, 512, config_fd))
    {
        /* read line : "<key>=<val>\n" */
        key = strtok (buf, "=");
        val = strtok (NULL, "\n");

        if (key != NULL)
        {
            /* remove leadings spaces/tab */
            while (((key[0]!=0)) && ((key[0]==' ')||(key[0]=='\t'))) key++;

            if (key[0] != '#') /* ignore comments */
            {
                if (val != NULL) /* line ok  */
                {
                    /* remove \r & \n */
                    for (iter=0; iter<strlen(val); iter++)
                    {
                        if ((val[iter] == 10) || (val[iter] == 13)) val[iter]=0;
                    }
                    /* extract data */
                    parse_keyword (key, val);
                }
            }
          }
      }

#ifdef DBG_CONFIGFILE_PARSE
    DEBUG_PRINT ("DBG [CONFIG PARSER] - Closing file '%s'", config_filename);
#endif
    fclose (config_fd);

    return &cfg;
}
