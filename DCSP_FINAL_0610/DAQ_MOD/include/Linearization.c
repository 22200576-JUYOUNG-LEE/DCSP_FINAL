#include "Header.h"

double a1 = 0.0, b1 = 0.0, c1 = 0.0, d1 = 0.0, e1 = 0.0;
double a2 = 0.0, b2 = 0.0, c2 = 0.0, d2 = 0.0, e2 = 0.0;

double Linearization(double rate_cmd_rad_s) {
    double motor_input_v = 0.0;
    double rate_cmd_deg_s = rate_cmd_rad_s * SCALE_RAD2DEG;

    // 1. Saturation implementation 
    if (rate_cmd_deg_s > LIN_MAX_RATE) rate_cmd_deg_s = LIN_MAX_RATE;
    else if (rate_cmd_deg_s < -LIN_MAX_RATE) rate_cmd_deg_s = -LIN_MAX_RATE;

    // 2. No need of LIN_V_DZ
    if (rate_cmd_deg_s >= LIN_V_DEADZONE) {
        // 4th order: CW 
        motor_input_v = a1 * pow(rate_cmd_deg_s, 4) + b1 * pow(rate_cmd_deg_s, 3) + c1 * pow(rate_cmd_deg_s, 2) + d1 * rate_cmd_deg_s + e1;
    }
    else if (rate_cmd_deg_s <= -LIN_V_DEADZONE) {
        // 4th order: CCW
        motor_input_v = a2 * pow(rate_cmd_deg_s, 4) + b2 * pow(rate_cmd_deg_s, 3) + c2 * pow(rate_cmd_deg_s, 2) + d2 * rate_cmd_deg_s + e2;
    }
    else {
        motor_input_v = DAQ_V_STANDARD;
    }

    return motor_input_v;
}

void ReadLinearCoefficent(void) {
    FILE* fp = fopen("../data/Linearization_coef.csv", "r");
    char buffer[256];

    if (fp == NULL) {
        printf("ERROR: [IN Linearization] COULD NOT OPEN FILE!\n");
        system("pause");
        exit(1);
    }

    // 2nd Row obtained: 5 cols
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        sscanf(buffer, "%lf,%lf,%lf,%lf,%lf", &a1, &b1, &c1, &d1, &e1);
    }

    // 3rd Row obtained: 5 cols
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        sscanf(buffer, "%lf,%lf,%lf,%lf,%lf", &a2, &b2, &c2, &d2, &e2);
    }
    fclose(fp);
}