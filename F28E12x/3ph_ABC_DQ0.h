/*
 * 3ph_ABC_DQ0.h
 *
 *  Created on: 27 de mai de 2023
 *      Author: waner
 */

#ifndef ABC_DQ0_H_
#define ABC_DQ0_H_

typedef struct{
    float a;
    float b;
    float c;
    float alpha;
    float beta;
    float sin;
    float cos;
    float d;
    float q;
    float z;
}st_DQ0_ABC;

void ABC_DQ0_Init(st_DQ0_ABC *v);
void DQ0_to_ABC(st_DQ0_ABC *v);
void AB_to_ABC(st_DQ0_ABC *v);
void ABC_to_DQ0(st_DQ0_ABC *v);


#pragma FUNC_ALWAYS_INLINE(In_DQ0_to_ABC)
inline void In_DQ0_to_ABC(st_DQ0_ABC *v){
    v->alpha = v->d*v->cos - v->q*v->sin;
    v->beta  = v->d*v->sin + v->q*v->cos;
    v->a = v->alpha + 0.5f*v->z;
    v->b = -0.5f*v->alpha + 0.8660254f*v->beta + 0.5f*v->z;
    v->c = -0.5f*v->alpha - 0.8660254f*v->beta + 0.5f*v->z;
}

#pragma FUNC_ALWAYS_INLINE(In_ABC_to_DQ0)
inline void In_ABC_to_DQ0(st_DQ0_ABC *v){
    v->alpha=(0.6666666667f)*(v->a-0.5f*(v->b+v->c));
    v->beta=(0.57735026913f)*(v->b-v->c);
    v->z =0.57735026913f*(v->a+v->b+v->c);
    v->d=v->alpha*v->cos+v->beta*v->sin;
    v->q=-v->alpha*v->sin+v->beta*v->cos;
}

#endif
