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

#ifndef PIFARM_SENSORS_H
#define PIFARM_SENSORS_H   1

#include "pifarm_commons.h"

#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <wiringPiI2C.h>
#include <bme280.h>

/* I2C BME280 specific */
void    readCalibrationData(uint8_t fd, bme280_calib_data *data)                    ;
int32_t getTemperatureCalibration(bme280_calib_data *cal, uint32_t * adc_T);
float   getAltitude(float pressure);
float   compensateTemperature(int32_t t_fine)                                   ;
float   compensatePressure(int32_t adc_P, bme280_calib_data *cal, int32_t t_fine);
float   compensateHumidity(int32_t adc_H, bme280_calib_data *cal, int32_t t_fine);
void    getRawData(int8_t * fd, bme280_raw_data *raw) ;
uint8_t acquire_bme280(void);

#endif
