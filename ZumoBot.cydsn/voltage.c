/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "voltage.h"
#include <project.h>


float battery_voltage()
{
    float result = 0;
    const float battery_voltage_convertion_coeffitient = 1.5;
    const float level_convert_coefficient = 5.0/4095.0;
    
    ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT);
    int16_t adc_value = ADC_Battery_GetResult16();
    
    result = adc_value * battery_voltage_convertion_coeffitient * level_convert_coefficient;

    return result;
}

bool voltage_test()
{
    float voltage = battery_voltage();
        
    if (voltage > 3.8) {
        return true;
    } else {
        return false;
    } 
}

/* [] END OF FILE */
