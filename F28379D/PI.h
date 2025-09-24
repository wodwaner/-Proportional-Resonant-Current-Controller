/* =================================================================================
File name:       PID_REG3.H  (IQ version)                    
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Header file containing constants, data type definitions, and 
function prototypes for the PIDREG3.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
------------------------------------------------------------------------------*/
#ifndef __PIREG_H__
#define __PIREG_H__

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

// ======================================================
#pragma FUNC_ALWAYS_INLINE(In_PI_Calc)
inline void In_PI_Calc(st_PID *v){
    v->Up = v->Kp*v->Err;
    v->Ui = v->Ui + v->Ki*(v->Err+v->Err1)*0.5 + (v->Kc*v->SatErr);
    v->OutPreSat = v->Up + v->Ui +v->Feedforward;
    if (v->OutPreSat > v->OutMax)  v->Out =  v->OutMax;
    else if (v->OutPreSat < v->OutMin) v->Out =  v->OutMin;
    else v->Out = v->OutPreSat;
    v->SatErr = v->Out - v->OutPreSat;
    v->Err1 = v->Err;
}
#pragma FUNC_ALWAYS_INLINE(In_PI_Reset)
inline void In_PI_Reset(st_PID *v){
    v->Err = 0;
    v->Out = 0;
    v->OutPreSat = 0;
    v->Ui = 0;
    v->Err1 = 0;
    v->SatErr = 0;
    v->Out = 0;
}
#endif // __PIDREG3_H__
