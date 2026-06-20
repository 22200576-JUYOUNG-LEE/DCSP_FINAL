#include "Header.h"

double Check_freq = 1.0 ; // [Hz]

double Check_signal(DynState s);
void Check_Dynamic(double Final_time, const char* OutDirName, const char* OutFileName, DynFn fn);

void Check_DAQ(void) {
    int idx = 0;

    Check_freq = 1.0;

    printf("\nChecking DAQ (2.5 * sin(0.5Hz) + 2.5 \n");

    Check_Dynamic(CHECK_TIME_FINAL, CHECK_DIR, CHECK_FILE, Check_signal);

}


double Check_signal(DynState s) {
    return CHECK_SIN_AMPLITUDE * sin(2.0 * UNIT_PI * Check_freq * (s.Time)) + CHECK_OFFSET;
}


void Check_Dynamic(double Final_time, const char* OutDirName, const char* OutFileName, DynFn fn) {

    float64     Vin[NUM_AI_CHANNELS] = { 0.0 };

    int         idx_max = Final_time * SAMPLING_FREQ;

    int         count = 0;

    double      time_curr   = 0.0;    //[sec]
    double      time_init   = 0.0;    //[sec]
    double      time        = 0.0;         //[sec]

    double      Command     = 0.0; //[V]

    static double      Out_Time     [N_MAX_BUFFER] = { 0.0, };
    static double      Out_Command  [N_MAX_BUFFER] = { 0.0, };

    static double      Out_No0[N_MAX_BUFFER] = { 0.0, };
    static double      Out_No1[N_MAX_BUFFER] = { 0.0, };
    static double      Out_No2[N_MAX_BUFFER] = { 0.0, };
    static double      Out_No3[N_MAX_BUFFER] = { 0.0, };


    DynState s = { time, Command, 0.0, 0.0 };

    Dataset     Out_DAQ_Dataset[] = {
        {"Time[sec]",      Out_Time},
        {"Command",        Out_Command},

        {"PortNo.0",       Out_No0},
        {"PortNo.1",       Out_No1},
        {"PortNo.2",       Out_No2},
        {"PortNo.3",       Out_No3},

    };
    int  numDataset = sizeof(Out_DAQ_Dataset) / sizeof(Out_DAQ_Dataset[0]);

    time_init = GetWindowTime();
    time_curr = time_init;

    do
    {
        time = (time_curr - time_init) * 0.001;

        s.Time = time;
        s.Command = Command;


        // 2. Processing
        Command = fn(s);

        // 3. Write and Read
        DAQ_Write(Command);
        DAQ_Read(Vin);

        // 4. Save to buffers
        Out_Time    [count] = time;
        Out_Command [count] = Command;

        Out_No0     [count] = Vin[0];
        Out_No1     [count] = Vin[1];
        Out_No2     [count] = Vin[2];
        Out_No3     [count] = Vin[3];

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

