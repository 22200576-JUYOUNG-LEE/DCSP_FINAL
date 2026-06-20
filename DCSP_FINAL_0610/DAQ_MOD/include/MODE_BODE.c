#include "Header.h"

double freq = 0.0;

double Bode_sin(DynState s);

void BodeMag(void)
{
    int idx = 0;

    InitPath();
    RUN_DAQ_mode = RUN_MODE_LINEAR;
    freq = 0.0;

    printf("\nBodeMag - sin\n");
    for (idx = 0; idx < BODE_ITER_MAX; idx++) {
        freq = idx * BODE_SIN_FREQ_STEP + BODE_INIT_FREQ;

        sprintf(OutFileName, "BodeMag_sin_freq%.2f.out", freq);

        DAQ_Pause(BODE_TIME_PAUSE);

        MotorDynamic(BODE_SIN_TIME_FINAL, BODE_DIR, OutFileName, Bode_sin);
    }
}

double Bode_sin(DynState s)
{
    return BODE_SIN_AMPLITUDE * sin(2.0 * UNIT_PI * freq * (s.Time));
}