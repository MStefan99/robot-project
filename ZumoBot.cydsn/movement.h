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

#ifndef MOVEMENT_H
#define MOVEMENT_H

    #include <stdint.h>

    // Does the tank turn of the robot. Allowed modes for direction: 0 (left), 1 (right).
    void motor_tank_turn(uint8_t direction, uint8_t speed, float delay);

    // Turns the robot with a desired speed speed using speed difference
    void motor_turn_diff(uint8_t speed, int diff);

#endif

/* [] END OF FILE */
