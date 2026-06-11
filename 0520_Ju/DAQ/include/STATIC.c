#include "Header.h"

static double      Vcmd = 0.0;

char OutDirName[NAME_BUFFER] = { 0 };
char OutFileName[NAME_BUFFER] = { 0 };
char OutDataFileName[NAME_BUFFER] = { 0 };

double Static_Step(DynState s);
void InitPath(void);


void IterStaticProperty(int Static_mode) {
    int idx = 0; 

    for (idx = 0; idx < STATIC_ITER_MAX; idx++) {
        printf("\n==============================\n");
        printf("\tIteration: %d", idx + 1);
        printf("\n==============================\n");
        StaticProperty(Static_mode, idx + 1);
    }

}


void StaticProperty(int Static_mode, int Iter_num){
    double      Vc_pos = 0.0;
    double      Vc_neg = 0.0;

    double      Vstep = 0.0;

    int         idx_curr = 0;
    int         idx_mid = STATIC_N_SWEEP / 2;
    double      Summary_Vc_avg[STATIC_N_SWEEP] = { 0.0, };
    double      Summary_Wgyro_avg[STATIC_N_SWEEP] = { 0.0, };

    Dataset     Out_static_Dataset[] = {
            {"V_cmd[V]", Summary_Vc_avg},
            {"\\omega_{gyro_avg}[deg/s]",  Summary_Wgyro_avg},
    };

    InitPath();

    if (Static_mode == NON_LINEAR) {
        printf("[Static Property Non-Linear] ...\n");// modify!

        Vc_pos = STATIC_POSITIVE_START_POINT;
        Vc_neg = STATIC_NEGATIVE_START_POINT;

        strcpy(OutDirName, STATIC_DIR);
        sprintf(OutFileName, "Static_iter%d.out", Iter_num);

        RUN_DAQ_mode = NON_LINEAR;
    }
    else {
        printf("[Static Property Linear] ...\n");// modify!

        Vc_pos = 0.0;
        Vc_neg = 0.0;

        strcpy(OutDirName, STATIC_DIR_LIN);
        strcpy(OutFileName, STATIC_FILE);

        RUN_DAQ_mode = LINEAR;
    }

    for (int mag = 0; mag < STATIC_N_SWEEP / 2; mag++) {

        for (int j = 0; j < 2; j++) {
            
            printf("\n[%d/%d]", (2 * mag + 1) + j, STATIC_N_SWEEP);

            DAQ_Pause(STATIC_TIME_PAUSE);

            if (j == SIGN_POSITIVE) {

                Vstep = (fabs(Vc_pos) > STATIC_CLOSE_NEGATIVE_POINT &&
                    fabs(Vc_pos) < STATIC_CLOSE_POSITIVE_POINT)
                    ? STATIC_CLOSE_STEP : STATIC_WIDE_STEP;

                Vc_pos = Vc_pos + Vstep;
                Vcmd = Vc_pos;
                idx_curr = idx_mid + mag;
            }
            else {

                Vstep = (fabs(Vc_neg) > STATIC_CLOSE_NEGATIVE_POINT &&
                    fabs(Vc_neg) < STATIC_CLOSE_POSITIVE_POINT)
                    ? STATIC_CLOSE_STEP : STATIC_WIDE_STEP;

                Vc_neg = Vc_neg - Vstep;
                Vcmd = Vc_neg;
                idx_curr = idx_mid - mag - 1;
            }

            //printf("\nVcmd is %.2f\n", Vcmd);// -> nice work!
            //printf("idx_curr: %d\n", idx_curr); // -> nice work!
            
            sprintf(OutDataFileName, "Static_Step_%.2f.out", Vcmd);

            RunDAQ(STATIC_TIME_FINAL, STATIC_DATA_DIR, OutDataFileName, Static_Step);
            printf("\nVcmd: %.3f [V]", Vcmd);
            printf("\nWgyro avg: %.4f [rad/s]\n", g_daqAvg.Wgyro);

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
    memset(OutFileName, 0, NAME_BUFFER);
    memset(OutDirName, 0, NAME_BUFFER);
    memset(OutDataFileName, 0, NAME_BUFFER);
} 
