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

#include <movement.h>
#include <Motor.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>

void motor_tank_turn(uint8_t direction, uint8_t speed, float delay)
{
    MotorDirLeft_Write(!direction);
    MotorDirRight_Write(direction);
    PWM_WriteCompare1(speed); 
    PWM_WriteCompare2(speed);
    
    vTaskDelay(delay);
    
    MotorDirLeft_Write(0);
    MotorDirRight_Write(0);
}

void motor_turn_diff(uint8_t speed, int diff)
{
    uint8_t l_speed = speed;
    uint8_t r_speed = speed;
    if (abs(diff) > speed){
        if(diff>0){
            r_speed=0;
        } else {
            l_speed=0;
        }
    } else {
        if (diff > 0){
           r_speed -= diff*speed/255;
       } else {
           l_speed += diff*speed/255;
       }
    }
    
    PWM_WriteCompare1(l_speed); 
    PWM_WriteCompare2(r_speed);
}

/* [] END OF FILE */
