#include "myHeader.h"

#define V_CLOSE_INITIAL  2.20
#define V_CLOSE_FINISH   2.80
#define V_WIDE_STEP      0.02
#define V_CLOSE_STEP     0.02
#define NUM_SWEEP_RUNS   101

#define LIN_SWEEP_RUNS   70
#define LIN_VSTEP        0.02

// ── Gyro Offset Calibration ───────────────────────────────────
double Find_Vgyro_offset(void)
{
    float64 Vin[NUM_AI_CHANNELS] = { 0.0 };
    double  vgyro_offset = 0.0;
    double  time_init, time_curr;
    int     count_avg = 0;

    printf("\n[Calibration] Measuring Vgyro_offset...\n");
    DAQ_Reset();

    time_init = time_curr = GetWindowTime();

    for (int k = 0; k < N_STEP; k++) {
        DAQ_Read(Vin);

        if (k > N_STEP / 2)
            vgyro_offset = Cal_avg(++count_avg, vgyro_offset, Vin[2]);

        while (1) {
            time_curr = GetWindowTime();
            if (time_curr - time_init - k * SAMPLING_TIME * 1000.0
                    >= SAMPLING_TIME * 1000.0) break;
        }
    }

    printf("[Calibration] Vgyro_offset = %.3f V\n", vgyro_offset);
    return vgyro_offset;
}

// ── Average Angular Velocity at a Fixed Vc ───────────────────
double Find_Wgyro_avg(const char* dir, double Vc)
{
    float64 Vin[NUM_AI_CHANNELS] = { 0.0 };
    double  Wgyro_avg = 0.0, Wgyro = 0.0;
    double  time_init, time_curr, time = 0.0;
    int     count_avg = 0;

    static double Out_Time [N_STEP];
    static double Out_Wgyro[N_STEP];

    Dataset ds[] = {
        { "Time[sec]",               Out_Time  },
        { "\\omega_{gyro}[deg/sec]", Out_Wgyro },
    };

    char fname[100];
    snprintf(fname, sizeof(fname), "Vc_%.2f_data.out", Vc);

    printf("\nVc = %.4f V\n", Vc);

    time_init = time_curr = GetWindowTime();

    for (int k = 0; k < N_STEP; k++) {
        time = (time_curr - time_init) * 0.001;

        DAQ_Write(Vc);
        DAQ_Read(Vin);

        Wgyro = GYRO_V2RAD * (Vin[2] - Vgyro_offset);  // was: Kg * ... (bug fix)

        if (k > N_STEP / 2)
            Wgyro_avg = Cal_avg(++count_avg, Wgyro_avg, Wgyro);

        Out_Time [k] = time;
        Out_Wgyro[k] = Wgyro * RAD2DEG;

        if (_kbhit() && _getch() == 's') { DAQ_Close(); exit(0); }

        while (1) {
            time_curr = GetWindowTime();
            if (time_curr - time_init - k * SAMPLING_TIME * 1000.0
                    >= SAMPLING_TIME * 1000.0) break;
        }
    }

    printf("Wgyro_avg = %.6f [deg/s]\n", Wgyro_avg * RAD2DEG);
    SaveDataset(dir, fname, ds, 2, N_STEP);

    return Wgyro_avg;
}

// ── Static Property Sweep (Original) ─────────────────────────
void ObtainMotorStaticProperty(void)
{
    double Vc_pos = VCMD_OFFSET, Vc_neg = VCMD_OFFSET;
    double Vc_cur, Wgyro_avg, Vstep;
    int    mid = NUM_SWEEP_RUNS / 2, idx;

    double Sum_Vc   [NUM_SWEEP_RUNS] = { 0.0 };
    double Sum_Wgyro[NUM_SWEEP_RUNS] = { 0.0 };

    Sum_Vc   [mid] = VCMD_OFFSET;
    Sum_Wgyro[mid] = 0.0;

    Dataset ds[] = {
        { "V_c[V]",                   Sum_Vc    },
        { "\\omega_{gyro_avg}[deg/s]", Sum_Wgyro },
    };

    for (int mag = 0; mag < NUM_SWEEP_RUNS / 2; mag++) {
        for (int j = 0; j < 2; j++) {
            DAQ_Pause();
            printf("\n[%d/%d]", 2 * mag + 1 + j, NUM_SWEEP_RUNS);

            if (j == 0) {
                Vstep  = (fabs(Vc_pos) > V_CLOSE_INITIAL &&
                          fabs(Vc_pos) < V_CLOSE_FINISH)
                         ? V_CLOSE_STEP : V_WIDE_STEP;
                Vc_pos += Vstep;
                Vc_cur  = Vc_pos;
                idx     = mid + mag + 1;
            } 
            
            else {
                Vstep  = (fabs(Vc_neg) > V_CLOSE_INITIAL &&
                          fabs(Vc_neg) < V_CLOSE_FINISH)
                         ? V_CLOSE_STEP : V_WIDE_STEP;
                Vc_neg -= Vstep;
                Vc_cur  = Vc_neg;
                idx     = mid - mag - 1;
            }

            Wgyro_avg      = Find_Wgyro_avg("Vc2Wgyro", Vc_cur);
            Sum_Vc   [idx] = Vc_cur;
            Sum_Wgyro[idx] = Wgyro_avg * RAD2DEG;
        }
    }

    SaveDataset("Static", "sweep_summary.out", ds, 2, NUM_SWEEP_RUNS);
}

// ── Static Property Sweep (Linearized) ───────────────────────
void ObtainMotorStaticProperty_Linear(void)
{
    double Vc_pos = 0.0, Vc_neg = 0.0;
    double Vc_cur, Wgyro_avg;
    int    mid = LIN_SWEEP_RUNS / 2, idx;

    double Sum_Vc   [LIN_SWEEP_RUNS] = { 0.0 };
    double Sum_Wgyro[LIN_SWEEP_RUNS] = { 0.0 };

    Sum_Vc   [mid] = 0.0;
    Sum_Wgyro[mid] = 0.0;

    Dataset ds[] = {
        { "V_c[V]",                   Sum_Vc    },
        { "\\omega_{gyro_avg}[deg/s]", Sum_Wgyro },
    };

    for (int mag = 0; mag < LIN_SWEEP_RUNS / 2; mag++) {
        for (int j = 0; j < 2; j++) {
            DAQ_Pause();
            printf("\n[%d/%d]", 2 * mag + 1 + j, LIN_SWEEP_RUNS);

            if (j == 0) {
                Vc_pos += LIN_VSTEP;
                Vc_cur  = Vc_pos;
                idx     = mid + mag + 1;
            } else {
                Vc_neg -= LIN_VSTEP;
                Vc_cur  = Vc_neg;
                idx     = mid - mag - 1;
            }

            Wgyro_avg      = Find_Wgyro_avg("Vc2Wgyro", Linear_func2(Vc_cur, VDZ));
            Sum_Vc   [idx] = Vc_cur;
            Sum_Wgyro[idx] = Wgyro_avg * RAD2DEG;
        }
    }

    SaveDataset("Static", "Linear_07.out", ds, 2, LIN_SWEEP_RUNS);
}
