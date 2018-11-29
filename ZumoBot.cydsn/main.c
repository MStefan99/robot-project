
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

#define ZUMO_TITLE_READY "Zumo025/ready"
#define ZUMO_TITLE_START "Zumo025/start"
#define ZUMO_TITLE_STOP "Zumo025/stop"
#define ZUMO_TITLE_TIME "Zumo025/time"

static const uint8_t speed = 100;
static const int cross_to_stop_on = 2;

bool movement_allowed = false;
volatile bool calibration_mode = false;

CY_ISR_PROTO(Button_Interrupt);

CY_ISR(Button_Interrupt)
{
    movement_allowed = true;
    calibration_mode = true;
    SW1_ClearInterrupt();
}

typedef enum {
    forward,
    right,
    back, 
    left
} robot_direction;


typedef struct {
    int x;
    int y;
    robot_direction direction;
} robot_position; 

robot_position current_position = { 0, 0, forward};

bool did_detect_obstacle();
void update_position();
void turn(robot_direction direction_to_turn, int line_shift);


int zmain(void)
{    
    reflectance_offset_ reflectance_offset = {0,0,0};
    struct sensors_ reflectance_values;
    bool reflectance_black = false;
    uint8_t cross_count = 0;
    const uint8_t speed = 255;
    int line_shift_change;
    int line_shift;
    int shift_correction;
    float p_coefficient = 2.5;
    float d_coefficient = 4;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    Button_isr_StartEx(Button_Interrupt); // Link button interrupt to isr
    
    reflectance_start();
    UART_1_Start();    
    ADC_Battery_Start();
    ADC_Battery_StartConvert();  
    printf("Program initialized\n");
    PWM_Start();
    
    IR_Start();
    bool new_cross_detected = false;
    bool position_updated;
    
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
                
                motor_turn_diff(speed, shift_correction);
                new_cross_detected = false;
            } else if (current_position.x == 0 && current_position.y == 13) { // the end of the maze. 
                //TODO msaveleva: move forward for a while before stop. 
                motor_forward(0,0);
                position_updated = false;
            } else if (new_cross_detected && cross_count > 2) {
                if (did_detect_obstacle()) {
                    if (current_position.x >= 0) {
                        turn(left, line_shift);
                        current_position.direction = left;
                    } else {
                        turn(right, line_shift);
                        current_position.direction = right;
                    }
                } else {
                    if (current_position.direction == left) {
                        turn(right, line_shift);
                    } else if (current_position.direction == right) {
                        turn(left, line_shift);
                    }
                    
                    current_position.direction = forward;
                }
                
                new_cross_detected = false;
                position_updated = false;
            } else { // moving forward between to the next cross. 
                motor_turn_diff(speed, shift_correction);
                new_cross_detected = false;
                
                if (!position_updated) {
                    update_position();
                    position_updated = true;
                }
            }
        }
    }
}

void turn(robot_direction direction_to_turn, int line_shift) {
    if (direction_to_turn == left) {
        motor_tank_turn(0, speed, 500);
	    motor_turn_diff(speed, line_shift);
    } else if (direction_to_turn == right) {
        motor_tank_turn(1, speed, 600);
	    motor_turn_diff(speed, line_shift);
    }
}

bool did_detect_obstacle() {
    //TODO msaveleva: implement. 
    return true;
}

void update_position() {
    switch (current_position.direction) {
        case forward:
        current_position.y += 1;
        break;
        
        case back:
        current_position.y -= 1;
        break;
        
        case left:
        current_position.x -= 1;
        break;
        
        case right:
        current_position.x += 1;
        break;
        
        default:
        break;
    }
}

/* [] END OF FILE */
