#include "Header.h"

void StaticValidation() {

    InitPath();

    RUN_DAQ_mode = LINEAR;

    printf("[Motor Validation] ...\n");
    for (int iter = 0; iter < VALID_ITER_MAX; iter++) {

        sprintf(OutFileName, "Valid_tri_iter%d.out", iter + 1);

        DAQ_Pause(VALID_TRI_TIME_PAUSE);

        RunDAQ(VALID_TRI_TIME_FINAL, VALID_DIR, OutFileName, Valid_triangle);
    }

    for (int iter = 0; iter < VALID_ITER_MAX; iter++) {

        sprintf(OutFileName, "Valid_sin_iter%d.out", iter + 1);

        DAQ_Pause(VALID_TRI_TIME_PAUSE);

        RunDAQ(VALID_SIN_TIME_FINAL, VALID_DIR, OutFileName, Valid_sin);
    }
}

double Valid_triangle(DynState s)
{
    if (s.Time < VALID_TRI_TIME_POINT1) return 0.0;
    else if (s.Time < VALID_TRI_TIME_POINT2) return s.Vcmd + VALID_TRI_SLOPE * SAMPLING_TIME;
    else if (s.Time < VALID_TRI_TIME_POINT3) return s.Vcmd - VALID_TRI_SLOPE * SAMPLING_TIME;
    else if (s.Time < VALID_TRI_TIME_POINT4) return s.Vcmd + VALID_TRI_SLOPE * SAMPLING_TIME;
    else if (s.Time < VALID_TRI_TIME_POINT5) return s.Vcmd - VALID_TRI_SLOPE * SAMPLING_TIME;
    else if (s.Time < VALID_TRI_TIME_FINAL)  return s.Vcmd + VALID_TRI_SLOPE * SAMPLING_TIME;

    else                                     return 0.0;
}

double Valid_sin(DynState s)
{
    if (s.Time < VALID_SIN_TIME_1)          return 0.0;
    else if (s.Time < VALID_SIN_TIME_2)     return VALID_SIN_AMPLITUDE * sin(2.0 * UNIT_PI * VALID_SIN_FREQ* (s.Time - VALID_SIN_TIME_1));
    else                    return 0.0;
}