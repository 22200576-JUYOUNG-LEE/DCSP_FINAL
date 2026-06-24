#include "Header.h"

double i_out_prev = 0.0;
double i_err_prev = 0.0;

double I_controller(double error, double gain);
double P_controller(double error, double gain);

double I_controller(double error, double gain) {


    // update present out: 
    double out = (gain * SAMPLING_TIME) / 2.0 * (error + i_err_prev) + i_out_prev;

    // update previous error & output: 

    i_err_prev = error;
    i_out_prev = out;

    return out;
}

double P_controller(double error, double gain) {

    return gain * error;
}


void Stabilization(void) {
    printf("\n================================\n");
    printf("\Stabilization LOOP (PI Control + LIN)");
    printf("\n================================\n");

    i_err_prev = 0.0;
    i_out_prev = 0.0;

    Format(FORMAT_ANGLE_ZERO);

    DAQ_Pause(DESIGNATION_TIME_PAUSE);

    RUN_DAQ_mode    = RUN_MODE_LINEAR;

    target_rate_rad = 5.0; 

    MotorDynamic(STABILIZATION_FINAL_TIME, STABILIZATION_DIR, STABILIZATION_FILE, Stabilization_Control);
}
        
void Stabilization_Control(DynState s) {
    error               = 0.0;

    current_rate_rad    = 0.0;
    current_rad         = 0.0;

    Command             = 0.0;

    current_rate_rad    = s.Wgyro;
    error               = target_rate_rad - current_rate_rad;

    Command             = P_controller(error, STABILIZATION_CONSTANT_P) + I_controller(error, STABILIZATION_CONSTANT_I);

    return  Command;
}




