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

#ifndef LOG_H_
#define LOG_H_

    #include <stdbool.h>
    #include <FreeRTOS.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <mqtt_sender.h>
    
    typedef struct {
        char *title;
        char *time;
    } log_entry;

    // Makes a valid log line from passed arguments (used for log_add)
    log_entry make_entry(char *title, char *time);
    
    // Adds a new log line
    bool log_add(char *title, char *time);
    
    // Adds an entry to the log with the time
    void log_time(char *title, TickType_t time);
    
    // Clears the log
    void log_clear();

    // Reads the desired log line (unsafe!)
    log_entry log_read(int index);

    // Outputs the entire log via serial connection (USB)
    void log_output();
    
    // Sends the entire log via mqtt
    void log_send();

#endif

/* [] END OF FILE */