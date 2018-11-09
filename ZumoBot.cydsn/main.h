/**
 * @file    main.h
 * @brief   main header file
*/

#include <stdbool.h>
#include <stdint.h>

#ifndef MAIN_H_
#define MAIN_H_

extern volatile bool calibration_mode;
volatile bool calibration_mode = false;

typedef struct sensors_difference_{
    int16_t sensor1;
    int16_t sensor2;
    int16_t sensor3;    
    }reflectance_offset_;

#endif
/* [] END OF FILE */
