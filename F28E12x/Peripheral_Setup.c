/*
 * Peripheral_Setup.c
 */
#include "Peripheral_Setup.h"

void Setup_GPIO(void){   
    EALLOW;
	GpioDataRegs.GPADAT.all = 0;
    /*GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 0;   
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;   */


    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;

    GpioCtrlRegs.GPBDIR.bit.GPIO45 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 1;

    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;

    GpioCtrlRegs.GPHDIR.bit.GPIO227 = 1;
    GpioCtrlRegs.GPHDIR.bit.GPIO224 = 1;
    GpioCtrlRegs.GPHDIR.bit.GPIO230 = 1;

    GpioCtrlRegs.GPHPUD.bit.GPIO227 = 1;
    GpioCtrlRegs.GPHPUD.bit.GPIO224 = 1;
    GpioCtrlRegs.GPHPUD.bit.GPIO230 = 1;
    EDIS;
}

void Setup_ePWM(void){
    // pg 1978 spruhm8i.pdf - Technical reference
    CpuSysRegs.PERCLKCR.bit.TBCLKSYNC = 0;
    // Set Period for 20kHz switching frequency (TBCLK = 160MHz)
    // Period = 160MHz/(2*20kHz) = 4000 counts for Up-Down count mode
    Pwm1Regs.TBPRDS.bit.TBPRDS = PWM_PERIOD;                    // Set timer period
    Pwm1Regs.TBPRD.bit.TBPRD = PWM_PERIOD;                        // Set timer period
    // Time Base Control
    Pwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    Pwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    Pwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW_LOAD_DISABLE;      // Shadow to Active load of TBPRD register
    Pwm1Regs.TBCTL.bit.SYNCOSEL = SYNC_OUT_DISABLED;  
    //Pwm1Regs.TBCTL.bit.FREE_SOFT = EMULATION_FREE_RUN; 
    Pwm1Regs.CMPCTL.bit.PWM1_LOADAMODE = CC_CTR_ZERO_PRD;   // Shadow to Active load of CMPA register
    Pwm1Regs.CMPCTL.bit.PWM1_LOADBMODE = CC_CTR_ZERO_PRD;
    Pwm1Regs.CMPCTL.bit.PWM2_LOADAMODE = CC_CTR_ZERO_PRD;
    Pwm1Regs.CMPCTL.bit.PWM2_LOADBMODE = CC_CTR_ZERO_PRD;
    Pwm1Regs.CMPCTL.bit.PWM3_LOADAMODE = CC_CTR_ZERO_PRD;
    Pwm1Regs.CMPCTL.bit.PWM3_LOADBMODE = CC_CTR_ZERO_PRD;    
    Pwm1Regs.CMPCTL.bit.LOADCMODE = CC_CTR_ZERO_PRD;        // Shadow to Active load of CMPC register
    Pwm1Regs.CMPCTL.bit.LOADDMODE = CC_CTR_ZERO_PRD;
    
    // Set Compare values for all channels (50% duty cycle), shadow

    Pwm1Regs.PWM1_AQCTLAS.bit.CAU = AQ_CLEAR;         // Set high on CMPA up
    Pwm1Regs.PWM1_AQCTLAS.bit.CAD = AQ_SET;       // Set low on CMPA down
    Pwm1Regs.PWM2_AQCTLAS.bit.CAU = AQ_CLEAR;         // Set high on CMPA up
    Pwm1Regs.PWM2_AQCTLAS.bit.CAD = AQ_SET;       // Set low on CMPA down
    Pwm1Regs.PWM3_AQCTLAS.bit.CAU = AQ_CLEAR;         // Set high on CMPA up
    Pwm1Regs.PWM3_AQCTLAS.bit.CAD = AQ_SET;       // Set low on CMPA down

    // Dead-Band Configuration
    Pwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;   // Enable Dead-band module
    Pwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;         // Active high
    Pwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;           // Use PWMxA as source
    // Set Dead-Band Delays
    Pwm1Regs.DBREDS.bit.DBREDS = 684;              // Rising edge delay
    Pwm1Regs.DBFEDS.bit.DBFEDS = 684;              // Falling edge delay

    // Trigger on every event  
    Pwm1Regs.SOCEN.bit.SOCA_ENABLE = 1;         // Disable SOCA
    Pwm1Regs.SOCSEL.bit.SOCA_SEL = SOC_TBCTR_ZERO_PERIOD;  
    Pwm1Regs.SOCPERIOD.bit.SOCA_PERIOD = 1;     // Generate pulse on 1st event
  
}


void Setup_ADC_A(void){
    // Setup VREF as internal
    SetVREF(ADC_ADCA, ADC_INTERNAL, ADC_VREF3P3);
    EALLOW;
    // Set ADCCLK divider to /4
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;
    // Set pulse positions to late
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    // Power up the ADC and then delay for 1 ms
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    
    DELAY_US(1000);
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 8;  
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 9;     // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;   // Trigger on MCPWM1 SOCA

    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 5;  
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = AdcaRegs.ADCSOC0CTL.bit.ACQPS;     // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = AdcaRegs.ADCSOC0CTL.bit.TRIGSEL;   // Trigger on MCPWM1 SOCA
    
    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 12;  
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = AdcaRegs.ADCSOC0CTL.bit.ACQPS;     // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = AdcaRegs.ADCSOC0CTL.bit.TRIGSEL;   // Trigger on MCPWM1 SOCA
    
    AdcaRegs.ADCINTSEL.bit.INT1SEL = 2; // End of SOC2 will set INT1 flag
    AdcaRegs.ADCINTSEL.bit.INT1E = 1;   // Enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // Make sure INT1 flag is cleared
    EDIS;
}
