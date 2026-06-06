#include "Header.h"

double OperPoint_Vc = 0.0, OperPoint_Wgyro = 0.0;
double a1 = 0.0, a2 = 0.0;
double b1 = 0.0, b2 = 0.0;
double c1 = 0.0, c2 = 0.0;
double d1 = 0.0, d2 = 0.0;
double e1 = 0.0, e2 = 0.0;


double a_deadZone = 0.0;
double b_deadZone = 0.0;
double c_deadZone = 0.0;
double d_deadZone = 0.0; 


double Linearization(double vin) {
    double Vc = 0.0;
    double Vcmd = vin;

    // 1. Saturation implementation 
    if (Vcmd > LIN_MAX_RATE) Vcmd = LIN_MAX_RATE;
    else if (Vcmd < -LIN_MAX_RATE) Vcmd = -LIN_MAX_RATE;

    // 2. No need of LIN_V_DZ
    if (Vcmd >= LIN_V_DEADZONE) {
        // 4th order: CW 
        Vc = a1 * pow(Vcmd, 4) + b1 * pow(Vcmd, 3) + c1 * pow(Vcmd, 2) + d1 * Vcmd + e1;
    }
    else if (Vcmd <= -LIN_V_DEADZONE) {
        // 4th order: CCW
        Vc = a2 * pow(Vcmd, 4) + b2 * pow(Vcmd, 3) + c2 * pow(Vcmd, 2) + d2 * Vcmd + e2;
    }
    else {
        Vc = DAQ_V_STANDARD;
    }

    return Vc;
}

void ReadLinearCoefficent(void) {
    FILE* fp = fopen("../data/LINEAR_COEF.csv", "r");
    char buffer[256];

    if (fp == NULL) {
        printf("ERROR: [IN Linearization] COULD NOT OPEN FILE!\n");
        system("pause");
        exit(1);
    }

    // 1st Row obtained: Operation points
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        sscanf(buffer, "%lf,%lf", &OperPoint_Wgyro, &OperPoint_Vc);
    }

    // 2nd Row obtained: 5 cols
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        sscanf(buffer, "%lf,%lf,%lf,%lf,%lf", &a1, &b1, &c1, &d1, &e1);
    }

    // 3rd Row obtained: 5 cols
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        sscanf(buffer, "%lf,%lf,%lf,%lf,%lf", &a2, &b2, &c2, &d2, &e2);
    }

    // 4th Row obtaeind: 4 cols 
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        sscanf(buffer, "%lf,%lf,%lf,%lf", &a_deadZone, &b_deadZone, &c_deadZone, &d_deadZone);
    }

    fclose(fp);
}