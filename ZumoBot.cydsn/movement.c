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

void motor_tank_turn(uint8_t direction, uint8_t l_speed, uint8_t r_speed)
{
    MotorDirLeft_Write(!direction);
    MotorDirRight_Write(direction);
    PWM_WriteCompare1(l_speed); 
    PWM_WriteCompare2(r_speed);
    
    MotorDirLeft_Write(0);
    MotorDirRight_Write(0);
}

void motor_turn_diff(uint8_t speed, int diff)
{
    uint8_t l_speed = speed;
    uint8_t r_speed = speed;
    if (abs(diff) > speed){
        if(diff>0){
            MotorDirLeft_Write(1);
            l_speed -= diff*speed/2000;
            r_speed = 0;
        } else {
            MotorDirRight_Write(1);
            r_speed -= diff*speed/2000;
            l_speed = 0;
        }
    } else {
        MotorDirLeft_Write(0);
        MotorDirRight_Write(0);
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
