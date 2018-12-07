
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
#define ZUMO_TITLE_STOP "Zumo033/stop"
#define ZUMO_TITLE_TIME "Zumo033/time"
#define ZUMO_TITLE_POSITION "Zumo033/position"

static const uint8_t speed = 150;
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

robot_position current_position = { 0, 0, forward };

bool did_detect_obstacle();
void update_position();
void turn(robot_direction direction_to_turn);
void log_time(char *title, TickType_t time);
void handle_detect_obstacle(bool *saw_block, robot_direction *previous_direction);


int zmain(void)
{    
    reflectance_offset_ reflectance_offset = {0,0,0};
    struct sensors_ reflectance_values;
    bool reflectance_black = false;
    uint8_t cross_count = 0;
    int line_shift_change;
    int line_shift;
    int shift_correction;
    float p_coefficient = 2.5;
    float d_coefficient = 4;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    Button_isr_StartEx(Button_Interrupt); // Link button interrupt to isr
    
    reflectance_start();
    UART_1_Start(); 
    Ultra_Start();
    ADC_Battery_Start();
    ADC_Battery_StartConvert();  
    printf("Program initialized\n");
    print_mqtt(ZUMO_TITLE_READY, "maze");
    PWM_Start();
    
    IR_Start();
    bool new_cross_detected = false;
    bool position_updated = true;
    bool logs_printed = false;
    bool saw_block = false;
    robot_direction previous_direction = forward; 
    TickType_t start_time;
    TickType_t end_time;
    bool maze_finished = false;
    
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
        
        if(calibration_mode) {
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
                log_time(ZUMO_TITLE_START, start_time);
                
                motor_turn_diff(speed, shift_correction);
                new_cross_detected = false;
            } else if (new_cross_detected && current_position.y == 13 && current_position.x == 0) { // the end of the maze. 
                position_updated = false;
                new_cross_detected = false;
                maze_finished = true;
                
                end_time = xTaskGetTickCount();
                log_time(ZUMO_TITLE_STOP, end_time);
                log_time(ZUMO_TITLE_TIME, end_time - start_time); 
            } else if (new_cross_detected && cross_count > 2 && current_position.y < 11) {
                if (did_detect_obstacle() && current_position.direction == forward) {
                    handle_detect_obstacle(&saw_block, &previous_direction);
                } else {
                    if (current_position.direction == forward) {
                        saw_block = false;
                    }
                    
                    if (current_position.direction == left) {
                        turn(right);
                    } else if (current_position.direction == right) {
                        turn(left);
                    }
                    
                    current_position.direction = forward;
                    
                    if (did_detect_obstacle()) {
                        handle_detect_obstacle(&saw_block, &previous_direction);
                    }
                }
                
                new_cross_detected = false;
                position_updated = false;
            } else if (new_cross_detected && current_position.y == 11 && current_position.x != 0) {
                if (current_position.x < 0 && current_position.direction != right) {
                    turn(right);
                    current_position.direction = right;
                } else if (current_position.x > 0 && current_position.direction != left) {
                    turn(left);
                    current_position.direction = left;
                }
                
                new_cross_detected = false;
                position_updated = false;
            } else if (new_cross_detected && current_position.y == 11 && current_position.x == 0) {
                if (current_position.direction == left) {
                    turn(right);
                } else if (current_position.direction == right) {
                    turn(left);
                }
                
                current_position.direction = forward;
                
                new_cross_detected = false;
                position_updated = false;
            } else if (new_cross_detected && current_position.y == 12 && current_position.x == 0) {
                new_cross_detected = false;
                position_updated = false;
            } else { // moving forward to the next cross 
                if (maze_finished) {
                    if (!logs_printed) {
                        motor_forward(speed / 2, 1200);
                        motor_forward(0,0);
                        
                        logs_printed = true;
                        log_send();
                    }
                } else {
                    motor_turn_diff(speed, shift_correction);
                    new_cross_detected = false; //TODO msaveleva: remove?
                
                    if (!position_updated) {
                        update_position();
                        position_updated = true;
                    }
                }
            }
        }
    }
}

void handle_detect_obstacle(bool *saw_block, robot_direction *previous_direction) {
    if (*saw_block) {
        turn(*previous_direction);
        current_position.direction = *previous_direction;
    } else if (current_position.x >= 0) {
        turn(left);
        current_position.direction = left;
        *previous_direction = left;
    } else {
        turn(right);
        current_position.direction = right;
        *previous_direction = right;
    }

    *saw_block = true;
}

void turn(robot_direction direction_to_turn) {
    if (direction_to_turn == left) {
        motor_tank_turn(0, speed, 300);
    } else if (direction_to_turn == right) {
        motor_tank_turn(1, speed, 300);
    }
}

bool did_detect_obstacle() {
    int distance = Ultra_GetDistance();
    printf("distance: %lu\n", (u_long)distance);
    
    return distance <= 22;
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

    char *buf = malloc(sizeof(char) * 20);
    sprintf(buf, "%d %d", current_position.x, current_position.y);
    
    log_add(ZUMO_TITLE_POSITION, buf);
}

void log_time(char *title, TickType_t time) {
    char *buf = malloc(sizeof(char) * 20);
    sprintf(buf, "%d", time);
    log_add(title, buf);
}

/* [] END OF FILE */
