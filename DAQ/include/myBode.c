#include "myHeader.h"

#define   BODE_ITER_START (int) 0
#define   BODE_ITER_MAX   (int) 30




void BodeMag(TaskHandle taskAI, TaskHandle taskAO, double Vgyro_offset) {
    double      Final_time_sin = 6;

    const char* OutDirName = "BodeMag";
    char        OutFileName[100] = { "" };

    double      freq = 0.0;

    printf("\nBodeMag - sin\n");

    for (int iter = BODE_ITER_START; iter < BODE_ITER_MAX; iter++) {
        freq = iter * 0.2;

        sprintf(OutFileName, "BodeMag_sin_freq%.2f.out", freq);

        PauseDAQ(taskAO);
        PauseDAQ(taskAO);
        Bode_Dynamic_function(taskAI, taskAO, Vgyro_offset, freq, Final_time_sin, OutDirName, OutFileName, Bode_sin);
    }

}

void Bode_Dynamic_function(TaskHandle taskAI, TaskHandle taskAO, double Vgyro_offset, double freq, double Final_time,
    const char* OutDirName, const char* OutFileName, double (*Processing)(double, double, double, double, double)) {
    float64     Vin[NUM_AI_CHANNELS] = { 0.0 };

    int         idx_max = Final_time * SAMPLING_FREQ;

    double      time_curr = 0.0;    //[sec]
    double      time_init = 0.0;    //[sec]
    double      time = 0.0;         //[sec]

    double      Vcmd = 0.0;         //[V]
    double      Vc = 0.0;           //[V]
    double      Vgyro = 0.0;        //[V]
    double      Vpoten = 0.0;       //[V]
    double      Wgyro = 0.0;        //[rad/sec]  

    static double      Out_Time[DYNA_N_STEP_MAX] = { 0.0, };
    static double      Out_ao1_Data[DYNA_N_STEP_MAX] = { 0.0, };
    static double      Out_ai2_Data[DYNA_N_STEP_MAX] = { 0.0, };
    static double      Out_ai3_Data[DYNA_N_STEP_MAX] = { 0.0, };
    static double      Out_Wgyro_Data[DYNA_N_STEP_MAX] = { 0.0, };

    Dataset     Out_DAQ_Dataset[] = {
        {"Time[sec]", Out_Time},
        {"V_{cmd}[V]",  Out_ao1_Data},
        {"V_{gyro}[V]", Out_ai2_Data},
        {"\\theta[deg]", Out_ai3_Data},
        {"\\omega_{gyro}[deg/sec]", Out_Wgyro_Data}
    };
    int         numDataset = sizeof(Out_DAQ_Dataset) / sizeof(Out_DAQ_Dataset[0]);

    // 1. Init time tick
    time_init = GetWindowTime();
    time_curr = time_init;

    for (int count = 0; count < idx_max; count++) {
        time = (time_curr - time_init) * 0.001;

        // 2. Processing
        Vcmd = Processing(freq, Vcmd, time, Wgyro, Vpoten);

        // 2-1. Linearization
        Vc = Linear_func2(Vcmd, Vdz);

        // 3. Write and Read
        WriteDAQ(taskAO, Vc);
        ReadDAQ(taskAI, Vin);

        Vgyro = Vin[2];
        Wgyro = Kg * (Vgyro - Vgyro_offset);
        Vpoten = Vin[3];

        // 4. Save to buffers
        Out_Time[count] = time;
        Out_ao1_Data[count] = Vcmd;
        Out_ai2_Data[count] = Vin[2];
        Out_ai3_Data[count] = Vpoten * (REV) / Vpoten_MAX;
        Out_Wgyro_Data[count] = Wgyro * RAD2DEG;

        // @. Emergency Stop
        if (_kbhit())
            if (_getch() == 's') {
                CloseDAQ(taskAI, taskAO);
                exit(0);
            }
        // @. Wite for endging a tick
        while (1)
        {
            time_curr = GetWindowTime();
            if (time_curr - time_init - count * SAMPLING_TIME * 1000.0
                >= (SAMPLING_TIME * 1000.0)) break;
        }
    }

    SaveDataset(OutDirName, OutFileName, Out_DAQ_Dataset, numDataset, idx_max);
}

double Bode_sin(double freq, double Vcmd, double time, double Wgyro, double Vpoten) {

    if (time < 1.0) return 0;
    else if (time < 5.0) return 0.7 * sin(2 * UNIT_PI * freq * (time - 1.0));
    else return 0;

}

