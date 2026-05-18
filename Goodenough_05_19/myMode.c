#include "myHeader.h"

// ── Mode Labels ───────────────────────────────────────────────
static const char* MODE_LABELS[MODE_COUNT] = {
    "Obtain Motor Static Property",
    "Motor Validation",
    "Slewrate Limit",
    "Bode Mag",
    "Linearized Mapping",
};

// ── Mode Selection ────────────────────────────────────────────
Mode SelectOperatingMode(void)
{
    int answer = 0;

    printf("\n[Select Mode] ================\n");
    for (int i = 0; i < MODE_COUNT; i++)
        printf("%d: %s\n", i + 1, MODE_LABELS[i]);
    printf("==============================\n");
    printf("Input: ");
    scanf_s("%d", &answer);
    printf("\n");

    return (Mode)answer;
}

// ── Mode Dispatch ─────────────────────────────────────────────
void RunMode(Mode mode)
{
    switch (mode) {
        case MODE_STATIC:        ObtainMotorStaticProperty();        break;
        case MODE_VALIDATION:    Validation();                       break;
        case MODE_SLEWRATE:      Slewrate();                         break;
        case MODE_BODE:          BodeMag();                          break;
        case MODE_STATIC_LINEAR: ObtainMotorStaticProperty_Linear(); break;
        default: printf("[ERROR] Unknown mode: %d\n", mode);         break;
    }
}

// ── Unified Dynamic Loop ──────────────────────────────────────
// All measurement modes share the same timed acquire-write-read loop.
// Callers supply a DynFn that encodes the signal shape for that mode.
//   freq  : sinusoidal frequency [Hz]; pass 0.0 for non-Bode modes.
//   T     : total run time [s].
//   dir   : output subdirectory name (relative to ../data/).
//   file  : output filename.
//   fn    : signal/control function — see DynFn typedef in myHeader.h.
void Run_Dynamic(double freq, double T,
                 const char* dir, const char* file, DynFn fn)
{
    float64 Vin[NUM_AI_CHANNELS] = { 0.0 };
    int     idx_max = (int)(T * SAMPLING_FREQ);

    double time_init, time_curr, time = 0.0;
    double Vcmd = 0.0, Vc = 0.0;
    double Vgyro = 0.0, Wgyro = 0.0, Vpoten = 0.0;

    static double Out_Time [DYNA_N_STEP_MAX];
    static double Out_Vcmd [DYNA_N_STEP_MAX];
    static double Out_Vgyro[DYNA_N_STEP_MAX];
    static double Out_Theta[DYNA_N_STEP_MAX];
    static double Out_Wgyro[DYNA_N_STEP_MAX];

    DynState s = { freq, Vcmd, time, Wgyro, Vpoten };

    Dataset ds[] = {
        { "Time[sec]",               Out_Time  },
        { "V_{cmd}[V]",              Out_Vcmd  },
        { "V_{gyro}[V]",             Out_Vgyro },
        { "\\theta[deg]",            Out_Theta },
        { "\\omega_{gyro}[deg/sec]", Out_Wgyro },
    };

    time_init = time_curr = GetWindowTime();

    for (int k = 0; k < idx_max; k++) {
        time  = (time_curr - time_init) * 0.001;
        Vcmd  = fn(s);
        Vc    = Linear_func2(Vcmd, VDZ);

        DAQ_Write(Vc);
        DAQ_Read(Vin);

        Vgyro  = Vin[2];
        Wgyro  = GYRO_V2RAD * (Vgyro - Vgyro_offset);
        Vpoten = Vin[3];

        Out_Time [k] = time;
        Out_Vcmd [k] = Vcmd;
        Out_Vgyro[k] = Vgyro;
        Out_Theta[k] = Vpoten * REV / VPOTEN_MAX;
        Out_Wgyro[k] = Wgyro * RAD2DEG;

        if (_kbhit() && _getch() == 's') { DAQ_Close(); exit(0); }

        while (1) {
            time_curr = GetWindowTime();
            if (time_curr - time_init - k * SAMPLING_TIME * 1000.0
                    >= SAMPLING_TIME * 1000.0) break;
        }
    }

    SaveDataset(dir, file, ds, sizeof(ds) / sizeof(ds[0]), idx_max);
}
