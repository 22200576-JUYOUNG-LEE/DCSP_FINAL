#include "myHeader.h"

#define   VINITIAL          (double) 0.0
#define   VFINISH           (double) 5.0

#define   V_CLOSE_INITIAL   (double) 2.20
#define   V_CLOSE_FINISH    (double) 2.80

#define   V_WIDE_STEP       (double) 0.02
#define   V_CLOSE_STEP      (double) 0.02

#define   NUM_SWEEP_RUNS   (int) 101

double Find_Vgyro_offset(TaskHandle taskAI, TaskHandle taskAO)
{
    double      time_init;
    double      time_curr;

    int         count_avg = 0;

    float64     Vin[NUM_AI_CHANNELS] = { 0.0 };

    double      Vgyro_curr = 0.0;
    double      Vgyro_offset = 0.0;

    printf("\n[Calibration] Measuring Vgyro_offset (motor off, gimbal stationary)...\n");

    ResetDAQ(taskAO);

    time_init = GetWindowTime();
    time_curr = time_init;

    for (int count = 0; count < N_STEP; count++)
    {
        ReadDAQ(taskAI, Vin);

        if (count > N_STEP / 2)
        {
            count_avg = count - N_STEP / 2;

            Vgyro_curr = Vin[2];

            Vgyro_offset = Cal_avg(count_avg, Vgyro_offset, Vgyro_curr);
        }

        while (1)
        {
            time_curr = GetWindowTime();
            if (time_curr - time_init - count * SAMPLING_TIME * 1000.0
                >= (SAMPLING_TIME * 1000.0)) break;
        }
    }

    printf("[Calibration] Vgyro_offset = %.3f V\n", Vgyro_offset);
    return Vgyro_offset;
}

double Find_Wgyro_avg(TaskHandle taskAI, TaskHandle taskAO, const   char* OutDirName, double Vc, double Vgyro_offset) {
    float64     Vin[NUM_AI_CHANNELS] = { 0.0 };

    double      Wgyro_avg = 0.0;    //[rad/sec]
    double      time_curr = 0.0;
    double      time_init = 0.0;
    double      time = 0.0;         //[sec]

    double      Vgyro_curr = 0.0;   //[V]
    double      Vpoten = 0.0;       //[V]
    double      Wgyro = 0.0;        //[rad/sec]               

    int         count_avg = 0;

    static double      Out_Time[N_STEP] = { 0.0, };
    static double      Out_Wgyro_Data[N_STEP] = { 0.0, };

    char  OutFileName[100] = { "" };

    Dataset     Out_DAQ_Dataset[] = {
        {"Time[sec]", Out_Time},
        {"\\omega_{gyro}[deg/sec]", Out_Wgyro_Data}
    };
    int         numDataset = sizeof(Out_DAQ_Dataset) / sizeof(Out_DAQ_Dataset[0]);

    time_init = GetWindowTime();
    time_curr = time_init;

    printf("\nVc = %.4f V\n", Vc);
    for (int count = 0; count < N_STEP; count++)
    {
        time = (time_curr - time_init) * 0.001; // [sec]

        WriteDAQ(taskAO, Vc);
        ReadDAQ(taskAI, Vin);

        Vgyro_curr = Vin[2];
        Wgyro = Kg * (Vgyro_curr - Vgyro_offset);
        Vpoten = Vin[3];

        if (count > N_STEP / 2) {
            count_avg = count - N_STEP / 2;
            Wgyro_avg = Cal_avg(count_avg, Wgyro_avg, Wgyro);
        }

        // 5. Save to buffers
        Out_Time[count] = time;
        Out_Wgyro_Data[count] = Wgyro * RAD2DEG;

        // @. Emergency Stop
        if (_kbhit())
            if (_getch() == 's') {
                CloseDAQ(taskAI, taskAO);
                exit(0);
            }

        while (1)
        {
            time_curr = GetWindowTime();
            if (time_curr - time_init - count * SAMPLING_TIME * 1000.0
                >= (SAMPLING_TIME * 1000.0)) break;
        }
    }
    printf("Wgyro_avg = %.6f [deg/s]\n", Wgyro_avg);

    snprintf(OutFileName, sizeof(OutFileName), "Vc_%.2f_data.out", Vc);
    SaveDataset(OutDirName, OutFileName, Out_DAQ_Dataset, numDataset, N_STEP);

    return      Wgyro_avg;
}

void ObtainMotorStaticProperty(TaskHandle taskAI, TaskHandle taskAO, double Vgyro_offset) {

    double      Vc_pos = Vcmd_offset;
    double      Vc_neg = Vcmd_offset;

    double      Vc_current = 0.0;
    double      Wgyro_avg = 0.0;

    double      Vstep = 0.0;

    int current_index = 0;
    int         mid_index = NUM_SWEEP_RUNS / 2;
    double      Summary_Vc_avg[NUM_SWEEP_RUNS] = { 0.0, };
    double      Summary_Wgyro_avg[NUM_SWEEP_RUNS] = { 0.0, };

    // Initialize center point
    Summary_Vc_avg[mid_index] = Vcmd_offset;
    Summary_Wgyro_avg[mid_index] = 0.0;

    const char* OutDirName = "Static";
    const char* OutFileName = "sweep_summary.out";

    // Updated label to [deg/s] to match the RAD2DEG conversion below
    Dataset     Out_static_Dataset[] = {
        {"V_c[V]", Summary_Vc_avg},
        {"\\omega_{gyro_avg}[deg/s]",  Summary_Wgyro_avg},
    };

    for (int mag = 0; mag < NUM_SWEEP_RUNS / 2; mag++) {

        for (int j = 0; j < 2; j++) {
            PauseDAQ(taskAO);
            printf("\n[%d/%d]", (2 * mag + 1) + j, NUM_SWEEP_RUNS);

            if (j == 0) { // Positive Direction
                // Use fabs to check magnitude so logic works for negative sweeps
                if (fabs(Vc_pos) > V_CLOSE_INITIAL && fabs(Vc_pos) < V_CLOSE_FINISH) Vstep = V_CLOSE_STEP;
                else Vstep = V_WIDE_STEP;

                Vc_pos = Vc_pos + Vstep;
                Vc_current = Vc_pos;
                current_index = mid_index + mag + 1;
            }
            else { // Negative Direction
                if (fabs(Vc_neg) > V_CLOSE_INITIAL && fabs(Vc_neg) < V_CLOSE_FINISH) Vstep = V_CLOSE_STEP;
                else Vstep = V_WIDE_STEP;

                Vc_neg = Vc_neg - Vstep;
                Vc_current = Vc_neg;
                current_index = mid_index - mag - 1;
            }

            Wgyro_avg = Find_Wgyro_avg(taskAI, taskAO, "Vc2Wgyro", Vc_current, Vgyro_offset);

            Summary_Vc_avg[current_index] = Vc_current;
            Summary_Wgyro_avg[current_index] = Wgyro_avg * RAD2DEG;
        }
    }
    SaveDataset(OutDirName, OutFileName, Out_static_Dataset, 2, NUM_SWEEP_RUNS);
}

