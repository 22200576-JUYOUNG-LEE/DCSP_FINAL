#ifndef _MY_DAQ_H
#define _MY_DAQ_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <conio.h>
#include <direct.h>
#include "NIDAQmx.h"

// ── Time / Sampling ───────────────────────────────────────────
#define FINAL_TIME          3.0
#define SAMPLING_FREQ       200.0
#define SAMPLING_TIME       (1.0 / SAMPLING_FREQ)
#define N_STEP              (int)(FINAL_TIME * SAMPLING_FREQ)
#define DYNA_N_STEP_MAX     10000
#define PAUSE_TIME          1.0

// ── Math Units ────────────────────────────────────────────────
#define UNIT_PI             3.14159265358979
#define RAD2DEG             (180.0 / UNIT_PI)
#define REV                 360.0

// ── Hardware ──────────────────────────────────────────────────
#define DEVICE_NUMBER       6
#define NUM_AO_CHANNELS     2
#define NUM_AI_CHANNELS     4

#define MOTOR_OFF_VSW       0.0
#define MOTOR_ON_VSW        3.0
#define MOTOR_OFF_VCMD      2.5
#define VCMD_OFFSET         2.5
#define VPOTEN_MAX          5.0
#define VDZ                 0.05
#define GYRO_V2RAD          ((1000.0 * UNIT_PI) / (0.67 * 180.0))

static inline double Cal_avg(int n, double prev_avg, double new_sample)
{
    return (1.0 - 1.0 / n) * prev_avg + (1.0 / n) * new_sample;
}

// ── Types ─────────────────────────────────────────────────────
typedef struct {
    const char*   name;
    const double* data;
} Dataset;

// Unified dynamic function signature.
// freq  : sinusoidal frequency [Hz] — pass 0.0 for non-Bode modes.
// Vcmd  : previous command voltage (carry-over state for ramp signals).
// t     : elapsed time [s].
// Wgyro : angular velocity from gyro [rad/s].
// Vpoten: potentiometer voltage [V].
// Returns the new Vcmd to send this step.
typedef double (*DynFn)(double freq, double Vcmd, double t, double Wgyro, double Vpoten);

typedef struct {
    double freq;
    double Vcmd;
    double t;
    double Wgyro;
    double Vpoten;

} DynState;

typedef double (*DynFn)(DynState s);


typedef enum {
    MODE_STATIC         = 1,
    MODE_VALIDATION     = 2,
    MODE_SLEWRATE       = 3,
    MODE_BODE           = 4,
    MODE_STATIC_LINEAR  = 5,
    MODE_COUNT          = 5,
} Mode;

// ── Globals ───────────────────────────────────────────────────
extern TaskHandle taskAI;
extern TaskHandle taskAO;
extern double     Vgyro_offset;

// ── Function Declarations ─────────────────────────────────────

// myWin.c
double GetWindowTime(void);
void   SaveDataset(const char* dir, const char* file,
                   const Dataset* ds, int ncol, int nrow);

// myDAQ.c
void DAQ_Init(int dev_num);
void DAQ_Reset(void);
void DAQ_Write(double Vcmd);
void DAQ_Read(double* Vin);
void DAQ_Pause(void);
void DAQ_Close(void);

// myMotor.c
double Find_Vgyro_offset(void);
double Find_Wgyro_avg(const char* dir, double Vc);
void   ObtainMotorStaticProperty(void);
void   ObtainMotorStaticProperty_Linear(void);

// myValidation.c
void   Validation(void);
double Vcmd2Vc(double vin, double vdz);

// mySlewrate.c
void Slewrate(void);

// myBode.c
void BodeMag(void);

// myMode.c
Mode SelectOperatingMode(void);
void RunMode(Mode mode);
void Run_Dynamic(double freq, double T,
                 const char* dir, const char* file, DynFn fn);

#endif
