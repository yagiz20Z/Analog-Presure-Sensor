/*
 * APS.h
 *
 *  Created on: Jun 27, 2026
 *      Author: yagiz20Z
 */

#ifndef INC_APS_H_
#define INC_APS_H_

#include "main.h"
#include <stdint.h>

// #define ADC_MAX_VALUE      4095.0f
// #define STM32_VREF         3.3f

// #define VOLTAGE_DIVIDER_RATIO  1.5f
// #define SENSOR_ZERO_VOLTAGE    0.5f
// #define SENSOR_SCALE_MPA       0.4f
// #define SURFACE_VOLTAGE        0.75f


typedef struct
{
    uint32_t raw;
    float adc_voltage;
    float sensor_voltage;
    float pressure_mpa;
    float pressure_bar;
    float depth_meter;
} APS_Data_t;

typedef struct
{
    ADC_HandleTypeDef  *hadc;
    UART_HandleTypeDef *huart;

    float adc_vref;                 
    float adc_max;                  
    float voltage_divider_ratio;    

    float sensor_zero_voltage;      
    float sensor_scale_mpa;         

    float surface_voltage;          
} APS_Config_t;

typedef struct
{
    APS_Config_t config;
    APS_Data_t data;
} APS_Handle_t;


void APS_Init(APS_Handle_t *aps, APS_Config_t *config);
void APS_Read(APS_Handle_t *aps);
void APS_Print(APS_Handle_t *aps);


// uint32_t APS_ReadRaw(void);
// float APS_RawToAdcVoltage(uint32_t adc_raw);
// float APS_RawToSensorVoltage(uint32_t adc_raw);
// float APS_VoltageToMPa(float voltage);
// float APS_VoltageToBar(float voltage);
// float APS_VoltageToDepthMeter(float voltage);
// void APS_Read(APS_Data_t *data);
// void APS_Print(const APS_Data_t *data);


#endif /* INC_APS_H_ */
