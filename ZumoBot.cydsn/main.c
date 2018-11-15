
#include <project.h>
#include <stdio.h>
#include <stdlib.h>
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
#include <movement.h>

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/


bool movement_allowed = false;
volatile bool calibration_mode = false;
int get_turn_direction();
void obstacle_avoid();

CY_ISR_PROTO(Button_Interrupt);

CY_ISR(Button_Interrupt)
{
    movement_allowed = true;
    calibration_mode = true;
    SW1_ClearInterrupt();
}



int zmain(void)
{    
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    Button_isr_StartEx(Button_Interrupt); // Link button interrupt to isr
    reflectance_start();
    UART_1_Start();    
    Ultra_Start();
    ADC_Battery_Start();
    ADC_Battery_StartConvert();    
    PWM_Start();
    IR_Start();
    printf("Program initialized\n");
        
    for (;;) {  
        
        if(!voltage_test()){
            printf("Low voltage detected! Program will not continue unless sufficient voltage supplied.\n");
            PWM_Stop();
            vTaskDelay(1000);
            continue;
        }
        
        
        
        obstacle_avoid(get_turn_direction());
    }

}

//0 - left, 1 - right
int get_turn_direction() {
    int random_direction = rand() % 2;
    if (random_direction) {
        printf("Right\n");
    } else {
        printf("Left\n");
    }
        
    return random_direction;
}
   
void obstacle_avoid(int direction){    
    int distance=20;
    distance=Ultra_GetDistance();
    printf("Distance is: %d cm\n", distance);
    vTaskDelay(50);        
        
    if(distance < 10){
        motor_forward(0, 0); 
        Beep(100, 50);
        vTaskDelay(20);
        Beep(100, 50);
        motor_tank_turn(direction, 200, 250);
    } else {
        motor_forward(200, 0); 
    }
}

/* [] END OF FILE */
