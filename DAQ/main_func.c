#include ".\include\myHeader.h"

#define   V_CLOSE_INITIAL   (double) -0.7
#define   V_CLOSE_FINISH    (double) 0.7

#define   V_WIDE_STEP       (double) 0.02
#define   V_CLOSE_STEP      (double) 0.02

#define   NUM_SWEEP_RUNS   (int) 70

void Modi_ObtainMotorStaticProperty();

TaskHandle  taskAI = 0;
TaskHandle  taskAO = 0;

double      Vgyro_offset = 0.0;

int         User_mode = 0;

int main(void)
{
    

    DAQ_Init(DEV_NUM);
    // 1. 순서도 작성하라, Flow가 전부 보이게하라
    // 2. 함수하고 스위치로
    Vgyro_offset = Find_Vgyro_offset();

    User_mode = SelectOperatingMode();

    // 이 스위치 구문은 어디로 들어가한다. 
    switch (User_mode) {
        case 1: // 1 (번호로 되어있는 것을 유저 모드)
            printf("[Obtain Motor Static Property] ...\n"); // 텍스트 array로 만들어서 저장하라
            ObtainMotorStaticProperty();
            break;
        case 2:
            printf("[Motor Validation] ...\n");
            Validation();
            break;
        case 3:
            printf("[Slewrate Limit] ...\n");
            Slewrate();
            break;
        case 4:
            printf("[Bode Mag] ...\n");
            BodeMag();
            break;
        case 5:
            printf("[Bode Mag] ...\n");
            Modi_ObtainMotorStaticProperty();
            break;
        default:
            printf("[ERROR] Wrong answer %d", User_mode);
            break;
    }
    DAQ_Close();

    return 0;
}

int SelectOperatingMode() {
    int User_answer = 0;

    printf("\n[Select Operating Mode] =====\n");
    printf("1: Obtain Motor Static Property\n");
    printf("2: Motor Validation\n");
    printf("3: Slewrate Limit\n");
    printf("4: Bode Mag\n");
    printf("5: Linearized mapping \n");
    printf("==============================\n");
    printf("input: ");
    scanf_s("%d", &User_answer);
    printf("\n");

    return User_answer;
}

void Modi_ObtainMotorStaticProperty() {
    double      Vcmd = 0.0;

    double      Vc_pos = 0;
    double      Vc_neg = 0;

    double      Vc_current = 0.0;
    double      Wgyro_avg = 0.0;

    double      Vstep = 0.0;

    int current_index = 0;
    int         mid_index = NUM_SWEEP_RUNS / 2;
    double      Summary_Vc_avg[NUM_SWEEP_RUNS] = { 0.0, };
    double      Summary_Wgyro_avg[NUM_SWEEP_RUNS] = { 0.0, };

    // Initialize center point
    Summary_Vc_avg[mid_index] = 0;
    Summary_Wgyro_avg[mid_index] = 0.0;

    const char* OutDirName = "Static";
    const char* OutFileName = "Linear_07.out";

    // Updated label to [deg/s] to match the RAD2DEG conversion below
    Dataset     Out_static_Dataset[] = {
        {"V_c[V]", Summary_Vc_avg},
        {"\\omega_{gyro_avg}[deg/s]",  Summary_Wgyro_avg},
    };

    for (int mag = 0; mag < NUM_SWEEP_RUNS / 2; mag++) {

        for (int j = 0; j < 2; j++) {
            DAQ_Pause();
            printf("\n[%d/%d]", (2 * mag + 1) + j, NUM_SWEEP_RUNS);

            if (j == 0) { // Positive Direction
                // Use fabs to check magnitude so logic works for negative sweeps
                Vstep = V_WIDE_STEP;

                Vc_pos = Vc_pos + Vstep;
                Vc_current = Vc_pos;
                current_index = mid_index + mag + 1;
            }
            else { // Negative Direction
                Vstep = V_WIDE_STEP;

                Vc_neg = Vc_neg - Vstep;
                Vc_current = Vc_neg;
                current_index = mid_index - mag - 1;
            }
            Vcmd = Linear_func2(Vc_current,Vdz);

            Wgyro_avg = Find_Wgyro_avg("Vc2Wgyro", Vcmd);

            Summary_Vc_avg[current_index] = Vc_current;
            Summary_Wgyro_avg[current_index] = Wgyro_avg * RAD2DEG;
        }
    }
    SaveDataset(OutDirName, OutFileName, Out_static_Dataset, 2, NUM_SWEEP_RUNS);
}

// 

void Dynamic_function(double Final_time, const char* OutDirName, const char* OutFileName, double (*Processing)(double, double, double, double)) {
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
        Vcmd = Processing(Vcmd, time, Wgyro, Vpoten);

        // 2-1. Linearization
        Vc = Linear_func2(Vcmd, Vdz);

        // 3. Write and Read
        DAQ_Write(Vc);
        DAQ_Read(Vin);

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

