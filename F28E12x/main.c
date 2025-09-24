#include "Peripheral_Setup.h"
#include "Structs.h"
#include "Dlog.h"
#define GLOBAL_Q    21
#include "IQmathLib.h"
#include "3ph_ABC_DQ0.h"
#include "PI.h"
#include "FT.h"

#define CTRLTYPE_PI 0

#ifdef _FLASH
#pragma CODE_SECTION(isr_adc, ".TI.ramfunc");
#pragma CODE_SECTION(isr_cpu_timer0, ".TI.ramfunc");
#pragma CODE_SECTION(Run_Ctrl, ".TI.ramfunc");
#endif

// ======= Interruptions ===================
__interrupt void isr_cpu_timer0(void);
__interrupt void isr_adc(void);
// ====== Local functions ==================
void PWM_Pulse(uint8_t pulse);
void Run_Ctrl(uint8_t new);
void Run_Protection(uint8_t new);
void Init_Controllers(void);
// ==========================================

st_Ctrl Ctrl = {0,0,0,0,0,0,0,0,50};
st_DLOG dlog;
st_Offset Offset = {0,0,0,0,Reset};
st_Measurements Measures = {0};

st_PID C_iLd = PID_DEFAULTS;
st_PID C_iLq = PID_DEFAULTS;
st_DQ0_ABC iL_dq0_abc;
st_DQ0_ABC iL_abc_dq0;

st_DQ0_ABC iL_ab_abc;
st_DQ0_ABC iL_abc_ab;
st_FT Ipra = FT_DEFAULTS;
st_FT Iprb = FT_DEFAULTS;
float iLpeak = 0;

int main(void){

    InitSysCtrl();                              // Initialize System Control:
    DisablePeripheralClocks();
    DINT;                                       // Disable CPU interrupts
    InitPieCtrl();                              // Initialize the PIE control registers to their default state
    IER = 0x0000;                               // Disable CPU interrupts
    IFR = 0x0000;                               // Clear all CPU interrupt flags:
    InitPieVectTable();                         // Initialize the PIE vector table

    EALLOW;    
    PieVectTable.TIMER0_INT = &isr_cpu_timer0;  // Redirect function to interruption
    PieVectTable.ADCA1_INT = &isr_adc;          // Redirect function to interruption
    EDIS;

    // 
    PieCtrlRegs.PIEIER2.bit.INTx2 = 1;          // Enable PieVector to Timer 0 interrupt
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;          // Enable PieVector to ADCA1 interrupt
    IER |= (M_INT1|M_INT2);                   // Enable lines of interrupt

    Setup_GPIO();                               // Configure GPIOs
    Setup_ePWM();                               // Configure ePWM
    Setup_ADC_A();
    InitCpuTimers();                            // Initializae cpu timer
    ConfigCpuTimer(&CpuTimer0, 200, 30000);   // Configure cpu timer0 with 200MHz and 1s
    CpuTimer0Regs.TCR.all = 0x4000;             // Enable timer 0
    //========================================
    CpuSysRegs.PERCLKCR.bit.TBCLKSYNC = 1;
    GpioDataRegs.GPBTOGGLE.bit.GPIO45 = 1;

    Init_Controllers();
    EINT;                                       // Enable Global interrupt INTM
    ERTM;                                       // Enable Global realtime interrupt DBGM

    while(1){        
        Run_Protection(Ctrl.New_Sample);
        PWM_Pulse(Ctrl.PWM);
        Run_Ctrl(Ctrl.New_Sample);
    }// Infinite loop
    return 0;
}
//=========== Local functions ==================
#if CTRLTYPE_PI == 1
void Run_Ctrl(uint8_t new){
    static float theta = 0;
	_iq iqtheta, iqcos, iqsin;
    float dutya, dutyb, dutyc;

	if(new == 0) return;

    theta += 6.28318530718f/600.0f;
    if(theta > 6.28318530718f) theta -= 6.28318530718f;

    iqtheta = _IQ(theta);
    iqsin = _IQsin(iqtheta);
    iqcos = _IQcos(iqtheta);

    iL_abc_dq0.a = Measures.iLa;
    iL_abc_dq0.b = Measures.iLb;
    iL_abc_dq0.c = Measures.iLc;
    iL_abc_dq0.sin = _IQtoF(iqsin);
    iL_abc_dq0.cos = _IQtoF(iqcos);

    ABC_to_DQ0(&iL_abc_dq0);

    if(Ctrl.PWM == 1){
        C_iLd.Err = (iLpeak - iL_abc_dq0.d);
        C_iLd.Feedforward = 0;
        PI_Calc(&C_iLd);
        C_iLq.Err = (0 - iL_abc_dq0.q);
        C_iLq.Feedforward = 0;
        PI_Calc(&C_iLq);

        // iLdq to abc PI
        iL_dq0_abc.d = C_iLd.Out*0.0333333f;
        iL_dq0_abc.q = C_iLq.Out*0.0333333f;
        iL_dq0_abc.sin = iL_abc_dq0.sin;
        iL_dq0_abc.cos = iL_abc_dq0.cos;
        DQ0_to_ABC(&iL_dq0_abc);

       	dutya = (iL_dq0_abc.a + 1.0f)*0.5f;
    	dutyb = (iL_dq0_abc.b + 1.0f)*0.5f;
        dutyc = (iL_dq0_abc.c + 1.0f)*0.5f;
    }else{
        PI_Reset(&C_iLq);
        PI_Reset(&C_iLd);
    }
    Ctrl.New_Sample = 0;

    Pwm1Regs.PWM1_CMPAS.bit.PWM1_CMPAS = (Uint16)(dutya*PWM_PERIOD);
    Pwm1Regs.PWM2_CMPAS.bit.PWM2_CMPAS = (Uint16)(dutyb*PWM_PERIOD);
    Pwm1Regs.PWM3_CMPAS.bit.PWM3_CMPAS = (Uint16)(dutyc*PWM_PERIOD);

    DLOG_Func(&dlog);
   return;
}
#else
void Run_Ctrl(uint8_t new){
	static float theta = 0;
    float dutya = 0, dutyb = 0, dutyc = 0;
	float ialpha, ibeta;
	_iq iqtheta, iqcos, iqsin;
	
	if(new == 0) return;

    theta += 6.28318530718f/600.0f;
    if(theta > 6.28318530718f) theta -= 6.28318530718f;

	iqtheta = _IQ(theta);
    iqsin = _IQsin(iqtheta);
    iqcos = _IQcos(iqtheta);

	ialpha = iLpeak * _IQtoF(iqsin);
    ibeta = iLpeak * _IQtoF(iqcos);

	iL_abc_ab.a = Measures.iLa;
	iL_abc_ab.b = Measures.iLb;
	iL_abc_ab.c = Measures.iLc;
	ABC_to_DQ0(&iL_abc_ab);

	if(Ctrl.PWM == 1){
		Ipra.x0 = ialpha - iL_abc_ab.alpha;
		FT_Calc(&Ipra);

		Iprb.x0 = ibeta - iL_abc_ab.beta;
		FT_Calc(&Iprb);

		iL_ab_abc.alpha = (Ipra.y0 + Ipra.x0)/15.0f;
		iL_ab_abc.beta  = (Iprb.y0 + Iprb.x0)/15.0f;
		AB_to_ABC(&iL_ab_abc);

 		dutya = (iL_ab_abc.a + 1.0f)*0.5f;
        dutyb = (iL_ab_abc.b + 1.0f)*0.5f;
        dutyc = (iL_ab_abc.c + 1.0f)*0.5f;
	}else{
		FT_Reset(&Ipra);
		FT_Reset(&Iprb);
	}
	Ctrl.New_Sample = 0;
	Pwm1Regs.PWM1_CMPAS.bit.PWM1_CMPAS = (Uint16)(dutya*PWM_PERIOD);
    Pwm1Regs.PWM2_CMPAS.bit.PWM2_CMPAS = (Uint16)(dutyb*PWM_PERIOD);
    Pwm1Regs.PWM3_CMPAS.bit.PWM3_CMPAS = (Uint16)(dutyc*PWM_PERIOD);
    DLOG_Func(&dlog);
}
#endif

void Run_Protection(uint8_t new){
    static uint16_t num_pulse = 0;
    if(new == 0) return;

    if(Ctrl.PWM == 1 && Ctrl.Timing == 1){
        num_pulse =  (Ctrl.Qnt_pulse == num_pulse) ? 0 : (num_pulse + 1);
        Ctrl.PWM = (num_pulse == 0) ? 0 : 1;
    }else{
        num_pulse = 0;
    }

    if(Ctrl.Protection == 0) return;

    Ctrl.Over_iLa =  (Measures.iLa > IL_MAX || Measures.iLa < -IL_MAX) ? 1 : Ctrl.Over_iLa;
 	Ctrl.Over_iLb =  (Measures.iLb > IL_MAX || Measures.iLb < -IL_MAX) ? 1 : Ctrl.Over_iLb;
 	Ctrl.Over_iLc =  (Measures.iLc > IL_MAX || Measures.iLc < -IL_MAX) ? 1 : Ctrl.Over_iLc;

    if(Ctrl.Over_iLa || Ctrl.Over_iLb || Ctrl.Over_iLc) Ctrl.PWM = 0;
}

void PWM_Pulse(uint8_t pulse){
    static uint8_t _pulse = 1;
    if(_pulse == pulse) return;

    GpioDataRegs.GPADAT.all &= 0x000000A0;
    EALLOW;
    if(pulse == 0){
        GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
        GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;
        GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
        GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;   
        GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;
        GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;
    }else{
        GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;
        GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
        GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
        GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   
        GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;
        GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;
    }
    EDIS;
	_pulse = pulse;
   return;
}

void Init_Controllers(void){
	DLOG_Init(&dlog);
    dlog.input_ch1 = &Measures.iLa;
    dlog.input_ch2 = &Measures.iLb;
	dlog.input_ch3 = &Measures.iLc;
	dlog.input_trigger = &Measures.iLa;

    dlog.pre_scalar = 4;
    dlog.skip_count = 0;
    dlog.status = NO_TRIGGER;

    Offset.Calc = Reset;
	ABC_DQ0_Init(&iL_abc_dq0);
	ABC_DQ0_Init(&iL_dq0_abc);
	ABC_DQ0_Init(&iL_ab_abc);
	ABC_DQ0_Init(&iL_abc_ab);

	C_iLd.Kp = C_iLq.Kp = 1.0f;
	C_iLd.Ki = C_iLq.Ki = 175.0f/36000.0f;
	C_iLd.Kc = C_iLq.Kc = 1.0f;
	C_iLd.OutMax = C_iLq.OutMax = 30.0f;
	C_iLd.OutMin = C_iLq.OutMin = -30.0f;

	Ipra.b0 = Iprb.b0 =  8.718798630747742E-4f;
	Ipra.b2 = Iprb.b2 =  -Ipra.b0;
	Ipra.a1 = Iprb.a1 =  -1.998146671456443f;
	Ipra.a2 = Iprb.a2 =  0.998256240273851f;


	/*Ipra.b0 = Iprb.b0 = 0.008711203519059;
	Ipra.b2 = Iprb.b2 =-Ipra.b0;
	Ipra.a1 = Iprb.a1 = -1.996406050380459;
	Ipra.a2 = Iprb.a2 = 0.996515518592376;*/

} 

// ======= Interruptions ===================
__interrupt void isr_cpu_timer0(void){    
    GpioDataRegs.GPBTOGGLE.bit.GPIO33 = 1;
    GpioDataRegs.GPBTOGGLE.bit.GPIO45 = 1;
    
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}

__interrupt void isr_adc(void){    // Run at 2x fpwm, fpwm = 10kHz



 //GpioDataRegs.GPBSET.bit.GPIO34 = 1;
    static float avg[] = {0, 0, 0};    // auxiliary variable to reset offset

    if(Ctrl.New_Sample == 1) Ctrl.Over_Run = 1;
    Ctrl.New_Sample = 1;

    if(Offset.Calc == Reset){
        Offset.Calc = Running;
        Offset.Cnt = 0;
        Offset.Ch0 = Offset.Ch1 = Offset.Ch2 = 0;       
        avg[0] = avg[1] = avg[2] = 0.0;
    }

  	Measures.iLa = GAIN_I*(float)((int16_t)AdcaResultRegs.ADCRESULT0.bit.RESULT - 2047) - Offset.Ch0;
    Measures.iLb = GAIN_I*(float)((int16_t)AdcaResultRegs.ADCRESULT1.bit.RESULT - 2047) - Offset.Ch1;
    Measures.iLc = GAIN_I*(float)((int16_t)AdcaResultRegs.ADCRESULT2.bit.RESULT - 2047) - Offset.Ch2;

  
    if(Offset.Calc == Running){
        Offset.Cnt++;
        avg[0] += Measures.iLa;
        avg[1] += Measures.iLb;
        avg[2] += Measures.iLc;

        if(Offset.Cnt == N_SAMPLE_OFFSET){
            Offset.Calc = OK;
            Offset.Ch0 = avg[0]/QNT_SAMPLE_OFFSET;
            Offset.Ch1 = avg[1]/QNT_SAMPLE_OFFSET;
            Offset.Ch2 = avg[2]/QNT_SAMPLE_OFFSET;
         }
    }
   
    Ctrl.New_Sample = 1;
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    // Check if overflow has occurred
    if(AdcaRegs.ADCINTOVF.bit.ADCINT1OVF == 1) {
        AdcaRegs.ADCINTOVFCLR.bit.ADCINT1OVF = 1; //clear INT1 overflow flag
        AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

