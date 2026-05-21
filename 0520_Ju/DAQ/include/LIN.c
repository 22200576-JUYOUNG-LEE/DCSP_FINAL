#include "Header.h"

double OperPoint_Vc = 0.0, OperPoint_Wgyro = 0.0;
double a1 = 0.0, a2 = 0.0;
double b1 = 0.0, b2 = 0.0;
double c1 = 0.0, c2 = 0.0;

double Linearization(double vin) {

    double vcmd = OperPoint_Vc;

    double linearSlope = (OperPoint_Wgyro / LIN_V_DEADZONE);

    double w_ref = linearSlope * vin;
    double discriminant = 0.0;

    if (vin > LIN_V_DEADZONE) {

        discriminant = b1 * b1 - 4.0 * a1 * (c1 - w_ref);

        if (discriminant >= 0) vcmd = (-b1 + sqrt(discriminant)) / (2.0 * a1);
    }
    else if (vin < -LIN_V_DEADZONE) {

        discriminant = b2 * b2 - 4.0 * a2 * (c2 - w_ref);

        if (discriminant >= 0) vcmd = (-b2 + sqrt(discriminant)) / (2.0 * a2);
    }
    else vcmd = DAQ_V_STANDARD;

    return vcmd;
}

void ReadLinearCoefficent(void) {
    FILE* fp = fopen("../data/LIN_COEF.csv", "r");

    if (fp == NULL) {
        printf("ERROR: [IN Linearization] COULD NOT OPEN FILE!\n");
        system("pause");
        exit(1);
    }

    fscanf(fp, "%lf,%lf", &OperPoint_Vc, &OperPoint_Wgyro);
    fscanf(fp, "%lf,%lf", &a1, &a2);
    fscanf(fp, "%lf,%lf", &b1, &b2);
    fscanf(fp, "%lf,%lf", &c1, &c2);

    fclose(fp);
}