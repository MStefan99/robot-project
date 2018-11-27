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

#include <log.h>
#include <stdlib.h>
#include <stdio.h>
#include <mqtt_sender.h>

//static log_entry log[0];

log_entry make_entry(char* title, TickType_t time){
    log_entry log_item = {title, time};
    return log_item;
}

void log_add(log_entry** log, log_entry data){
    log_entry* old = *log;
    log_entry* new;
    int entry_count = sizeof(log) / sizeof(log_entry);
    new = realloc(old, (++entry_count) * sizeof(log_entry));
    *log = new;
    *log[entry_count] = data;
    free(old);
    
}

void log_write(log_entry** log, int position, log_entry data){
    *log[position] = data;
}

log_entry log_read(log_entry** log, int position){
    return *log[position];
}

void log_output(log_entry** log){
    int entry_count = sizeof(log) / sizeof(log_entry);
    for(int i = 0; i < entry_count; i++){
        printf("Data: %s\n"
            "Time: %lu\n\n", log[i]->title, log[i]->time);
    }
}

void log_send(log_entry** log){
    int entry_count = sizeof(log) / sizeof(log_entry);
        for(int i = 0; i < entry_count; i++){
            print_mqtt(log[i]->title, "%lu", log[i]->time);
        }
}

/* [] END OF FILE */
