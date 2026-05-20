#include "Header.h"

double freq = 0.0;

static double Bode_sin(DynState s)
{
    if      (s.Time < BODE_SIN_TIME_1) return 0.0;
    else if (s.Time < BODE_SIN_TIME_2) return BODE_SIN_AMPLITUDE * sin(2.0 * UNIT_PI * freq * (s.Time - BODE_SIN_TIME_1));
    else                   return 0.0;
}

void BodeMag(void)
{
    int idx = 0;

    InitPath();
    RUN_DAQ_mode = LINEAR;
    freq = 0.0;

    printf("\nBodeMag - sin\n");
    for (idx = BODE_ITER_START; idx < BODE_ITER_MAX; idx++) {
        freq = idx * BODE_SIN_FREQ_STEP;

        sprintf(OutFileName, "BodeMag_sin_freq%.2f.out", freq);
        
        DAQ_Pause(BODE_TIME_PAUSE); 

        RunDAQ(BODE_SIN_TIME_FINAL, BODE_DIR , OutFileName, Bode_sin);
    }
}
