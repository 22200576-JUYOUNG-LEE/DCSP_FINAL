#include "Header.h"

double      target_angle_rad    = 0.0;
double      target_rate_rad     = 0.0;

double      current_rad         = 0.0;
double      current_rate_rad    = 0.0;

double      error               = 0.0;
double      d_error             = 0.0;

double      Command             = 0.0;

void Designation(double target_angle) {
    printf("\n================================\n");
    printf("\tDESIGNATION LOOP (PD Control + LIN)");
    printf("\n================================\n");

    
    Format(FORMAT_ANGLE_ZERO);
    DAQ_Pause(DESIGNATION_TIME_PAUSE);
    
    RUN_DAQ_mode        = RUN_MODE_LINEAR;

    target_angle_rad    = target_angle * SCALE_DEG2RAD;

    MotorDynamic(DESIGNATION_TIME_FINAL, "Designation", "Designation_Step.out", Designation_Control);
}

double Designation_Control(DynState s) {
    current_rad         = s.Angle;
    current_rate_rad    = s.Wgyro;

    error               = target_angle_rad - current_rad;
    d_error             = 0.0 - current_rate_rad;

    Command = (DESIGNATION_K_P * error) + (DESIGNATION_K_D * d_error);

    return Command;
}

