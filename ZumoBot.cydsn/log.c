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

    static log_entry (*logs)[] = NULL;
    static int count = 0;
 
log_entry make_entry(char *title, TickType_t time)
{
    log_entry log_item = {title, time};
    return log_item;
}  
    
void log_add(char *title, TickType_t time)
{
    log_entry data = make_entry(title, time);
    logs = realloc(logs, sizeof(data) * (count + 1));
    (*logs)[count] = data;
    count++;
}
    
log_entry log_read(int position)
{
    return (*logs)[position];
}

void log_output()
{
    for(int i = 0; i < count; i++){
        printf("Data: %s\n"
               "Time: %lu\n\n", (*logs)[i].title, (u_long)(*logs)[i].time);
    }
}

void log_send()
{
    for(int i = 0; i < count; i++){
        print_mqtt((*logs)[i].title, "%lu", (u_long)(*logs)[i].time);
    }
}
/* [] END OF FILE */
