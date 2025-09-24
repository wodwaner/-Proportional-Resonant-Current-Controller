/*
 * Structs.h
 *
 *  Created on: 17 de jul de 2023
 *      Author: wodwa
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_
#include  <stdint.h>

#define N_SAMPLE_OFFSET 65535
#define QNT_SAMPLE_OFFSET 65535.0f

#define GAIN_I (0.01389f)
#define GAIN_V (0.01f)

#define IL_MAX (20.0f)
#define VAC_MAX (50.0f)
#define VDC_MAX (50.0f)

#ifndef _UINT8_T_DECLARED
typedef	unsigned char uint8_t;
#define	_UINT8_T_DECLARED
#endif

typedef struct{
    uint8_t PWM       :1;
    uint8_t Timing    :1;
    uint8_t New_Sample:1;
    uint8_t Over_Run  :1;
    uint8_t Over_iLa  :1;
    uint8_t Over_iLb   :1;
    uint8_t Over_iLc  :1;
    uint8_t Protection:1;
    uint8_t Qnt_pulse;
} st_Ctrl;

typedef enum {OK = 0, Reset = 1, Running} en_Offset;
typedef struct{
    float Ch0;
    float Ch1;
	float Ch2;
    uint16_t Cnt;
    en_Offset Calc;
} st_Offset;

typedef struct{
    float iLa;
	float iLb;
	float iLc;
} st_Measurements;
#endif /* STRUCTS_H_ */
