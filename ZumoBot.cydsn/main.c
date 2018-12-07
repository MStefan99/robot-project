
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
#include <log.h>
#include <movement.h>

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

#define ZUMO_TITLE_READY "Zumo033/ready"
#define ZUMO_TITLE_START "Zumo033/start"
#define ZUMO_TITLE_HIT   "Zumo033/hit"
#define ZUMO_TITLE_STOP  "Zumo033/stop"
#define ZUMO_TITLE_TIME  "Zumo033/time"

static const uint8_t speed = 100;
static const int cross_to_stop_on = 2;
static const float pi = 3.14159265359;

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
    struct accData_ data;
    bool reflectance_black = false;
    uint8_t cross_count = 0;
    const uint8_t speed = 255;
    int threshold = 4000;
    double angle = 0;
    bool new_cross_detected = false;
    bool hit_detected = false;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    Button_isr_StartEx(Button_Interrupt); // Link button interrupt to isr
    
    reflectance_start();
    UART_1_Start();    
    ADC_Battery_Start();
    ADC_Battery_StartConvert();  
    LSM303D_Start();
    PWM_Start();
    IR_Start();
    printf("Program initialized\n");
    print_mqtt(ZUMO_TITLE_READY, "sumo");
    
    for (;;) {  
        
        if(!voltage_test()){
            printf("Low voltage detected! Program will not continue unless sufficient voltage supplied.\n");
            PWM_Stop();
            vTaskDelay(1000);
            continue;
        }
        
        if(!cross_detected()){
            if(reflectance_black){
                // Update cross count after leaving the intersection
                ++cross_count;
                new_cross_detected = true;
            }
            reflectance_black = false;
        } else {
            reflectance_black = true;
        }
        
        if(calibration_mode){
            reflectance_read(&reflectance_values);
            reflectance_offset = reflectance_calibrate(&reflectance_values);
            calibration_mode = false;
        } 
        
        reflectance_read(&reflectance_values);
        reflectance_normalize(&reflectance_values, &reflectance_offset);
        
        if (movement_allowed) {
            if (new_cross_detected && cross_count == 1) {
                motor_forward(0, 0);
                IR_flush();
                IR_wait();
                print_mqtt(ZUMO_TITLE_START, "%u", xTaskGetTickCount()); // TODO MStefan99: add logging
                motor_forward(50, 500);
                new_cross_detected = false;
            } else if (cross_count < 1) {
                
                motor_forward(speed, 0);
            } else {
                if (line_detected(2)){
                    motor_tank_turn(1, speed, speed * 1.2);
                }
            }
            
            LSM303D_Read_Acc(&data);
            if((abs(data.accX) > threshold || abs(data.accY) > threshold) && !hit_detected){
                angle = - ( atan2( (float)data.accY, (float)data.accX ) - pi ) * 180 / pi; // TODO MStefan99: simplify                       
                print_mqtt(ZUMO_TITLE_HIT, "%u %f", xTaskGetTickCount(), angle); // TODO MStefan99: add logging
                hit_detected = true;
            } 
            
            if(abs(data.accX) < threshold && abs(data.accY) < threshold){
                hit_detected = false;
            }    
        } // TODO MStefan99: add end condition
    }
}

/* [] END OF FILE */
