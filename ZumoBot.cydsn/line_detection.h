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

#ifndef LINE_DETECTION
#define LINE_DETECTION

    #include <stdbool.h>
    #include <Reflectance.h>
    
    typedef struct sensors_difference_{
        int16_t sensor1;
        int16_t sensor2;
        int16_t sensor3;    
    } reflectance_offset_;
    
    // Sets the calibration between right and left sensors
    struct sensors_difference_ reflectance_calibrate(struct sensors_ *ref_readings); 
    // Edits the reflectance readings according to previous calibration
    int16_t reflectance_normalize(struct sensors_ *ref_readings, struct sensors_difference_ *ref_offset); 
    // Detects cross 
    bool cross_detected();
    
#endif

/* [] END OF FILE */
