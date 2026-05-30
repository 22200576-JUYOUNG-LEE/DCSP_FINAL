# include "Header.h"

double FORMAT_func(DynState s);

void Potentio() {
	printf("\n================================\n");
	printf("\tPOTENTIO by keyboard");
	printf("\n================================\n");

	RUN_DAQ_mode = USER_INPUT;

	RunDAQ(POTEN_FINAL_TIME, POTEN_DATA_DIR, POTEN_FILE, NULL);
}

void Format() {

	printf("\n================================\n");
	printf("\tFORMAT Angle");
	printf("\n================================\n");

	RUN_DAQ_mode = LINEAR;
	RunDAQ(POTEN_FINAL_TIME, POTEN_DATA_DIR, POTEN_FILE, FORMAT_func);
}


double FORMAT_func(DynState s)
{
	double velocity = 0.0;
	double Vcmd = 0.0;

	
	velocity = s.Vpoten - DAQ_V_STANDARD;
	Vcmd = velocity / 5.0;

	if (Vcmd < LIN_V_DEADZONE) Vcmd = LIN_V_DEADZONE;

	if (s.Vpoten > DAQ_V_STANDARD)  return -Vcmd;
	else							return +Vcmd;
}