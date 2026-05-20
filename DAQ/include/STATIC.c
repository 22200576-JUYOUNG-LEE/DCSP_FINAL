#include "Header.h"

static double      Vcmd = 0.0;

char OutDirName[N_NAME_BUFFER] = { 0 };
char OutFileName[N_NAME_BUFFER] = { 0 };

double Static_Step(DynState s);
void InitPath(void);

void StaticProperty(int Static_mode){
    double      Vc_pos = 0.0;
    double      Vc_neg = 0.0;

    double      Vstep = STATIC_V_STEP;

    int         idx_curr = 0;
    int         idx_mid = STATIC_N_SWEEP / 2;
    double      Summary_Vc_avg[STATIC_N_SWEEP] = { 0.0, };
    double      Summary_Wgyro_avg[STATIC_N_SWEEP] = { 0.0, };

    Dataset     Out_static_Dataset[] = {
            {"V_c[V]", Summary_Vc_avg},
            {"\\omega_{gyro_avg}[deg/s]",  Summary_Wgyro_avg},
    };

    InitPath();

    if (Static_mode == NON_LINEAR) {
        printf("[Static Property Non-Linear] ...\n");// modify!

        Vc_pos = DAQ_V_STANDARD;
        Vc_neg = DAQ_V_STANDARD;

        strcpy(OutDirName, STATIC_DIR);

        RUN_DAQ_mode = NON_LINEAR;
    }
    else {
        printf("[Static Property Linear] ...\n");// modify!

        Vc_pos = 0.0;
        Vc_neg = 0.0;

        strcpy(OutDirName, STATIC_DIR_LIN);

        RUN_DAQ_mode = LINEAR;
    }

    //printf("\nThe Static proterty mode is %d [0 is Non]\n", RUN_DAQ_mode);

    for (int mag = 0; mag < STATIC_N_SWEEP / 2; mag++) {

        for (int j = 0; j < 2; j++) {
            
            printf("\n[%d/%d]", (2 * mag + 1) + j, STATIC_N_SWEEP);

            DAQ_Pause(STATIC_TIME_PAUSE);

            if (j == STATIC_SIGN_POSITIVE) {
                Vc_pos = Vc_pos + Vstep;
                Vcmd = Vc_pos;
                idx_curr = idx_mid + mag;
            }
            else {
                Vc_neg = Vc_neg - Vstep;
                Vcmd = Vc_neg;
                idx_curr = idx_mid - mag - 1;
            }

            //printf("\nVcmd is %.2f\n", Vcmd);// -> nice work!
            //printf("idx_curr: %d\n", idx_curr); // -> nice work!
            sprintf(OutFileName, "Static_Step_%.2f.out", Vcmd);

            RunDAQ(STATIC_TIME_FINAL, STATIC_DATA_DIR, OutFileName, Static_Step);

            Summary_Vc_avg[idx_curr] =    g_daqAvg.Vcmd;
            Summary_Wgyro_avg[idx_curr] = g_daqAvg.Wgyro;
        }
    }
    SaveDataset(OutDirName, STATIC_FILE, Out_static_Dataset, 2, STATIC_N_SWEEP);
}

double Static_Step(DynState s) {
    //if (s.Time < STATIC_TIME_DELAY ) return DAQ_V_STANDARD;
    return Vcmd;
}

void InitPath(void) {
    memset(OutFileName, 0, N_NAME_BUFFER);
    memset(OutDirName, 0, N_NAME_BUFFER);
}
