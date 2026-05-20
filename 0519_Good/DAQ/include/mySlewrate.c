#include "myHeader.h"

#define SLEW_ITER_START  18
#define SLEW_ITER_MAX    22
#define SLEW_VSTEP       0.1

// ── Slewrate Step Signal (DynFn signature) ────────────────────
// Motor is driven only in the second half of the run (k > N_STEP/2).
// We replicate that behaviour by gating on elapsed time:
//   first half  (t < FINAL_TIME/2): command stays at 0 → DAQ_Write receives
//               Linear_func2(0) which keeps the motor off.
//   second half (t >= FINAL_TIME/2): command ramps to the target Vcmd.
//
// freq carries the target Vcmd for this sweep step (re-used as a general
// parameter slot — its value is set by Slewrate() below).
static double Slew_step(DynState s)
{
    double target = s.freq;   // freq slot reused to pass target voltage

    if (s.t < FINAL_TIME / 2.0) return 0.0;
    else                       return target;
}

// ── Slewrate Sweep ────────────────────────────────────────────
void Slewrate(void)
{
    char fname[100];

    printf("\nSlewrate\n");

    for (int mag = SLEW_ITER_START; mag < SLEW_ITER_MAX; mag++) {
        double Vcmd_target = (mag + 1) * SLEW_VSTEP;

        printf("\n[%d/%d] Vcmd = %.2f V", mag + 1, SLEW_ITER_MAX, Vcmd_target);

        sprintf(fname, "Vcmd_%.2f_data.out", Vcmd_target);
        DAQ_Pause();

        // Pass the target voltage through the freq parameter slot.
        Run_Dynamic(Vcmd_target, FINAL_TIME, "Slewrate", fname, Slew_step);
    }
}
