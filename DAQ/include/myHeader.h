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
#define   FINAL_TIME      (double)(            3 )  // [Sec]
#define   SAMPLING_FREQ   (double)(            200 ) // [Hz]
#define   SETTLING_TIME   (double)( FINAL_TIME*0.5  ) // [2.5 sec]
#define   SAMPLING_TIME   (double)( 1.0/SAMPLING_FREQ )
#define   UNIT_PI         (double)( 3.14159265358979  )
#define   RAD2DEG         (double) 180 / UNIT_PI
#define   REV             (double) 360

#define   DEV_NUM         (int)    6
#define   NUM_A0_CHANNELS (int)    (2               )
#define   NUM_AI_CHANNELS (int)    (4               ) 

#define   DYNA_N_STEP_MAX (int)   (          10000)

#define   PAUSE_TIME      (double) 1.0

#define   MOTOR_OFF_Vsw   (double) 0.0
#define   MOTOR_ON_Vsw    (double) 3.0
#define   MOTOR_OFF_Vcmd  (double) 2.5

#define   Vcmd_offset     (double) 2.5
#define   Vpoten_MAX      (double) 5
#define   Vdz             (double) 0.05

#define   Kg              (double) (1000 * UNIT_PI) / (0.67 * 180) //GYRO_V2RADS


typedef struct {
    const char* name;
    const double* data;
} Dataset;

// main.c
int SelectOperatingMode();
void Dynamic_function(TaskHandle taskAI, TaskHandle taskAO, double Vgyro_offset, double Final_time, 
    const char* OutDirName, const char* OutFileName, double (*Processing)(double, double, double, double));

// 1. myWin.c
double GetWindowTime(void);
void SaveDataset(const char* DirName, const char* OutFileName, const Dataset* Out_Dataset, const int num_col, const int num_row);

// 2. myDAQ.c
void InitDAQ(TaskHandle* taskAI, TaskHandle* taskAO, int devNum);// TaskHandle* taskAI -> Global // alt key
void ResetDAQ(TaskHandle taskAO); // 글자수 동일하게 만들기, DAQ앞으로 빼서 정의하기
void WriteDAQ(TaskHandle taskAO, double Vcmd);// Argument는 만들지 마라, 두 채널을 쓴다면, 두 채널을 
void ReadDAQ(TaskHandle taskAI, double* Vin);
void PauseDAQ(TaskHandle taskAO);
void CloseDAQ(TaskHandle taskAI, TaskHandle taskAO);

// 3. myMath.c
double Cal_avg(int num_data, double Ybar_pre, double Y);

// 4. myMotor.c
double Find_Vgyro_offset(TaskHandle taskAI, TaskHandle taskAO);
double Find_Wgyro_avg(TaskHandle taskAI, TaskHandle taskAO, char* OutDirName, double Vc, double Vgyro_offset);
void   ObtainMotorStaticProperty(TaskHandle taskAI, TaskHandle taskAO, double Vgyro_offset);

// 5. myValidation.c
void Validation(TaskHandle taskAI, TaskHandle taskAO, double Vgyro_offset);
double Vali_triangle(double Vcmd, double time, double Wgyro, double Vpoten);
//Vali -> Valid 
// 인배디드는 void로 선언 
double Vali_sin(double Vcmd, double time, double Wgyro, double Vpoten);
double Linear_func1(double Vcmd);
double Linear_func2(double vin, double vdz);

//6. mySlewratelimit.c
void Slewrate(TaskHandle taskAI, TaskHandle taskAO, double Vgyro_offset);
double modi_Find_Wgyro_avg(TaskHandle taskAI, TaskHandle taskAO, double Vcmd, char* OutDirName, double Vgyro_offset);

// 7. myBode.c
void BodeMag(TaskHandle taskAI, TaskHandle taskAO, double Vgyro_offset);
void Bode_Dynamic_function(TaskHandle taskAI, TaskHandle taskAO, double Vgyro_offset, double freq, double Final_time,
    const char* OutDirName, const char* OutFileName, double (*Processing)(double, double, double, double, double));
double Bode_sin(double freq, double Vcmd, double time, double Wgyro, double Vpoten);


#endif