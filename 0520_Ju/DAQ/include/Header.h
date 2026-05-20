#ifndef		_MY_DAQ_H	
#define		_MY_DAQ_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <direct.h>
#include "NIDAQmx.h"

#define   N_STEP          (int)   ( FINAL_TIME*SAMPLING_FREQ )
#define   FINAL_TIME      (double)(            4 )  // [Sec]
#define   SAMPLING_FREQ   (double)(            200 ) // [Hz]
#define   SETTLING_TIME   (double)( FINAL_TIME*0.5  ) // [2.5 sec]
#define   SAMPLING_TIME   (double)( 1.0/SAMPLING_FREQ )
#define   UNIT_PI         (double)( 3.14159265358979  )

#define   N_NAME_BUFFER   (int)   (               256)

#define   SCALE_RAD2DEG   (double) (180.0 / UNIT_PI)
#define   SCALE_REV2DEG   (double) (360.0)

#define   GYRO_V2RADS     (double) (1000.0 * UNIT_PI) / (0.67 * 180.0)

#define   DEVICE_NUM      (int)    (                6)
#define   NUM_A0_CHANNELS (int)    (                2)
#define   NUM_AI_CHANNELS (int)    (                4) 

#define   N_MAX_BUFFER    (int)    (            10000)

#define   DAQ_V_STOP       (double) (              0.0)
#define   DAQ_V_START      (double) (              5.0)
#define   DAQ_V_STANDARD   (double) (              2.5)

#define   V_POTEN         (double) (              5.0)


#define CAL_AVG(num_data, Ybar_pre, Y) \
    (((1.0 - 1.0 / (num_data)) * (Ybar_pre)) + ((1.0 / (num_data)) * (Y)))


// RUNNING DEFINE
#define   NON_LINEAR            (int) (0)
#define   LINEAR                (int) (1)

#define   OFFSET_TIME           (double) (              5.0)
#define   OFFSET_DIR            (const char*)(     "Offset")
#define   OFFSET_FILE           (const char*)( "Offset.out")

#define   STATIC_V_STEP         (double) (     0.02)//       <- 
#define   STATIC_N_SWEEP        (int) (  50)
#define   STATIC_TIME_PAUSE     (double) (1.0)
#define   STATIC_TIME_DELAY     (double)(0.5)
#define   STATIC_TIME_FINAL     (double) (              5.0)
#define   STATIC_DIR            (const char*)(     "Static")   
#define   STATIC_DIR_LIN        (const char*)("Static_Linear")   
#define   STATIC_FILE           (const char*)( "Static.out")
#define   STATIC_DATA_DIR       (const char*)("Static_data")   
#define   STATIC_SIGN_POSITIVE  (int)(               0)
#define   STATIC_SIGN_NEGATIVE  (int)(               1)

#define   LIN_VOLT_START        (double)(        2.76)
#define   LIN_GYRO_START        (double)(     85.0817)
#define   LIN_V_DEADZONE        (double) (    0.05)
#define   LIN_COEFFI_POSITIVE_A (double)(-274.4147)
#define   LIN_COEFFI_POSITIVE_B (double)(2725.1332)
#define   LIN_COEFFI_POSITIVE_C (double)(-5352.2284)
#define   LIN_COEFFI_NEGATIVE_A (double)(296.2148)
#define   LIN_COEFFI_NEGATIVE_B (double)(-78.8436)
#define   LIN_COEFFI_NEGATIVE_C (double)(-1383.6486)

#define   VALID_DIR             (const char*)("Validation")
#define   VALID_ITER_MAX        (int) (2)

#define   VALID_TRI_TIME_PAUSE  (double) (2.0)
#define   VALID_TRI_TIME_PERIOD (double)(              5.0)
#define   VALID_TRI_SLOPE       (double)(0.7 / VALID_TRI_TIME_PERIOD)
#define   VALID_TRI_TIME_POINT1 (double)(              1.0)
#define   VALID_TRI_TIME_POINT2 (double)(VALID_TRI_TIME_POINT1+VALID_TRI_TIME_PERIOD)
#define   VALID_TRI_TIME_POINT3 (double)(VALID_TRI_TIME_POINT2+ 2.0* VALID_TRI_TIME_PERIOD)
#define   VALID_TRI_TIME_FINAL  (double)(             26.0)

#define   VALID_SIN_TIME_PAUSE  (double)(1.0)
#define   VALID_SIN_AMPLITUDE   (double)(0.7)
#define   VALID_SIN_TIME_FINAL  (double)(             22.0)
#define   VALID_SIN_TIME_1      (double)(1.0)
#define   VALID_SIN_FREQ        (double)(0.07)
#define   VALID_SIN_TIME_2      (double)(VALID_SIN_TIME_1+ 1 / VALID_SIN_FREQ)

#define   BODE_DIR              (const char*)("BodeMag")
#define   BODE_TIME_PAUSE       (double)(2.0)
#define   BODE_ITER_START       (int) (0)
#define   BODE_ITER_MAX         (int) (30)

#define   BODE_SIN_FREQ_STEP    (double)(0.2)
#define   BODE_SIN_TIME_FINAL   (double)(6.0)
#define   BODE_SIN_TIME_1       (double)(1.0)
#define   BODE_SIN_TIME_2       (double)(5.0)
#define   BODE_SIN_AMPLITUDE    (double)(0.7)


typedef enum {
    MODE_STATIC         = 1,
    MODE_STATIC_LINEAR  = 2,
    MODE_VALIDATION     = 3, 
    MODE_BODE           = 4,

    N_MODE              = 5,
} Mode;

extern TaskHandle   taskAI;
extern TaskHandle   taskAO;

extern int          RUN_DAQ_mode;

extern double       Vgyro_offset;

extern char         OutDirName[N_NAME_BUFFER];
extern char         OutFileName[N_NAME_BUFFER];

typedef struct {
    const char* name;
    const double* data;
} Dataset;

typedef struct {
    double Time;
    double Vcmd;
    double Wgyro;
    double Vpoten;
} DynState;

typedef double (*DynFn)(DynState s);

typedef struct {
    double Vcmd;
    double Vgyro;
    double Wgyro;
    double Angle;
} DAQ_Averages;

extern DAQ_Averages g_daqAvg;


// 1. myWin.c
double GetWindowTime(void);
void SaveDataset(const char* DirName, const char* OutFileName, const Dataset* Out_Dataset, const int num_col, const int num_row);

// myMode.c
Mode SelectOperatingMode(void);
void RunMode(void);
void RunDAQ(double Final_time, const char* OutDirName, const char* OutFileName, DynFn fn);
void InitAvg(void);

// 2. myDAQ.c
void DAQ_Init();
void DAQ_Reset(); 
void DAQ_Write(double Vcmd);
void DAQ_Read(double* Vin);
void DAQ_Pause(double time_delay);
void DAQ_Close();


// 4. myMotor.c
void StaticProperty(int Static_mode);
void InitPath(void);


// 5. myValidation.c
void StaticValidation();
double Valid_triangle(DynState s);
double Valid_sin(DynState s);
double Linearization(double vin);

// 6. myBode.c
void BodeMag();


#endif