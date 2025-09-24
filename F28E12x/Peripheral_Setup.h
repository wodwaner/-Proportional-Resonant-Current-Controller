/*
 * Peripheral_Setup.h
 *
 *  Created on: 23 de jul de 2020
 *      Author: waner
 */

#ifndef PERIPHERAL_SETUP_H_
#define PERIPHERAL_SETUP_H_
#include "F28x_Project.h"

//Configures which trigger will start a burst conversion sequence.
#define TRIG_SEL_SOFT   0x00
#define TRIG_SEL_TIM0   0x01
#define TRIG_SEL_TIM1   0x02
#define TRIG_SEL_TIM2   0x03
#define TRIG_SEL_GPIO   0x04
#define TRIG_SEL_ePWM1_SOCA 0x05
#define TRIG_SEL_ePWM1_SOCB 0x06
#define TRIG_SEL_ePWM1_SOCC 0x07
#define TRIG_SEL_ePWM1_SOCD 0x08
#define TRIG_SEL_ePWM2_SOCA 0x09
#define TRIG_SEL_ePWM2_SOCB 0x0A
#define TRIG_SEL_ePWM2_SOCC 0x0B
#define TRIG_SEL_ePWM2_SOCD 0x0C
#define TRIG_SEL_ePWM3_SOCA 0x0D
#define TRIG_SEL_ePWM3_SOCB 0x0E
#define TRIG_SEL_ePWM3_SOCC 0x0F
#define TRIG_SEL_ePWM3_SOCD 0x10
#define TRIG_SEL_ePWM4_SOCA 0x11
#define TRIG_SEL_ePWM4_SOCB 0x12
#define TRIG_SEL_ePWM4_SOCC 0x13
#define TRIG_SEL_ePWM4_SOCD 0x14



void Setup_GPIO(void);
void Setup_ePWM(void);
void Setup_ADC_A(void);
/*
void Setup_ADC_B(void);
void Setup_DAC(void);
*/

#define CPU_FREQ        200E6
#define LSPCLK_FREQ     (CPU_FREQ/2)
#define SCI_FREQ        115200
#define SCI_PRD         (LSPCLK_FREQ/(SCI_FREQ*8))-1

#ifndef _UINT8_T_DECLARED
typedef unsigned char uint8_t;
#define _UINT8_T_DECLARED
#endif

#define PWM_PERIOD ((200E6/2)/18000)

#endif /* PERIPHERAL_SETUP_H_ */
