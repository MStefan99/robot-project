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
    
    typedef struct {
        char *title;
        TickType_t time;
    } log_entry;
    
    // Makes a valid log entry from passed arguments
    log_entry make_entry(char* title, TickType_t time);    

    // Adds a new log entry
    void log_add(log_entry** log, log_entry data);

    // Writes log entry at the desired line (unsafe!)
    void log_write(log_entry** log, int position, log_entry data);

    // Reads the log at the desired line
    log_entry log_read(log_entry** log, int position);

    // Outputs the log via serial connection
    void log_output(log_entry** log);

    // Sends the log via mqtt
    void log_send(log_entry** log);

#endif

/* [] END OF FILE */
