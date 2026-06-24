#include "Header.h"



double Pulse_signal(DynState s); 


void Pulse(void) {

	InitPath();
	
	RUN_DAQ_mode = RUN_MODE_LINEAR;

	printf("[Motor validation_Pulse...]\n");

	sprintf(OutFileName, "Valid_pulse.out");

	DAQ_Pause(VALID_TRI_TIME_PAUSE);

	MotorDynamic(PULSE_FINAL_TIME, VALID_DIR, OutFileName, Pulse);


}

double Pulse_signal(DynState s) {


	tick = 3.0; // [sec]

	if		(s.Time <= tick)		return PULSE_DESIRED_RATE_ANGLE_RAD_S;
	else if (s.Time <= tick * 2)	return 0.0;
	else if (s.Time <= tick * 3)	return -PULSE_DESIRED_RATE_ANGLE_RAD_S;
	else if (s.Time <= tick * 4)	return 0.0;
	else if (s.Time <= tick * 5)	return PULSE_DESIRED_RATE_ANGLE_RAD_S;
	else if (s.Time <= tick * 6)	return 0.0;
	else if (s.Time <= tick * 7)	return -PULSE_DESIRED_RATE_ANGLE_RAD_S;
	else if (s.Time <= tick * 8)	return 0.0;
	else if (s.Time <= tick * 9)	return PULSE_DESIRED_RATE_ANGLE_RAD_S;

}