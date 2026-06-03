#include "Header.h"




/*Projected by Goodenough(05.30.26)*/
#define AVG_SAMPLES     100                          // 0.5 sec at 200 Hz
#define N_STEPS         41                           // -60 to +60, 3 deg steps

static double target_deg[N_STEPS];
static double saved_V[N_STEPS];
static int    saved_count = 0;

static double s_Vc = DAQ_V_STANDARD;
static int    averaging = 0;
static double avg_accum = 0.0;
static int    avg_count = 0;

static void BuildTargetList(void) {
    for (int i = 0; i < N_STEPS; i++)
        target_deg[i] = -60.0 + i * 3.0;
}

double format(DynState s) {



    double potentio_curr_volt = s.Vpoten;

    double volt_err = (potentio_curr_volt - 2.5);


    if (volt_err >= 0) return -volt_err;
    else if (volt_err < 0) return volt_err;

}