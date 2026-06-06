#include "Header.h"

static double      Vcmd = 0.0;
static int         Iter_num = 0;
char OutDirName[N_NAME_BUFFER] = { 0 };
char OutFileName[N_NAME_BUFFER] = { 0 };
char OutDataFileName[N_NAME_BUFFER] = { 0 };

double Static_Step(DynState s);
void InitPath(void);

void InitPath(void) {
    memset(OutFileName, 0, N_NAME_BUFFER);
    memset(OutDirName, 0, N_NAME_BUFFER);
    memset(OutDataFileName, 0, N_NAME_BUFFER);
}

void IterStaticProperty(int Static_mode) {

    for (Iter_num = 0; Iter_num < STATIC_ITER_MAX; Iter_num++) {
        printf("\n==============================\n");
        printf("\tIteration: %d", Iter_num + 1);
        printf("\n==============================\n");
        StaticProperty(Static_mode);
    }

}

void StaticProperty(int Static_mode){
    double      Vc_pos = 0.0;
    double      Vc_neg = 0.0;

    double      Vstep = 0.0;

    int         idx_curr = 0;
    int         idx_mid = STATIC_N_SWEEP / 2;
    double      Summary_Vcmd_avg[STATIC_N_SWEEP] = { 0.0, };
    double      Summary_Wgyro_avg[STATIC_N_SWEEP] = { 0.0, };
    double      Summary_Angle_avg[STATIC_N_SWEEP] = { 0.0, };

    Dataset     Out_static_Dataset[] = {
            {"V_cmd[V]", Summary_Vcmd_avg},
            {"\\omega_{gyroavg}[rad/s]",  Summary_Wgyro_avg},
    };

    InitPath();

    if (Static_mode == RUN_MODE_NONLINEAR) {

        Vc_pos = STATIC_POSITIVE_START_POINT;
        Vc_neg = STATIC_NEGATIVE_START_POINT;

        strcpy(OutDirName, STATIC_DIR);
        sprintf(OutFileName, "Static_iter%d.out", Iter_num);

        RUN_DAQ_mode = RUN_MODE_NONLINEAR;
    }
    else {

        Vc_pos = 0.0;
        Vc_neg = 0.0;

        strcpy(OutDirName, STATIC_DIR_LIN);
        strcpy(OutFileName, STATIC_FILE);

        RUN_DAQ_mode = RUN_MODE_LINEAR;
    }

    for (int mag = 0; mag < STATIC_N_SWEEP / 2; mag++) {

        for (int j = 0; j < 2; j++) {
            
            printf("\n[%d/%d]", (2 * mag + 1) + j, STATIC_N_SWEEP);

            DAQ_Pause(STATIC_TIME_PAUSE);

            if (j == STATIC_SIGN_POSITIVE) {
                Vstep = STATIC_GET_VSTEP(Vc_pos);
                Vc_pos = Vc_pos + Vstep;
                Vcmd = Vc_pos;
                idx_curr = idx_mid + mag;
            }
            else {
                Vstep = STATIC_GET_VSTEP(Vc_neg);
                Vc_neg = Vc_neg - Vstep;
                Vcmd = Vc_neg;
                idx_curr = idx_mid - mag - 1;
            }
            
            sprintf(OutDataFileName, "Static_Step_%.2f.out", Vcmd);

            RunDAQ(STATIC_TIME_FINAL, STATIC_DATA_DIR, OutDataFileName, Static_Step);
            printf("\nVcmd: %.3f [V]", Vcmd);
            printf("\nWgyro avg: %.4f [rad/s]\n", Data_avg.Wgyro);

            Summary_Vcmd_avg[idx_curr]  =    Data_avg.Vcmd;
            Summary_Wgyro_avg[idx_curr] =    Data_avg.Wgyro;
        }
    }
    SaveDataset(OutDirName, OutFileName, Out_static_Dataset, 2, STATIC_N_SWEEP);
}

double Static_Step(DynState s) {
    //if (s.Time < STATIC_TIME_DELAY ) return DAQ_V_STANDARD;
    return Vcmd;
}

