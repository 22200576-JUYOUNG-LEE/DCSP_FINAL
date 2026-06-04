#include "Header.h"

void Designation(void) {
    printf("\n================================\n");
    printf("\tDESIGNATION LOOP (PD Control + LIN)");
    printf("\n================================\n");

    Format();
    DAQ_Pause(1.0);
    
    RUN_DAQ_mode = LINEAR;

    // 5sec 
    RunDAQ(1.0, "Designation_data", "Designation_Step.out", Designation_Control);
}

double Designation_Control(DynState s) {
    // 1. Modify here
    double Kp = 0.066389;
    double Kd = 0.002361;

    double target_deg = 80.0;

    // 2. sensor data [deg], [deg/s]
    double current_deg = VOLT2DEG(s.Vpoten);
    double current_rate_deg = s.Wgyro * SCALE_RAD2DEG;

    // 3. error calculation ()
    double error = target_deg - current_deg;
    double d_error = 0.0 - current_rate_deg;

    // 4. 
    double control_effort = (Kp * error) + (Kd * d_error);

    //// 5. Saturation  
    //double max_velocity = 1400.0 / 1300.0;
    //if (control_effort > max_velocity) control_effort = max_velocity;
    //if (control_effort < -max_velocity) control_effort = -max_velocity;

    // Vcmd return 
    return control_effort;
}

