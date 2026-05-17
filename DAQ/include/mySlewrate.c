#include "myHeader.h"

#define   NUM_SWEEP_RUNS        (int)   (22)


void Slewrate() {

    double      Vstep = 0.1; // [V]
    double      Vcmd = 0.0;
    double      Wgyro_avg = 0.0;

    double      Summary_Vcmd_avg[NUM_SWEEP_RUNS] = { 0.0, };
    double      Summary_Wgyro_avg[NUM_SWEEP_RUNS] = { 0.0, };

    Dataset     Out_static_Dataset[] = {
        {"V_c[V]", Summary_Vcmd_avg},
        {"\\omega_{gyro_avg}[rad/s]",  Summary_Wgyro_avg},
    };
    int         numDataset = sizeof(Out_static_Dataset) / sizeof(Out_static_Dataset[0]);


    for (int mag = 18; mag < NUM_SWEEP_RUNS; mag++) {
        DAQ_Pause();
        printf("\n[%d/%d]", (mag + 1), NUM_SWEEP_RUNS);

        Vcmd = (mag + 1) * Vstep;

        Wgyro_avg = modi_Find_Wgyro_avg(Vcmd, "Slewrate");

        Summary_Vcmd_avg[mag] = Vcmd;
        Summary_Wgyro_avg[mag] = Wgyro_avg;
    }
}

double modi_Find_Wgyro_avg(double Vcmd, char* OutDirName) {
    float64     Vin[NUM_AI_CHANNELS] = { 0.0 };

    double      Wgyro_avg = 0.0;    //[rad/sec]
    double      time_curr = 0.0;
    double      time_init = 0.0;
    double      time = 0.0;         //[sec]

    double      Vc = 0.0;
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

    printf("\nVcmd = %.2f V\n", Vcmd);
    for (int count = 0; count < N_STEP; count++)
    {
        time = (time_curr - time_init) * 0.001; // [sec]

        if (count > N_STEP / 2) {
            Vc = Linear_func2(Vcmd, Vdz);

            DAQ_Write(Vc);
            DAQ_Read(Vin);

            Vgyro_curr = Vin[2];
            Wgyro = Kg * (Vgyro_curr - Vgyro_offset);
            Vpoten = Vin[3];
        }

        // 5. Save to buffers
        Out_Time[count] = time;
        Out_Wgyro_Data[count] = Wgyro * RAD2DEG;

        // @. Emergency Stop
        if (_kbhit())
            if (_getch() == 's') {
                DAQ_Close();
                exit(0);
            }

        while (1)
        {
            time_curr = GetWindowTime();
            if (time_curr - time_init - count * SAMPLING_TIME * 1000.0
                >= (SAMPLING_TIME * 1000.0)) break;
        }
    }

    sprintf(OutFileName, "Vcmd_%.2f_data.out", Vcmd);
    SaveDataset(OutDirName, OutFileName, Out_DAQ_Dataset, numDataset, N_STEP);

    return      Wgyro_avg;
}