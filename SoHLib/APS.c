#include "APS.h"
#include <stdio.h>
#include <string.h>

void APS_Init(APS_Handle_t *aps, APS_Config_t *config)
{
    aps->config = *config;
}

static uint32_t APS_ReadRaw(APS_Handle_t *aps)
{
    uint32_t raw = 0;

    HAL_ADC_Start(aps->config.hadc);
    HAL_ADC_PollForConversion(aps->config.hadc, HAL_MAX_DELAY);
    raw = HAL_ADC_GetValue(aps->config.hadc);
    HAL_ADC_Stop(aps->config.hadc);

    return raw;
}

static float APS_RawToAdcVoltage(APS_Handle_t *aps, uint32_t raw)
{
    return ((float)raw * aps->config.adc_vref) / aps->config.adc_max;
}

static float APS_AdcVoltageToSensorVoltage(APS_Handle_t *aps, float adc_voltage)
{
    return adc_voltage * aps->config.voltage_divider_ratio;
}

static float APS_VoltageToMPa(APS_Handle_t *aps, float sensor_voltage)
{
    float pressure_mpa;

    pressure_mpa = (sensor_voltage - aps->config.sensor_zero_voltage)
                 * aps->config.sensor_scale_mpa;

    if (pressure_mpa < 0.0f)
    {
        pressure_mpa = 0.0f;
    }

    return pressure_mpa;
}

static float APS_VoltageToDepthMeter(APS_Handle_t *aps, float sensor_voltage)
{
    float pressure_bar;
    float depth_meter;

    pressure_bar = 4.0f * (sensor_voltage - aps->config.surface_voltage);

    if (pressure_bar < 0.0f)
    {
        pressure_bar = 0.0f;
    }

    depth_meter = pressure_bar * 10.2f;

    return depth_meter;
}

void APS_Read(APS_Handle_t *aps)
{
    aps->data.raw = APS_ReadRaw(aps);

    aps->data.adc_voltage =
        APS_RawToAdcVoltage(aps, aps->data.raw);

    aps->data.sensor_voltage =
        APS_AdcVoltageToSensorVoltage(aps, aps->data.adc_voltage);

    aps->data.pressure_mpa =
        APS_VoltageToMPa(aps, aps->data.sensor_voltage);

    aps->data.pressure_bar =
        aps->data.pressure_mpa * 10.0f;

    aps->data.depth_meter =
        APS_VoltageToDepthMeter(aps, aps->data.sensor_voltage);
        
}

void APS_Print(APS_Handle_t *aps)
{
    char buffer[150];

    snprintf(buffer, sizeof(buffer),
             "ADC: %lu | Vadc: %.2f V | Vsens: %.2f V | P: %.2f MPa | %.2f bar | Depth: %.2f m\r\n",
             aps->data.raw,
             aps->data.adc_voltage,
             aps->data.sensor_voltage,
             aps->data.pressure_mpa,
             aps->data.pressure_bar,
             aps->data.depth_meter);

    HAL_UART_Transmit(aps->config.huart,
                      (uint8_t *)buffer,
                      strlen(buffer),
                      HAL_MAX_DELAY);
}