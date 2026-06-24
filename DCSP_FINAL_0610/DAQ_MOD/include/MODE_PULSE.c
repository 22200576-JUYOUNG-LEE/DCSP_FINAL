#include "Header.h"



double Pulse_signal(DynState s); 


void Pulse(void) {

	InitPath();
	
	RUN_DAQ_mode = RUN_MODE_LINEAR;

	printf("[Motor validation_Pulse...]\n");

	sprintf(OutFileName, "Valid_pulse.out");

	DAQ_Pause(VALID_TRI_TIME_PAUSE);

	MotorDynamic(PULSE_FINAL_TIME, VALID_DIR, OutFileName, Pulse_signal);


}

double Pulse_signal(DynState s) {

	if		(s.Time <= PULSE_TIME_PERIOD)		return PULSE_DESIRED_RATE_ANGLE_RAD_S;
	else if (s.Time <= PULSE_TIME_PERIOD * 2.0)	return 0.0;
	else if (s.Time <= PULSE_TIME_PERIOD * 3.0)	return -PULSE_DESIRED_RATE_ANGLE_RAD_S;
	else if (s.Time <= PULSE_TIME_PERIOD * 4.0)	return 0.0;
	else if (s.Time <= PULSE_TIME_PERIOD * 5.0)	return PULSE_DESIRED_RATE_ANGLE_RAD_S;
	else if (s.Time <= PULSE_TIME_PERIOD * 6.0)	return 0.0;
	else if (s.Time <= PULSE_TIME_PERIOD * 7.0)	return -PULSE_DESIRED_RATE_ANGLE_RAD_S;
	else if (s.Time <= PULSE_TIME_PERIOD * 8.0)	return 0.0;
	else return PULSE_DESIRED_RATE_ANGLE_RAD_S;

}