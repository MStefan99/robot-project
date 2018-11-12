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

void reflectance_normalize(struct sensors_ *ref_readings, struct sensors_difference_ *ref_offset)
{
    ref_readings->r1 -= ref_offset->sensor1;
    ref_readings->r2 -= ref_offset->sensor2;
    ref_readings->r3 -= ref_offset->sensor3;
}

int get_offset(struct sensors_ *ref_readings){
    int delta_1 = ref_readings->r1 - ref_readings->l1;
    int delta_2 = ref_readings->r2 - ref_readings->l2;
    int delta_3 = ref_readings->r3 - ref_readings->l3;
    int delta_r2 = ref_readings->r2 - ref_readings->r1;
    int delta_l2 = ref_readings->l1 - ref_readings->l2;
    int delta_r3 = ref_readings->r3 - ref_readings->r2;
    int delta_l3 = ref_readings->l2 - ref_readings->l3;
    
    return (delta_3 + delta_2 + delta_1 + delta_r2 +  delta_l2 + delta_r3+ delta_l3) / 120;
}

int get_offset_change(struct sensors_ *ref_readings){
    static int previous_offset;
    int offset_change = (get_offset(ref_readings) - previous_offset) * 2;
    previous_offset = get_offset(ref_readings);
    return offset_change;
}

/* [] END OF FILE */
