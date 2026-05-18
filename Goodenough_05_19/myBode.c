#include "myHeader.h"

#define BODE_ITER_START  0
#define BODE_ITER_MAX    30

// ── Bode Sinusoidal Signal (DynFn signature) ──────────────────
// Applies a sine burst from t=1s to t=5s at the given frequency.
static double Bode_sin(DynState s)
{
    if      (s.t < 1.0) return 0.0;
    else if (s.t < 5.0) return 0.7 * sin(2.0 * UNIT_PI * s.freq * (s.t - 1.0));
    else              return 0.0;
}

// ── Bode Magnitude Sweep ──────────────────────────────────────
void BodeMag(void)
{
    char fname[100];

    printf("\nBodeMag - sin\n");

    for (int i = BODE_ITER_START; i < BODE_ITER_MAX; i++) {
        double freq = i * 0.2;
        sprintf(fname, "BodeMag_sin_freq%.2f.out", freq);
        DAQ_Pause(); DAQ_Pause();
        Run_Dynamic(freq, 6.0, "BodeMag", fname, Bode_sin);
    }
}
