#ifndef PINS_H
#define PINS_H

//I = Input
//O = Output
//L = Active Low
//H = Active High

//Ex: IL = Input, Active Low
//    OH = Output, Active High
//So: BMS_FLT_IH = BMS Fault is an Input, and Active High

//
#define SOC_IN A0         //State of Charge for TS batteries, not used
#define WHEEL3_IN A1      //Wheel RPM, not used
#define WHEEL2_IN A3
#define WHEEL1_IN A4
#define WHEEL0_IN A2

#define CONT_REQ_OUT 5    //Request for BMS to precharge, High closes AIRs, allowing charge

#define START_INPUT_IH 18 //Driver start input, Active High
#define BMS_FLT_IH 31     //BMS Fault, Active High
#define SEVCON_FLT_IL 29  //Sevcon Fault, Active Low (NOT before write)


#define BOTS_IN 27        //Directly after Break over travel switch 
#define IMD_FLT_IN 25 
#define RTDS_OUT 23       //Ready to drive sound Active High -- These three here and above need to be done

#define COCKPIT_SW_IL 28  //Downtstream of cockpit switch for shutdown - Active Low (Switched off if used)

#define TSMS_IN 30        //Tractive System Master Switch - Active Low (TSMS Open on Low) - UNUSED

#define FORWARD_OUT_L 50  //SEVCON Active Low
#define REVERSE_OUT_L 48  //SEVCON Active Low



#endif
