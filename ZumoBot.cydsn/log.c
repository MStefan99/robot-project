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
static log_entry (*tmp)[] = NULL;
static int count = 0;
 
log_entry make_entry(char *title, char *time)
{
    log_entry log_item = {title, time};
    return log_item;
}  
    
bool log_add(char *title, char *time)
{
    log_entry data = make_entry(title, time);
    tmp = realloc(logs, sizeof(data) * (count + 1));
    if (tmp == NULL){
        return 1;
    } else {
        logs = tmp;
        (*logs)[count] = data;
        count++;
        return 0;
    }
}
    
void log_time(char *title, TickType_t time) 
{
    char *buf = malloc(sizeof(char) * 30);
    sprintf(buf, "%lu", (u_long)time);
    log_add(title, buf);
}

void log_clear()
{
    free(logs);
    count = 0;
    logs = malloc(sizeof((*logs)[0]) * (count + 1));
}

log_entry log_read(int index)
{
    return (*logs)[index];
}

void log_output()
{
    for(int i = 0; i < count; i++){
        printf("Data: %s\n"
               "Time: %s\n\n", (*logs)[i].title, (*logs)[i].time);
    }
}

void log_send()
{
    for(int i = 0; i < count; i++){
        print_mqtt((*logs)[i].title, "%s", (*logs)[i].time);
    }
}

/* [] END OF FILE */