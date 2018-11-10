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

#include "line_detection.h"

bool cross_detected() {
    struct sensors_ ref_readings;
    const uint16_t threshold=20000;
    reflectance_read(&ref_readings);
    if(ref_readings.l3 + ref_readings.l2 + ref_readings.l1 + ref_readings.r1 + ref_readings.r2 + ref_readings.r3 > threshold * 6){
        return true;
    }
    
    return false;
}

struct sensors_difference_ reflectance_calibrate(struct sensors_ *ref_readings)
{
    struct sensors_difference_ sensor_diff;
    sensor_diff.sensor1 = ref_readings->r1 - ref_readings->l1; 
    sensor_diff.sensor2 = ref_readings->r2 - ref_readings->l2; 
    sensor_diff.sensor3 = ref_readings->r3 - ref_readings->l3; 
    return sensor_diff;
}

int16_t reflectance_normalize(struct sensors_ *ref_readings, struct sensors_difference_ *ref_offset)
{
    ref_readings->r1 -= ref_offset->sensor1;
    ref_readings->r2 -= ref_offset->sensor2;
    ref_readings->r3 -= ref_offset->sensor3;
    /* returns the amount of shift from the line calculated as follows:
       ((r3 - l3) + ((r2 - l2) / 3) + ((r1 - l1) / 5)) / 150 */
    return ((ref_readings->r3 - ref_readings->l3) + (ref_readings->r2 - ref_readings->l2) / 3 + (ref_readings->r1 - ref_readings->l1) / 5)/150;
}

/* [] END OF FILE */
