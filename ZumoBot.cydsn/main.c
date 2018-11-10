
#include <project.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>
#include <voltage.h>
#include <line_detection.h>

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

// Does the tank turn of the robot. Allowed modes for direction: 0 (left), 1 (right).
void motor_tank_turn(uint8 direction, uint8 l_speed, uint8 r_speed, uint32 delay);

// Turns the robot with a desired speed speed using speed difference
void motor_turn_diff(uint8 speed, int16 diff);

bool movement_allowed = false;
volatile bool calibration_mode = false;

CY_ISR_PROTO(Button_Interrupt);

CY_ISR(Button_Interrupt)
{
    movement_allowed = true;
    calibration_mode = true;
    SW1_ClearInterrupt();
}




int zmain(void)
{    
    reflectance_offset_ reflectance_offset = {0,0,0};
    struct sensors_ reflectance_values;
    bool reflectance_black = false;
    uint8_t line_count = 0;
    int shift;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    Button_isr_StartEx(Button_Interrupt); // Link button interrupt to isr
    
    reflectance_start();
    UART_1_Start();    
    ADC_Battery_Start();
    ADC_Battery_StartConvert();    
    printf("Program initialized\n");
    PWM_Start();
    
    for (;;) {  
        
        if(!voltage_test()){
            printf("Low voltage detected! Program will not continue unless sufficient voltage supplied.\n");
            PWM_Stop();
            vTaskDelay(1000);
            continue;
        }
        
        if(!line_detect()){
            if(reflectance_black){
                ++line_count;
            }
            reflectance_black = false;
        } else {
            reflectance_black = true;
        }
        
        if(calibration_mode){
            reflectance_read(&reflectance_values);
            reflectance_offset = reflectance_calibrate(&reflectance_values);
            calibration_mode=false;
        } 
        
        reflectance_read(&reflectance_values);
        shift = reflectance_normalize(&reflectance_values, &reflectance_offset);
        
        if(movement_allowed){
            if(line_count > 3){
                motor_forward(0,0);
            } else {
                motor_turn_diff(100, shift);
            } 
        }
    }
}

void motor_tank_turn(uint8 direction, uint8 l_speed, uint8 r_speed, uint32 delay)
{
    MotorDirLeft_Write(!direction);      // set left motor direction
    MotorDirRight_Write(direction);     // set right motor direction
    PWM_WriteCompare1(l_speed); 
    PWM_WriteCompare2(r_speed); 
    vTaskDelay(delay);    
    
    MotorDirLeft_Write(0);
    MotorDirRight_Write(0);
}

void motor_turn_diff(uint8 speed, int16 diff){
    uint8 l_speed = speed;
    uint8 r_speed = speed;
    if (diff > speed || -diff < -speed){
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
