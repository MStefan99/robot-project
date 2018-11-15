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
#include <FreeRTOS.h>
#include <task.h>
#include <stdlib.h>
#include <stdio.h>

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
    static struct sensors_ ref_previous;
    static bool line_lost;
    static bool line_lost_direction;
    int low_pass = 8000;
    int high_pass = 20000;
    int setpoint_value_inner = 21500;
    int setpoint_value_outer = 5000;
    
    int delta_r1 = ref_readings->r1 - setpoint_value_inner;
    int delta_l1 = setpoint_value_inner - ref_readings->l1;
    int delta_r2 = ref_readings->r2 - setpoint_value_outer;
    int delta_l2 = setpoint_value_outer - ref_readings->l2;
    int delta_r3 = ref_readings->r3 - setpoint_value_outer;
    int delta_l3 = setpoint_value_outer - ref_readings->l3;
    
    if (ref_previous.r3 > high_pass && ref_readings->r3 < low_pass && ref_readings->r2 < low_pass) {
        line_lost = true;
        line_lost_direction = true;
    } else if (ref_previous.l3 > high_pass && ref_readings->l3 < low_pass && ref_readings->l2 < low_pass) {
        line_lost = true;
        line_lost_direction = false;
    } else if (ref_readings->r3 < high_pass || ref_readings->l3 < high_pass){
        line_lost = false;
    }
    
    if (line_lost) {
        if(line_lost_direction){
        return 255;
        } else {
        return -255;
        }
    }
    
    ref_previous = *ref_readings;
    
    return (delta_r1 + delta_l1 + delta_r2 + delta_l2 + delta_r3 + delta_l3) / 180;
}

int get_offset_change(struct sensors_ *ref_readings){
    
    static int previous_offset;
    static TickType_t previous_time;
    
    int offset_change = (get_offset(ref_readings) - previous_offset) * 300 / (int)(xTaskGetTickCount() - previous_time);
    previous_time = xTaskGetTickCount();
    previous_offset = get_offset(ref_readings);
    
    return offset_change;
}

/* [] END OF FILE */
