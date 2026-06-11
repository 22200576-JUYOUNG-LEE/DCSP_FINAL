# include "Header.h"

#define POTEN_INIT	(double)(-30.0)
#define POTEN_STEP	(double)(3.0)

double Vp = 0.0;

double FORMAT_func(DynState s);


void Potentio() {
	double		Angle = POTEN_INIT;
	double		Angle_curr = 0.0;
	double      Summary_Angle_input_avg[STATIC_N_SWEEP] = { 0.0, };
	double      Summary_Angle_output_avg[STATIC_N_SWEEP] = { 0.0, };

	Dataset     Out_static_Dataset[] = {
			{"Angle_{input}[deg]",  Summary_Angle_input_avg},
			{"Angle_{output}[deg]",  Summary_Angle_output_avg},
	};

	InitPath();

	printf("\n================================\n");
	printf("\tPOTENTIO by keyboard");
	printf("\n================================\n");\

	Format(Angle);

	RUN_DAQ_mode = USER_INPUT;
	for (int iter = 0; iter < POTEN_ITER_MAX; iter++) {
		printf("\n[%d/%d]", iter + 1, POTEN_ITER_MAX);

		Angle_curr = Angle + (double)iter * POTEN_STEP;
		Format(Angle_curr);
		RunDAQ(POTEN_FINAL_TIME, POTEN_DATA_DIR, POTEN_FILE, NULL);

		printf("\nAngle_input avg: %.4f", Angle_curr);
		printf("\nAngle_output avg: %.4f \n", g_daqAvg.Angle);

		Summary_Angle_input_avg[iter] = Angle_curr;
		Summary_Angle_output_avg[iter] = g_daqAvg.Angle;

	}

	SaveDataset(POTEN_DATA_DIR, POTEN_SUMMARY_FILE, Out_static_Dataset, 2, POTEN_ITER_MAX);
}

void Format(double Angle) {

	printf("\n================================\n");
	printf("\tFORMAT Angle");
	printf("\n================================\n");
	
	Vp = POTEN_INIT_V + Angle * POTEN_DEG2V_POTER;
	RUN_DAQ_mode = LINEAR;
	RunDAQ(FORMAT_FINAL_TIME, POTEN_DATA_DIR, POTEN_FILE, FORMAT_func);
}


double FORMAT_func(DynState s)
{
	double velocity = 0.0;
	double Vcmd = 0.0;

	velocity = s.Vpoten - Vp;
	Vcmd = velocity/2.0;

	//if (Vcmd < LIN_V_DEADZONE && Vcmd > -LIN_V_DEADZONE) Vcmd = LIN_V_DEADZONE;

	if (s.Vpoten > Vp)  return -Vcmd;
	else				return -Vcmd;
}