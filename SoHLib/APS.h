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



typedef struct
{
    uint32_t raw;
    float adc_voltage;
    float sensor_voltage;
    float pressure_mpa;
    float pressure_bar;
    float depth_meter;
    float depth_cm;
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




#endif /* INC_APS_H_ */
