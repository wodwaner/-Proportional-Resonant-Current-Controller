
#ifndef __PIREG_H__
#define __PIREG_H__
#include "Structs.h"
typedef struct {
    float  Kp;              // Parameter: Proportional gain
    float  Ki;              // Parameter: Integral gain
    float  Kc;              // Parameter: Integral correction gain
    float  Up;              // Variable: Proportional output
    float  Ui;              // Variable: Integral output
    float  OutPreSat;       // Variable: Pre-saturated output
    float  OutMax;          // Parameter: Maximum output
    float  OutMin;          // Parameter: Minimum output
    float  Out;             // Output: PID output
    float  SatErr;          // Variable: Saturated difference
    float  Err;             // Variable: Error
    float  Err1;                // History: Previous error
    float  Feedforward;                // FEEDFORWARD
} st_PID;


void PI_Calc(st_PID *v);
void PI_Reset(st_PID *v);

/*-----------------------------------------------------------------------------
Default initalizer for the PIDREG3 object.
-----------------------------------------------------------------------------*/

#define PID_DEFAULTS {0,0,1.0,0,0,0,0,0,0,0,0,0,0}

#endif // __PIDREG3_H__
