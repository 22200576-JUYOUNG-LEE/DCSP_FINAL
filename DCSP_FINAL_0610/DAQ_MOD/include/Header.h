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

#define   SAMPLING_FREQ   (double)(            200 ) // [Hz]
#define   SAMPLING_TIME   (double)( 1.0/SAMPLING_FREQ )
#define   UNIT_PI         (double)( 3.14159265358979  )

#define   N_NAME_BUFFER   (int)   (256)
#define   N_MAX_BUFFER    (int)   (100000)

#define   DEVICE_NUM      (int)    (3)
#define   NUM_A0_CHANNELS (int)    (2)
#define   NUM_AI_CHANNELS (int)    (4) 

#define   DAQ_V_STOP       (double) (0.0)
#define   DAQ_V_START      (double) (3.0)
#define   DAQ_V_STANDARD   (double) (2.5)

#define   SCALE_RAD2DEG   (double) (180.0 / UNIT_PI)
#define   V_POTEN         (double) (5.0)

#define   CAL_AVG(num_data, Ybar_pre, Y) \
    (((1.0 - 1.0 / (num_data)) * (Ybar_pre)) + ((1.0 / (num_data)) * (Y)))
#define   V_POTEN2RAD(Vpoten) \
    (Vpoten * (2.0 * UNIT_PI) / V_POTEN - UNIT_PI)
#define   V_GYRO2RAD     (double) (1000.0 * UNIT_PI) / (0.67 * 180.0)


// RUNNING DEFINE
#define   RUN_MODE_NONLINEAR    (int) (0)
#define   RUN_MODE_LINEAR       (int) (1)
#define   RUN_MODE_USER_INPUT   (int) (2)

#define   CHECK_SIN_AMPLITUDE   (double)(2.5)
#define   CHECK_OFFSET          (double)(2.5)
#define   CHECK_DIR             (const char*)("Check_DAQ")
#define   CHECK_FILE            (const char*)("Checking_data.out")
#define   CHECK_TIME_FINAL      (double)(5.0)

#define   OFFSET_TIME           (double) (              5.0)
#define   OFFSET_DIR            (const char*)(     "Offset")
#define   OFFSET_FILE           (const char*)( "Offset.out")
      
#define   STATIC_ITER_MAX       (int)(10)
#define   STATIC_NEGATIVE_START_POINT   (double)(2.26)
#define   STATIC_POSITIVE_START_POINT   (double)(2.74)
#define   STATIC_CLOSE_NEGATIVE_POINT   (double)(2.16)
#define   STATIC_CLOSE_POSITIVE_POINT   (double)(2.84)
#define   STATIC_WIDE_STEP      (double)(0.1)
#define   STATIC_CLOSE_STEP     (double)(0.02)
#define   STATIC_N_SWEEP        (int)       ( 50)
#define   STATIC_TIME_PAUSE     (double)    (0.5)
#define   STATIC_TIME_FINAL     (double)    (4.0)
#define   STATIC_DIR            (const char*)(     "Static")   
#define   STATIC_DIR_LIN        (const char*)("Static_Linear")   
#define   STATIC_FILE           (const char*)( "Static.out")
#define   STATIC_DATA_DIR       (const char*)("Static_data")
#define   STATIC_SIGN_POSITIVE  (int)(               0)
#define   STATIC_SIGN_NEGATIVE  (int)(               1)
#define   STATIC_GET_VSTEP(val) \
    (((fabs(val) > STATIC_CLOSE_NEGATIVE_POINT) && \
      (fabs(val) < STATIC_CLOSE_POSITIVE_POINT)) \
     ? STATIC_CLOSE_STEP : STATIC_WIDE_STEP)

#define   LIN_MAX_RATE          (double) (1300.0)
#define   LIN_V_DEADZONE        (double) (20.0)
#define   LIN_ITER_MAX          (int) (1)

#define   VALID_DIR             (const char*)("Validation")
#define   VALID_ITER_MAX        (int) (1)

#define   VALID_TRI_AMPLITUDE   (double)(24.0)
#define   VALID_TRI_TIME_PAUSE  (double) (2.0)
#define   VALID_TRI_TIME_PERIOD (double)(30.0)
#define   VALID_TRI_SLOPE       (double)(VALID_TRI_AMPLITUDE/ VALID_TRI_TIME_PERIOD)
#define   VALID_TRI_TIME_POINT1 (double)(              1.0)
#define   VALID_TRI_TIME_POINT2 (double)(VALID_TRI_TIME_POINT1+VALID_TRI_TIME_PERIOD)
#define   VALID_TRI_TIME_POINT3 (double)(VALID_TRI_TIME_POINT2+ 2.0* VALID_TRI_TIME_PERIOD)
#define   VALID_TRI_TIME_POINT4 (double)(VALID_TRI_TIME_POINT3+ 2.0* VALID_TRI_TIME_PERIOD)
#define   VALID_TRI_TIME_POINT5 (double)(VALID_TRI_TIME_POINT4+ 2.0* VALID_TRI_TIME_PERIOD)
#define   VALID_TRI_TIME_FINAL  (double)(VALID_TRI_TIME_POINT5 + VALID_TRI_TIME_PERIOD + 1)

#define   VALID_SIN_TIME_PAUSE  (double)(1.0)
#define   VALID_SIN_AMPLITUDE   (double)(25.0)
#define   VALID_SIN_TIME_FINAL  (double)(VALID_SIN_TIME_2 +1)
#define   VALID_SIN_TIME_1      (double)(1.0)
#define   VALID_SIN_FREQ        (double)(0.01)
#define   VALID_SIN_TIME_2      (double)(VALID_SIN_TIME_1+ 3 / VALID_SIN_FREQ)

#define   BODE_ITER_MAX         (int) (20)
#define   BODE_DIR              (const char*)("BodeMag")
#define   BODE_TIME_PAUSE       (double)(2.0)
#define   BODE_INIT_FREQ        (double)(0.1)
#define   BODE_SIN_FREQ_STEP    (double)(0.02)
#define   BODE_SIN_TIME_FINAL   (double)(10.0)
#define   BODE_SIN_AMPLITUDE    (double)(8.0)

#define   FORMAT_ANGLE_ZERO     (double)(0.0)
#define   FORMAT_FINAL_TIME     (double)(2.0)
#define   FORMAT_FUNC_K         (double)(1.0)

#define   POTEN_INIT	        (double)(-30.0)
#define   POTEN_STEP	        (double)(3.0)
#define   POTEN_FINAL_TIME      (double)(3.0)
#define   POTEN_ITER_MAX        (int)   (20)
#define	  POTEN_DEG2V_POTEN	    (double)(5.0/ 360.0)
#define   POTEN_INIT_V          (double)(2.5)
#define   POTEN_FILE            (const char*)("Poten.out")
#define   POTEN_DATA_DIR        (const char*)("Poten_data") 
#define   POTEN_SUMMARY_FILE    (const char*)("Poten_summary.out")
#define   POTEN_EPS             (double)(1.0)

#define   DESIGNATION_TIME_FINAL    (double)(1.0)
#define   DESIGNATION_TIME_PAUSE    (double)(1.0)

#define   STABILIZATION_DIR         (const char*)("Stabilization")
#define   STABILIZATION_FILE        (const char*)("Stabil_Dynamics.out")
#define   STABILIZATION_CONSTANT_P  (double)(2.4141)
#define   STABILIZATION_CONSTANT_I  (double)(62.6297)
#define   STABILIZATION_FINAL_TIME  (double)(30.0)
#define   STABILIZATION_GYRO_OFFSET (double)(1.47)

typedef enum {
    MODE_STATIC         = 1,
    MODE_VALIDATION     = 2, 
    MODE_BODE           = 3,
    MODE_FORMAT         = 4,
    MODE_DESIGNATION    = 5,
    MODE_STABIL         = 6,



    N_MODE              = 6,
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
    double Time;        // [sec]
    double Command;     // [rad/s]
    double Wgyro;       // [rad/s]
    double Angle;       // [rad]
} DynState;

typedef double (*DynFn)(DynState s);

typedef struct {
    double Command;
    double Vgyro;
    double Wgyro;
    double Angle;
} DAQ_Averages;

extern DAQ_Averages Data_avg;


// 1. MODE_DAQ_CHECK.c
void Check_DAQ(void);

// myMode.c
Mode SelectOperatingMode(void);
void RunMode(void);

// MotorDynamic.c
void MotorDynamic(double Final_time, const char* OutDirName, const char* OutFileName, DynFn fn);

// 2. myDAQ.c
double  GetWindowTime(void);
void    DAQ_Init();
void    DAQ_Reset(); 
void    DAQ_Write(double Vcmd);
void    DAQ_Read(double* Vin);
void    DAQ_Pause(double time_delay);
void    DAQ_Close();
void    SaveDataset(const char* DirName, const char* OutFileName, const Dataset* Out_Dataset, const int num_col, const int num_row);

// 4. STATIC.c
void    IterStaticProperty();
void    StaticProperty();
void    InitPath(void);


// 5. myValidation.c
void StaticValidation();

double Linearization(double vin);
void ReadLinearCoefficent(void);

// 6. myBode.c
void BodeMag();

//7. Potien.c
void Potentio();
void Format(double Angle);


//8. Designaiton.c
void   Designation(void);
double Designation_Control(DynState s);

//9. MODE_STABIL.C
void Stabilization(void);
void Stabilization_Control(DynState s);

#endif