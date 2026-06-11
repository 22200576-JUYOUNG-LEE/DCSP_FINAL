#include "Header.h"

DAQ_Averages Data_avg = { 0.0, 0.0, 0.0, 0.0 };

void InitAvg(void);

void InitAvg(void) {
    Data_avg.Command = 0.0;
    Data_avg.Vgyro = 0.0;
    Data_avg.Wgyro = 0.0;
    Data_avg.Angle = 0.0;
}



void MotorDynamic(double Final_time, const char* OutDirName, const char* OutFileName, DynFn fn) {

    float64     Vin[NUM_AI_CHANNELS] = { 0.0 };

    int         idx_max = Final_time * SAMPLING_FREQ;

    int         count = 0;
    int         count_avg = 0;

    double      time_curr = 0.0;    //[sec]
    double      time_init = 0.0;    //[sec]
    double      time = 0.0;         //[sec]

    double      Command = 0.0;         //[V]
    double      Vc = DAQ_V_STANDARD;//[V]
    double      Vgyro = 0.0;        //[V]
    double      Vpoten = 0.0;       //[V]
    double      Wgyro = 0.0;        //[rad/sec]  

    static double      Out_Time[N_MAX_BUFFER] = { 0.0, };
    static double      Out_Command[N_MAX_BUFFER] = { 0.0, };
    static double      Out_Vgyro[N_MAX_BUFFER] = { 0.0, };
    static double      Out_Angle[N_MAX_BUFFER] = { 0.0, };
    static double      Out_Wgyro[N_MAX_BUFFER] = { 0.0, };

    DynState s = { time, Command, Wgyro, Vpoten };

    Dataset     Out_DAQ_Dataset[] = {
        {"Time[sec]",               Out_Time},
        {"\\omega_{cmd}[rad/s]",    Out_Command},
        {"V_{gyro}[V]",             Out_Vgyro},
        {"\\omega_{gyro}[rad/s]",   Out_Wgyro},
        {"\\theta[rad]",            Out_Angle}
    };
    int         numDataset = sizeof(Out_DAQ_Dataset) / sizeof(Out_DAQ_Dataset[0]);

    double keyboard_input = 0.0;

    InitAvg();

    time_init = GetWindowTime();
    time_curr = time_init;

    do
    {
        time = (time_curr - time_init) * 0.001;
        Vc = DAQ_V_STANDARD;

        s.Time = time;
        s.Command = Command;
        s.Wgyro = Wgyro;
        s.Vpoten = Vpoten;

        // 2. Processing
        if (fn != NULL) Command = fn(s);

        if (RUN_DAQ_mode == RUN_MODE_LINEAR) Vc = Linearization(Command);
        else Vc = Command;

        // 3. Write and Read
        DAQ_Write(Vc);
        DAQ_Read(Vin);

        Vgyro = Vin[2];
        Wgyro = (Vgyro - Vgyro_offset) * (V_GYRO2RAD);
        Vpoten = Vin[3];

        // 4. Save to buffers
        Out_Time[count] = time;
        Out_Command[count] = Command;
        Out_Vgyro[count] = Vin[2];
        Out_Wgyro[count] = Wgyro;
        Out_Angle[count] = V_POTEN2RAD(Vpoten);
        

        if (count >= idx_max / 2) {
            count_avg++;

            Data_avg.Command  = CAL_AVG(count_avg, Data_avg.Command, Out_Command[count]);
            Data_avg.Vgyro = CAL_AVG(count_avg, Data_avg.Vgyro, Out_Vgyro[count]);
            Data_avg.Wgyro = CAL_AVG(count_avg, Data_avg.Wgyro, Out_Wgyro[count]);
            Data_avg.Angle = CAL_AVG(count_avg, Data_avg.Angle, Out_Angle[count]);
        }

        if (_kbhit()) {
            if (_getch() == 's') {
                DAQ_Close();
                exit(0);
            }
        }

        // @. Wite for endging a tick
        while (1)
        {
            time_curr = GetWindowTime();
            if (time_curr - time_init - count * SAMPLING_TIME * 1000.0
                >= (SAMPLING_TIME * 1000.0)) break;
        }
        count++;
    } while (count < idx_max);

    SaveDataset(OutDirName, OutFileName, Out_DAQ_Dataset, numDataset, idx_max);
}

