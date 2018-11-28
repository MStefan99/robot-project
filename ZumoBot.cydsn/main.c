
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
#include <movement.h>
#include <log.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

#define ZUMO_TITLE_READY "Zumo033/ready"
#define ZUMO_TITLE_START "Zumo033/start"
#define ZUMO_TITLE_STOP "Zumo033/stop"
#define ZUMO_TITLE_TIME "Zumo033/time"

#define ZUMO_TITLE_MISS "Zumo033/miss"
#define ZUMO_TITLE_LINE "Zumo033/line"

static const uint8_t speed = 100;
static const int cross_to_stop_on = 3;

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
    bool new_cross_detected = false;
    bool track_completed = false;
    bool line_was_lost = false;
    bool logged_lost_line = false;
    uint8_t cross_count = 0;
    const uint8_t speed = 255;
    int line_shift_change;
    int line_shift;
    int shift_correction;
    float p_coefficient = 2.5;
    float d_coefficient = 4;    
    TickType_t start_time;
    TickType_t end_time;
    log_entry* logs_array = malloc(sizeof(log_entry));
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    Button_isr_StartEx(Button_Interrupt); // Link button interrupt to isr
    
    reflectance_start();
    UART_1_Start();    
    ADC_Battery_Start();
    ADC_Battery_StartConvert();  
    printf("Program initialized\n");
    print_mqtt(ZUMO_TITLE_READY, "line");
    PWM_Start();    
    IR_Start();
    
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
        
        line_shift = get_offset(&reflectance_values);
        line_shift_change = get_offset_change(&reflectance_values);        
        shift_correction = line_shift * p_coefficient + line_shift_change * d_coefficient;
        
        if (movement_allowed) {
            if (new_cross_detected && cross_count == 2) {
                motor_forward(0,0);
                
                IR_flush();
                IR_wait();
                
                start_time = xTaskGetTickCount();
                //print_mqtt(ZUMO_TITLE_START, "%ld", start_time);
                log_add(&logs_array, make_entry(ZUMO_TITLE_START, start_time));
                
                motor_turn_diff(speed, shift_correction);
                new_cross_detected = false;
            } else if (cross_count > cross_to_stop_on) {
                motor_forward(0,0);
                
                if (!track_completed) {
                    end_time = xTaskGetTickCount();
                    log_add(&logs_array, make_entry(ZUMO_TITLE_STOP, end_time));
                    
                    TickType_t result = end_time - start_time;
                    log_add(&logs_array, make_entry(ZUMO_TITLE_TIME, result));
                    
                    track_completed = true;
                    
                    log_send(&logs_array);
                }
            } else {
                motor_turn_diff(speed, shift_correction);
                new_cross_detected = false;
                
                if (cross_count > 1) {
                    //log only if track started
                    if (check_if_following_line() == 0 && line_was_lost) {
                        line_was_lost = false;
                        logged_lost_line = false;
                        
                        log_add(&logs_array, make_entry(ZUMO_TITLE_LINE, xTaskGetTickCount()));
                    } else if (check_if_following_line() != 0 && !logged_lost_line) {
                        logged_lost_line = true;
                        line_was_lost = true;
                        
                        log_add(&logs_array, make_entry(ZUMO_TITLE_MISS, xTaskGetTickCount()));
                    }
                }
                
            }
        }
    }
}

/* [] END OF FILE */
