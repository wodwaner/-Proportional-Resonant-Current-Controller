/*
 * 3ph_ABC_DQ0.c
 *
 *  Created on: 27 de mai de 2023
 *      Author: waner
 */

#include "3ph_ABC_DQ0.h"

void ABC_DQ0_Init(st_DQ0_ABC *v){
    v->a=0;
    v->b=0;
    v->c=0;
    v->alpha=0;
    v->beta=0;
    v->z=0;
    v->d=0;
    v->q=0;
}

void DQ0_to_ABC(st_DQ0_ABC *v){
    v->alpha = v->d*v->cos - v->q*v->sin;
    v->beta  = v->d*v->sin + v->q*v->cos;
    v->a = v->alpha + 0.5*v->z;
    v->b = -0.5f*v->alpha + 0.8660254f*v->beta + 0.5f*v->z;
    v->c = -0.5f*v->alpha - 0.8660254f*v->beta + 0.5f*v->z;
}

void AB_to_ABC(st_DQ0_ABC *v){
    v->a = v->alpha + 0.5*v->z;
    v->b = -0.5f*v->alpha + 0.8660254f*v->beta + 0.5f*v->z;
    v->c = -0.5f*v->alpha - 0.8660254f*v->beta + 0.5f*v->z;
}

void ABC_to_DQ0(st_DQ0_ABC *v){
    v->alpha=(0.6666666667f)*(v->a-0.5f*(v->b+v->c));
    v->beta=(0.57735026913f)*(v->b-v->c);
    v->z =0.57735026913f*(v->a+v->b+v->c);
    v->d=v->alpha*v->cos+v->beta*v->sin;
    v->q=-v->alpha*v->sin+v->beta*v->cos;
}
