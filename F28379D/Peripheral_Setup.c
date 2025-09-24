/*
 * Peripheral_Setup.c
 */
#include "Peripheral_Setup.h"

void Setup_GPIO(void){
    // pg 959 Table Mux, pg 965 Registers and  spruhm8i.pdf - Technical reference
    GpioDataRegs.GPASET.bit.GPIO24 = 1;

    EALLOW;
    // LED 31 A, 2
    // LED 34 B, 1
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
    GpioCtrlRegs.GPBCSEL1.bit.GPIO34 = GPIO_MUX_CPU1;
    GpioCtrlRegs.GPACSEL4.bit.GPIO31 = GPIO_MUX_CPU1;

    //PWM
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;


    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO9 = 1;
    EDIS;
}

void Setup_ePWM(void){
    // pg 1978 spruhm8i.pdf - Technical reference

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM3 = 1;

    EPwm1Regs.TZFRC.bit.OST = 1;
    EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
    EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO;
    EDIS;


    EPwm1Regs.TBPRD = PWM_PERIOD;               // Set timer period 4.8kHz
    EPwm1Regs.CMPA.bit.CMPA = EPwm1Regs.TBPRD>>1;   // duty cycle
    EPwm1Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm1Regs.TBPHS.bit.TBPHS = 0;                  // Phase is 0

    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading
    EPwm1Regs.TBCTL.bit.PHSDIR = TB_DOWN;         // Disable phase loading
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up/down
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm1Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;            // set actions for EPWM1A
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;

    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // Active Hi complementary
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  // enable Dead-band module
    EPwm1Regs.DBFED.bit.DBFED = 342;                 // FED = 20 TBCLKs
    EPwm1Regs.DBRED.bit.DBRED = 342;                 // RED = 20 TBCLKs
    //===============================
    Conf_Reg_ePWM(&EPwm1Regs, &EPwm2Regs);
    Conf_Reg_ePWM(&EPwm1Regs, &EPwm3Regs);
    //===================================
    //Trigger ADC

    EPwm1Regs.ETSEL.bit.SOCAEN = 1;                 // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_PRDZERO;   // Dispara ADC no topo
    EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;            // Trigger on every event

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;       // Enable ePWMs
    EDIS;
}

void Conf_Reg_ePWM(volatile struct EPWM_REGS *reg1, volatile struct EPWM_REGS *reg2){
    EALLOW;
    reg2->TZFRC.bit.OST = 1;
    reg2->TZCTL.bit.TZA = TZ_FORCE_LO;
    reg2->TZCTL.bit.TZB = TZ_FORCE_LO;
    EDIS;

    reg2->TBPRD = reg1->TBPRD;
    reg2->CMPA.bit.CMPA = reg1->CMPA.bit.CMPA;
    reg2->TBCTR = reg1->TBCTR;
    reg2->TBPHS.bit.TBPHS = reg1->TBPHS.bit.TBPHS;
    reg2->TBCTL.bit.PHSEN = reg1->TBCTL.bit.PHSEN;
    reg2->TBCTL.bit.PHSDIR = reg1->TBCTL.bit.PHSDIR;
    reg2->TBCTL.bit.SYNCOSEL = reg1->TBCTL.bit.SYNCOSEL;
    reg2->TBCTL.bit.CTRMODE = reg1->TBCTL.bit.CTRMODE;
    reg2->TBCTL.bit.HSPCLKDIV = reg1->TBCTL.bit.HSPCLKDIV;
    reg2->TBCTL.bit.CLKDIV = reg1->TBCTL.bit.CLKDIV;
    reg2->CMPCTL.bit.SHDWAMODE = reg1->CMPCTL.bit.SHDWAMODE;
    reg2->CMPCTL.bit.LOADAMODE = reg1->CMPCTL.bit.LOADAMODE;
    reg2->CMPCTL.bit.SHDWBMODE = reg1->CMPCTL.bit.SHDWBMODE;
    reg2->CMPCTL.bit.LOADBMODE = reg1->CMPCTL.bit.LOADBMODE;

    reg2->AQCTLA.bit.PRD = reg1->AQCTLA.bit.PRD;
    reg2->AQCTLA.bit.ZRO = reg1->AQCTLA.bit.ZRO;
    reg2->AQCTLA.bit.CAU = reg1->AQCTLA.bit.CAU;
    reg2->AQCTLA.bit.CAD = reg1->AQCTLA.bit.CAD;
    reg2->AQCTLB.bit.PRD = reg1->AQCTLB.bit.PRD;
    reg2->AQCTLB.bit.ZRO = reg1->AQCTLB.bit.ZRO;
    // reg2->AQCTLB.bit.CBU = reg1.AQCTLB.bit.CBU;
    // reg2->AQCTLB.bit.CBD = reg1.AQCTLB.bit.CBD;
    // reg2->AQCTLB.bit.CAU = reg1.AQCTLB.bit.CAU;
    // reg2->AQCTLB.bit.CAD = reg1.AQCTLB.bit.CAD;

    reg2->DBCTL.bit.POLSEL = reg1->DBCTL.bit.POLSEL;
    reg2->DBCTL.bit.OUT_MODE = reg1->DBCTL.bit.OUT_MODE;
    reg2->DBFED.bit.DBFED = reg1->DBFED.bit.DBFED;
    reg2->DBRED.bit.DBRED = reg1->DBRED.bit.DBRED;
}

void Setup_ADC_A(void){
    // pg 1592 spruhm8i.pdf - Technical reference
    Uint16 acqps;

    EALLOW;
    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;          // set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;       // Set pulse um ciclo antes do resultado
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;          // power up the ADC
    DELAY_US(1000);                             // delay for 1ms to allow ADC time to power up

    // determine minimum acquisition window (in SYSCLKS) based on resolution
    if(AdcaRegs.ADCCTL2.bit.RESOLUTION == ADC_RESOLUTION_12BIT) acqps = 30; // 75ns
    else acqps = 63;    // resolution is 16-bit AdcaRegs.ADCCTL2.bit.RESOLUTION
    acqps = 30;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;         //SOC0 will convert pin ADCIN2 A03 iLa
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;     //sample window
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;  //trigger on ePWM1 SOCA

    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 2;         //SOC1 will convert pin ADCINA3 A04 iLb
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = AdcaRegs.ADCSOC0CTL.bit.TRIGSEL;

    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 3;        //SOC2 will convert pin ADCINA5 A06 iLc
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = AdcaRegs.ADCSOC0CTL.bit.TRIGSEL;

    AdcaRegs.ADCSOC3CTL.bit.CHSEL = 1;        //SOC2 will convert pin ADCINA5 A06 iLc
    AdcaRegs.ADCSOC3CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = AdcaRegs.ADCSOC0CTL.bit.TRIGSEL;

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 3;      // end of SOC3 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;        // enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;      // make sure INT1 flag is cleared
    EDIS;
}

void Setup_ADC_B(void){
    // pg 1592 spruhm8i.pdf - Technical reference
    Uint16 acqps;
    EALLOW;
    CpuSysRegs.PCLKCR13.bit.ADC_B = 1;
    AdcbRegs.ADCCTL2.bit.PRESCALE = 6;          // set ADCCLK divider to /4
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;       // Set pulse um ciclo antes do resultado
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;          // power up the ADC
    DELAY_US(1000);                             // delay for 1ms to allow ADC time to power up
    // determine minimum acquisition window (in SYSCLKS) based on resolution
    if(AdcbRegs.ADCCTL2.bit.RESOLUTION == ADC_RESOLUTION_12BIT) acqps = 30;     // 75ns
    else acqps = 63;  // 320ns, resolution is 16-bit
    acqps = 30;
    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 0;         //SOC0 will convert pin ADCINB0 AO9 Va
    AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps;     //sample window
    AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;  //trigger on ePWM1 SOCA

    AdcbRegs.ADCSOC1CTL.bit.CHSEL = 1;         //SOC1 will convert pin ADCINB2 AO11 Vb
    AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps;
    AdcbRegs.ADCSOC1CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;

    AdcbRegs.ADCSOC2CTL.bit.CHSEL = 2;         //SOC2 will convert pin ADCINB5 AO12 Vc
    AdcbRegs.ADCSOC2CTL.bit.ACQPS = acqps;
    AdcbRegs.ADCSOC2CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;

    AdcbRegs.ADCSOC3CTL.bit.CHSEL = 3;         //SOC2 will convert pin ADCINB5 AO12 Vc
    AdcbRegs.ADCSOC3CTL.bit.ACQPS = acqps;
    AdcbRegs.ADCSOC3CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;

    AdcbRegs.ADCSOC4CTL.bit.CHSEL = 4;         //SOC2 will convert pin ADCINB5 AO12 Vc
    AdcbRegs.ADCSOC4CTL.bit.ACQPS = acqps;
    AdcbRegs.ADCSOC4CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;

    AdcbRegs.ADCSOC5CTL.bit.CHSEL = 5;         //SOC2 will convert pin ADCINB5 AO12 Vc
    AdcbRegs.ADCSOC5CTL.bit.ACQPS = acqps;
    AdcbRegs.ADCSOC5CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;

    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL = 5;      // end of SOC5 will set INT1 flag
    AdcbRegs.ADCINTSEL1N2.bit.INT1E = 1;        // enable INT1 flag
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;      // make sure INT1 flag is cleared
    EDIS;
}

void Setup_DAC(void){
    // pg 1760 spruhm8i.pdf - Technical reference
    EALLOW;
    CpuSysRegs.PCLKCR16.bit.DAC_A = 1;
    DacaRegs.DACCTL.bit.SYNCSEL =       0x00; // EPWM1SYNCPER
    DacaRegs.DACCTL.bit.LOADMODE =      0x00; // 0 Load on next SYSCLK, 1 Load on next EPWMSYNCPER,
    DacaRegs.DACCTL.bit.DACREFSEL =     0x01; // 0 VDAC/VSSA, 1 ADC VREFHI/VSSA
    DacaRegs.DACOUTEN.bit.DACOUTEN =    1;
    DacaRegs.DACLOCK.all =              0x00;

    CpuSysRegs.PCLKCR16.bit.DAC_B = 1;
    DacbRegs.DACCTL.bit.SYNCSEL =       0x00;
    DacbRegs.DACCTL.bit.LOADMODE =      0x00;
    DacbRegs.DACCTL.bit.DACREFSEL =     0x01;
    DacbRegs.DACOUTEN.bit.DACOUTEN =    1;
    DacbRegs.DACLOCK.all =              0x00;

    CpuSysRegs.PCLKCR16.bit.DAC_C = 1;
    DaccRegs.DACCTL.bit.SYNCSEL =       0x00;
    DaccRegs.DACCTL.bit.LOADMODE =      0x00;
    DaccRegs.DACCTL.bit.DACREFSEL =     0x01;
    DaccRegs.DACOUTEN.bit.DACOUTEN =    1;
    DaccRegs.DACLOCK.all =              0x00;
    EDIS;

    DacaRegs.DACVALS.bit.DACVALS =      0; //12 bits
    DacbRegs.DACVALS.bit.DACVALS =      0; //12 bits
    DaccRegs.DACVALS.bit.DACVALS =      0; //12 bits
    /////
}

void Setup_ADC_D(void){
    // pg 1592 spruhm8i.pdf - Technical reference
    Uint16 acqps;
    EALLOW;
    //write configurations
    CpuSysRegs.PCLKCR13.bit.ADC_D = 1;
    AdcdRegs.ADCCTL2.bit.PRESCALE = 6;                      // Set ADCCLK divider to /4
    AdcSetMode(ADC_ADCD, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcdRegs.ADCCTL1.bit.INTPULSEPOS = 1;                   // Set pulse positions to late
    AdcdRegs.ADCCTL1.bit.ADCPWDNZ = 1;                      // power up the ADC
    DELAY_US(1000);                                         // delay for > 1ms to allow ADC time to power up

    //determine minimum acquisition window (in SYSCLKS) based on resolution
    if(AdcdRegs.ADCCTL2.bit.RESOLUTION == ADC_RESOLUTION_12BIT) acqps = 14;     //75ns
    else acqps = 63; //resolution is 16-bit, 320ns

    //Select the channels to convert and end of conversion flag ADCB
    AdcdRegs.ADCSOC0CTL.bit.CHSEL = 0;                     //SOC0 will convert pin ADCIND0 #1 J21
    AdcdRegs.ADCSOC0CTL.bit.ACQPS = acqps;                  //sample window is 100 SYSCLK cycles
    AdcdRegs.ADCSOC0CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;  //trigger

    AdcdRegs.ADCSOC1CTL.bit.CHSEL = 1;                      //SOC1 will convert pin ADCIND1
    AdcdRegs.ADCSOC1CTL.bit.ACQPS = acqps;
    AdcdRegs.ADCSOC1CTL.bit.TRIGSEL = AdcdRegs.ADCSOC0CTL.bit.TRIGSEL;

    AdcdRegs.ADCSOC2CTL.bit.CHSEL = 2;                      //SOC2 will convert pin ADCIND2
    AdcdRegs.ADCSOC2CTL.bit.ACQPS = acqps;
    AdcdRegs.ADCSOC2CTL.bit.TRIGSEL = AdcdRegs.ADCSOC0CTL.bit.TRIGSEL;

    AdcdRegs.ADCSOC3CTL.bit.CHSEL = 3;                      //SOC3 will convert pin ADCIND3
    AdcdRegs.ADCSOC3CTL.bit.ACQPS = acqps;
    AdcdRegs.ADCSOC3CTL.bit.TRIGSEL = AdcdRegs.ADCSOC0CTL.bit.TRIGSEL;

    //pg 1569 spruhm8i.pdf - Technical reference
    AdcdRegs.ADCINTSEL1N2.bit.INT1SEL = 3;                  // End of SOC3 will set INT1 flag
    AdcdRegs.ADCINTSEL1N2.bit.INT1E = 1;                    // Disable INT1 flag
    AdcdRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;                  // Make sure INT1 flag is cleared
    EDIS;
}

