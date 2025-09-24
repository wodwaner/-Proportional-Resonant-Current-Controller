/*
 * Dlog.h
 *
 *  Created on: 15 de fev de 2022
 *      Author: wodwa
 */

#ifndef __DLOG_H__
#include "Structs.h"
#define __DLOG_H__

#define SIZE_LOG 300


//*********** Structure Definition ********//
typedef enum {WAIT_TRIGER = 0, TRIGGERED = 1, NO_TRIGGER = 2, CAPTURED} log_states;

typedef struct{
    float *input_ch1;
	float *input_ch2;
	float *input_ch3;
    float *input_trigger;
    float log_ch1[SIZE_LOG];
	float log_ch2[SIZE_LOG];
	float log_ch3[SIZE_LOG];

    float prev_value;
    float trig_value;
    log_states status;
    unsigned char pre_scalar;
    unsigned char skip_count;
    unsigned int count;
}st_DLOG;


void DLOG_Init(st_DLOG *v);
void DLOG_Func(st_DLOG *v);

#endif /* DLOG_H_ */
