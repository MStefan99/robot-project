
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

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

#define ZUMO_TITLE "Zumo025"

bool movement_allowed = false;
volatile bool calibration_mode = false;

CY_ISR_PROTO(Button_Interrupt);
bool incorrect_time(int hours, int minutes);

bool button_pressed = false; 

CY_ISR(Button_Interrupt)
{
    button_pressed = true;
    
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
    ADC_Battery_Start();
    ADC_Battery_StartConvert();    
    printf("Program initialized\n");
    PWM_Start();
    
    IR_Start();
    
    bool user_time_received = false;
    int hours = 0;
    int minutes = 0;
    
    RTC_Start();
    RTC_TIME_DATE now;
    
    for (;;) {  
        
        if(!voltage_test()){
            printf("Low voltage detected! Program will not continue unless sufficient voltage supplied.\n");
            PWM_Stop();
            vTaskDelay(1000);
            continue;
        }
        
        
        if (!user_time_received) {
            //MQTT logs has higher priority, so wait until MQTT connection is established before continue program.
            vTaskDelay(20000);
            printf("Welcome to Assignment 1 of Week 5. To continue program needs time in 24-hours format.\n");
        
            do {
                printf("Enter hours: ");
                scanf("%d", &hours);
                printf("Enter minutes: ");
                scanf("%d", &minutes); 
                
                if (incorrect_time(hours, minutes)) {
                    printf("Incorrect data, try again.\n");
                } else {
                    printf("You entered %d:%d\n", hours, minutes);
                    user_time_received = true;
                    
                    now.Hour = hours;
                    now.Min = minutes;
                    RTC_WriteTime(&now);
                }
            } while (incorrect_time(hours, minutes));
        }
        
        
        if (button_pressed) {
            RTC_DisableInt(); /* Disable Interrupt of RTC Component */
            now = *RTC_ReadTime(); /* copy the current time to a local variable */
            RTC_EnableInt(); /* Enable Interrupt of RTC Component */
            
            printf("%scurrent_time %2d:%02d.%02d\n", ZUMO_TITLE, now.Hour, now.Min, now.Sec);
            print_mqtt(ZUMO_TITLE, "current_time %2d:%02d.%02d", now.Hour, now.Min, now.Sec);

            button_pressed = false;
        }
        vTaskDelay(50);
    }
}

bool incorrect_time(int hours, int minutes) 
{
    bool incorrect_hours = hours < 0 || hours > 23;
    bool incorrect_minutes = minutes < 0 || minutes > 59; 
   
    return incorrect_hours || incorrect_minutes; 
}

/* [] END OF FILE */
