#include "myHeader.h"

#define ITER_MAX  23

// ── Control Signal Functions (DynFn signature) ────────────────
// freq is unused in both; it is present only to match the unified DynFn type.

static double Vali_triangle(DynState s)
{
    double slope = 0.7 / 5.0;

    if      (s.t <  1.0) return 0.0;
    else if (s.t <  6.0) return s.Vcmd + slope * SAMPLING_TIME;
    else if (s.t < 16.0) return s.Vcmd - slope * SAMPLING_TIME;
    else if (s.t < 26.0) return s.Vcmd + slope * SAMPLING_TIME;
    else                 return 0.0;
}

static double Vali_sin(DynState s)
{
    if      (s.t <  1.0) return 0.0;
    else if (s.t < 20.0) return 0.7 * sin(2.0 * UNIT_PI * (s.t - 1.0) / 15.0);
    else               return 0.0;
}

// ── Validation Run ────────────────────────────────────────────
void Validation(void)
{
    char fname[100];

    printf("\nValidation - Triangle\n");
    for (int i = 0; i < ITER_MAX; i++) {
        sprintf(fname, "Vali_tri_iter%d.out", i + 1);
        DAQ_Pause(); DAQ_Pause();
        Run_Dynamic(0.0, 26.0, "Validation", fname, Vali_triangle);
    }

    printf("\nValidation - Sin\n");
    for (int i = 0; i < ITER_MAX; i++) {
        sprintf(fname, "Vali_sin_iter%d.out", i + 1);
        DAQ_Pause();
        Run_Dynamic(0.0, 22.0, "Validation", fname, Vali_sin);
    }
}

// ── Linearization Mapping ─────────────────────────────────────
double Vcmd2Vc(double vin, double vdz)
{
    double a  = -313.5497, b  =  3016.6342, c  = -5849.5654;
    double a1 =  373.0041, b1 =  -329.4004, c1 = -1203.8487;
    double Vop1 = 90.6066;

    double w_ref = (Vop1 / vdz) * vin;
    double disc, vcmd = VCMD_OFFSET;

    if (vin > vdz) {
        disc = b * b - 4.0 * a * (c - w_ref);
        if (disc >= 0.0) vcmd = (-b + sqrt(disc)) / (2.0 * a);
    } else if (vin < -vdz) {
        disc = b1 * b1 - 4.0 * a1 * (c1 - w_ref);
        if (disc >= 0.0) vcmd = (-b1 + sqrt(disc)) / (2.0 * a1);
    }

    return vcmd;
}
