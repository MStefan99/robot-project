
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


int get_turn_direction();
int obstacle_found();


int zmain(void)
{    
    CyGlobalIntEnable; // Enable global interrupts.
    UART_1_Start();    
    Ultra_Start();
    ADC_Battery_Start();
    ADC_Battery_StartConvert();    
    PWM_Start();
    printf("Program initialized\n");
        
    for (;;) {  
        
        if(!voltage_test()){
            printf("Low voltage detected! Program will not continue unless sufficient voltage supplied.\n");
            PWM_Stop();
            vTaskDelay(1000);
            continue;
        }
        
        
        if (obstacle_found()){
        motor_tank_turn(get_turn_direction(), 200, 250);
        } else {
        motor_forward(200, 0);
        }
    }

}

// 0 - left, 1 - right
int get_turn_direction() {
    int random_direction = rand() % 2;
    if (random_direction) {
        print_mqtt("Zumo025/turn", "Right");
    } else {
        print_mqtt("Zumo025/turn", "Left");
    }
        
    return random_direction;
}
   
int obstacle_found(){    
    int distance;
    distance=Ultra_GetDistance();     
        
    if(distance < 10){
        return 1;        
    } else {
        return 0;
    }
}

/* [] END OF FILE */
