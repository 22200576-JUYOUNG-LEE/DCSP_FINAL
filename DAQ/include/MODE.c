#include "Header.h"

DAQ_Averages g_daqAvg = { 0.0, 0.0, 0.0, 0.0 };

static const char* MODE_LABELS[N_MODE] = {
    "Obtain Motor Static Property",
    "Linearized Mapping",
    "Motor Validation",
    "Bode Mag",
};

Mode SelectOperatingMode(void){
    int answer = 0;

    printf("\n[Select Mode] ================\n");
    for (int i = 0; i < N_MODE; i++)
        printf("%d: %s\n", i + 1, MODE_LABELS[i]);
    printf("==============================\n");
    printf("Input: ");
    scanf_s("%d", &answer);
    printf("\n");

    return (Mode)answer;
}

void RunMode(void)
{

    switch (SelectOperatingMode()) {
        case MODE_STATIC:        StaticProperty(NON_LINEAR);         break;
        case MODE_STATIC_LINEAR: StaticProperty(LINEAR);             break;
        case MODE_VALIDATION:    StaticValidation();                 break;
        case MODE_BODE:          BodeMag();                          break;
        
        default: printf("[ERROR] Unknown mode:\n");                  break;
    }
}

void InitAvg(void) {
    g_daqAvg.Vcmd = 0.0;
    g_daqAvg.Vgyro = 0.0;
    g_daqAvg.Wgyro = 0.0;
    g_daqAvg.Angle = 0.0;
}

void RunDAQ(double Final_time, const char* OutDirName, const char* OutFileName, DynFn fn) {

    float64     Vin[NUM_AI_CHANNELS] = { 0.0 };

    int         idx_max = Final_time * SAMPLING_FREQ;

    int         count_avg = 0;

    double      time_curr = 0.0;    //[sec]
    double      time_init = 0.0;    //[sec]
    double      time = 0.0;         //[sec]

    double      Vcmd = 0.0;         //[V]
    double      Vc = 0.0;           //[V]
    double      Vgyro = 0.0;        //[V]
    double      Vpoten = 0.0;       //[V]
    double      Wgyro = 0.0;        //[rad/sec]  
    
    static double      Out_Time[N_MAX_BUFFER] = { 0.0, };
    static double      Out_Vcmd[N_MAX_BUFFER] = { 0.0, };
    static double      Out_Vgyro[N_MAX_BUFFER] = { 0.0, };
    static double      Out_Theta[N_MAX_BUFFER] = { 0.0, };
    static double      Out_Wgyro[N_MAX_BUFFER] = { 0.0, };

    DynState s = {time, Vcmd, Wgyro, Vpoten };

    Dataset     Out_DAQ_Dataset[] = {
        {"Time[sec]",               Out_Time},
        {"V_{cmd}[V]",              Out_Vcmd},
        {"V_{gyro}[V]",             Out_Vgyro},
        {"\\theta[deg]",            Out_Theta},
        {"\\omega_{gyro}[deg/sec]", Out_Wgyro}
    };
    int         numDataset = sizeof(Out_DAQ_Dataset) / sizeof(Out_DAQ_Dataset[0]);

    InitAvg();

    time_init = GetWindowTime();
    time_curr = time_init;

    for (int count = 0; count < idx_max; count++) {
        time = (time_curr - time_init) * 0.001;

        s.Time = time;
        s.Vcmd = Vcmd;
        s.Wgyro = Wgyro;
        s.Vpoten = Vpoten;

        // 2. Processing
        Vcmd = fn(s);

        if (RUN_DAQ_mode == LINEAR) Vc = Linearization(Vcmd);
        else Vc = Vcmd;

        // 3. Write and Read
        DAQ_Write(Vc);
        DAQ_Read(Vin);

        Vgyro = Vin[2];
        Wgyro = (Vgyro - Vgyro_offset) * (GYRO_V2RADS);
        Vpoten = Vin[3];

        // 4. Save to buffers
        Out_Time[count] = time;
        Out_Vcmd[count] = Vcmd;
        Out_Vgyro[count] = Vin[2];
        Out_Theta[count] = Vpoten * (SCALE_REV2DEG) / V_POTEN;
        Out_Wgyro[count] = Wgyro * SCALE_RAD2DEG;

        if (count >= idx_max / 2) {
            count_avg++;

            g_daqAvg.Vcmd = CAL_AVG(count_avg, g_daqAvg.Vcmd, Out_Vcmd[count]);
            g_daqAvg.Vgyro = CAL_AVG(count_avg, g_daqAvg.Vgyro, Out_Vgyro[count]);
            g_daqAvg.Angle = CAL_AVG(count_avg, g_daqAvg.Angle, Out_Theta[count]);
            g_daqAvg.Wgyro = CAL_AVG(count_avg, g_daqAvg.Wgyro, Out_Wgyro[count]);
        }

        // @. Emergency Stop
        if (_kbhit())
            if (_getch() == 's') {
                DAQ_Close();
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


