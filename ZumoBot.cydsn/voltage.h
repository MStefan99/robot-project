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

#ifndef VOLTAGE_H_
#define VOLTAGE_H_
    
    #include <stdbool.h>
        
    // Returns the battery voltage 
    float battery_voltage(); 

    // Returns true if voltage is sufficient and false if not
    bool voltage_test(); 
    
#endif

/* [] END OF FILE */
