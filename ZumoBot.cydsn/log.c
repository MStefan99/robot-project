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
 
log_entry make_entry(char *title, char *data)
{
    log_entry log_item = {title, data};
    return log_item;
}  
    
void log_add(char *title, char *data)
{
    log_entry entry = make_entry(title, data);
    logs = realloc(logs, sizeof(data) * (count + 1));
    (*logs)[count] = entry;
    count++;
}
    
void log_add_time(char *title, TickType_t time) {
    char *buf = malloc(sizeof(char) * 20);
    sprintf(buf, "%u", time);
    log_add(title, buf);
}

log_entry log_read(int position)
{
    return (*logs)[position];
}

void log_output()
{
    for(int i = 0; i < count; i++){
        printf("Title: %s\n"
               "Data: %s\n\n", (*logs)[i].title, (*logs)[i].data);
    }
}

void log_send()
{
    for(int i = 0; i < count; i++){
        print_mqtt((*logs)[i].title, "%s", (*logs)[i].data);
    }
}
/* [] END OF FILE */
