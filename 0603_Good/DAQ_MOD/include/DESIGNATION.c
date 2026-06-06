#include "Header.h"

void Designation(void) {
    printf("\n================================\n");
    printf("\tDESIGNATION LOOP (PD Control + LIN)");
    printf("\n================================\n");

    Format(FORMAT_ANGLE_ZERO);
    DAQ_Pause(DESIGNATION_TIME_PAUSE);
    
    RUN_DAQ_mode = RUN_MODE_LINEAR;

    RunDAQ(DESIGNATION_TIME_FINAL, "Designation_data", "Designation_Step.out", Designation_Control);
}

double Designation_Control(DynState s) {
    // 1. Modify here
    double Kp = 0.066389;
    double Kd = 0.002361;

    double target_deg = 80.0;

    // 2. sensor data [deg], [deg/s]
    double current_deg = V_POTEN2RAD(s.Vpoten);
    double current_rate_deg = s.Wgyro;

    // 3. error calculation ()
    double error = target_deg - current_deg;
    double d_error = 0.0 - current_rate_deg;

    // 4. 
    double control_effort = (Kp * error) + (Kd * d_error);


    // Vcmd return 
    return control_effort;
}

