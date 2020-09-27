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
