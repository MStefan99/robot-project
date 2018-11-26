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

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_1_Start();
    ADC_SAR_Start();
    ADC_SAR_StartConvert();
    int16_t value=0;
    printf("ADC test...\n");
    for(;;)
    {
       ADC_SAR_IsEndConversion(ADC_SAR_WAIT_FOR_RESULT);
       value=ADC_SAR_GetResult16();
       printf("%d\n",value);
    }
    return 0;
}

/* [] END OF FILE */
