#include "Header.h"



double Pulse(DynState s); 


void Valid_Pulse(void) {

	InitPath();
	
	RUN_DAQ_mode = RUN_MODE_LINEAR;

	printf("[Motor validation_Pulse...]\n");

	sprintf(OutFileName, "Valid_pulse.out");

	DAQ_Pause(VALID_TRI_TIME_PAUSE);

	MotorDynamic(PULSE_FINAL_TIME, VALID_DIR, OutFileName, Pulse);


}

double Pulse(DynState s) {
	
	double tick = 0.0;

	double desired_rate_angle_2_rate_rad = 0.0;


	tick = 3.0; // [sec]
	
	desired_rate_angle_2_rate_rad=PULSE_DESIRED_RATE_ANGLE* (UNIT_PI) / (180.0); // [rad/sec]


	if		(s.Time <= tick)		return desired_rate_angle_2_rate_rad;
	else if (s.Time <= tick * 2)	return 0.0;
	else if (s.Time <= tick * 3)	return -desired_rate_angle_2_rate_rad;
	else if (s.Time <= tick * 4)	return 0.0;
	else if (s.Time <= tick * 5)	return desired_rate_angle_2_rate_rad;

}