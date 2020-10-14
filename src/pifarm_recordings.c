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

#include "pifarm_recordings.h"

extern context_t     *p_ctx ;

void record_sample(void)
{
    recording_t             p_record       ;
    FILE                    * fd_record_file ;
    struct        tm        now_tm  ;
    struct        timeval   time_now    ;

    gettimeofday(&time_now, NULL);
    now_tm = *localtime(&time_now.tv_sec);

    memset(&p_record, 0, sizeof(recording_t)) ;

    /* populate data */
    p_record.timestamp    = now_tm ;;
    p_record.mode         = p_ctx->mode ;
    p_record.temperature  = p_ctx->sensors->temperature;
    p_record.pressure     = p_ctx->sensors->pressure;
    p_record.humidity     = p_ctx->sensors->humidity;
    p_record.altitude     = p_ctx->sensors->altitude;
    p_record.light_status = p_ctx->actuators->light_status ;
    p_record.water_status = p_ctx->actuators->water_status ;
    p_record.fan_status   = p_ctx->actuators->fan_status ;
    p_record.heat_status  = p_ctx->actuators->heat_status ;

    /* try to open file */
    fd_record_file = fopen(p_ctx->cfg->records_file_path, "ab") ;

    /* create if not exists */
    if (fd_record_file == NULL)
    {
        fd_record_file = fopen(p_ctx->cfg->records_file_path, "wb") ;
        if (fd_record_file == NULL)
        {
            perror(p_ctx->cfg->records_file_path) ;
            return ;
        }
    }

    /* record */
    if(fwrite(&p_record, sizeof(recording_t), 1, fd_record_file) != 1)
        {
            perror("Erreur lors de l'ecriture des donnees dans le fichier!!");
            fclose(fd_record_file);
            return ;
        }

    /* close file */
    fclose(fd_record_file);

}

void test_read(void)
{
    FILE                    * fd_record_file ;
    recording_t             p_record       ;

    fd_record_file = fopen(p_ctx->cfg->records_file_path, "rb") ;
    memset(&p_record, 0, sizeof(recording_t)) ;
    while ((fread(&p_record, sizeof(p_record), 1 , fd_record_file)) == 1 )
    {
        printf("timestamp   : %d:%d\n",p_record.timestamp.tm_hour ,p_record.timestamp.tm_min  );
        printf("mode        : %d\n",   p_record.mode);
        printf("temperature : %f\n",   p_record.temperature );
        printf("humidity    : %f\n",   p_record.humidity );
        printf("pressure    : %f\n",   p_record.pressure );
        printf("altitude    : %f\n",   p_record.altitude );
        printf("light_status: %d\n",   p_record.light_status);
        printf("water_status: %d\n",   p_record.water_status);
        printf("fan_status  : %d\n",   p_record.fan_status);
        printf("heat_status : %d\n",   p_record.heat_status);
    }

    fclose(fd_record_file);

}
