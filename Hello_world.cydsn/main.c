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
#include "project.h"
#include <stdio.h>

const float battery_voltage_convertion_coeffitient = 1.5;
const float level_convert_coefficient = 5.0/4095.0;

float battery_voltage();

int main(void)
{
    
    
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_1_Start();    
    ADC_Battery_Start();
    ADC_Battery_StartConvert();
    
    
    printf("ADC Test...\n");
 

    
    
    for(;;)
    {
        
        float voltage = battery_voltage();
        printf("The voltage is: %.8f V\n", voltage);
        /* Place your application code here. */
    }
    return 0;
  
}

float battery_voltage(){
    float result = 0;
    ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT);
    int16_t adc_value = ADC_Battery_GetResult16();
    printf("level: %d\n", adc_value);
    result = adc_value * battery_voltage_convertion_coeffitient * level_convert_coefficient;

    return result;
}

/* [] END OF FILE */
