#include "PI.h"


void PI_Calc(st_PID *v){
    // Compute the proportional output
    v->Up = v->Kp*v->Err;
    // Compute the integral output
    v->Ui = v->Ui + v->Ki*(v->Err+v->Err1) + v->Kc*v->SatErr;
    // Compute the derivative output
    //v->Ud = _IQmpy(v->Kd,(v->Err - v->Err1));
    // Compute the pre-saturated output
    v->OutPreSat = v->Up + v->Ui +v->Feedforward;
    // Saturate the output
    if (v->OutPreSat > v->OutMax)  v->Out =  v->OutMax;
    else if (v->OutPreSat < v->OutMin) v->Out =  v->OutMin;
    else v->Out = v->OutPreSat;
    // Compute the saturate difference
    v->SatErr = v->Out - v->OutPreSat;
    // Update the previous error
    v->Err1 = v->Err;
}

void PI_Reset(st_PID *v){
    v->Err = 0;
    v->Err1 = 0;
    v->Ui = 0;
    v->Out = 0;
    v->OutPreSat = 0;
    //v->Ud = 0;
    v->SatErr = 0;
    v->Out = 0;
}






