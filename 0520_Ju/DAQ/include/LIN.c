#include "Header.h"

double OperPoint_Vc = 0.0, OperPoint_Wgyro = 0.0;
double a1 = 0.0, a2 = 0.0;
double b1 = 0.0, b2 = 0.0;
double c1 = 0.0, c2 = 0.0;

double a_deadZone = 0.0;
double b_deadZone = 0.0;
double c_deadZone = 0.0;

double Linearization(double vin) {

    double Vc = OperPoint_Vc;

    double SlopeDeadZone = (OperPoint_Vc/ LIN_V_DEADZONE);
    double linearSlope = (OperPoint_Wgyro / LIN_V_DEADZONE);

    double w_ref = linearSlope * vin;
    double discriminant = 0.0;

    if (vin > LIN_V_DEADZONE) {

        discriminant = b1 * b1 - 4.0 * a1 * (c1 - w_ref);

        if (discriminant >= 0) Vc = (-b1 + sqrt(discriminant)) / (2.0 * a1);
    }
    else if (vin < -LIN_V_DEADZONE) {

        discriminant = b2 * b2 - 4.0 * a2 * (c2 - w_ref);

        if (discriminant >= 0) Vc = (-b2 + sqrt(discriminant)) / (2.0 * a2);
    }
    else {
        //discriminant = b_deadZone * b_deadZone - 4.0 * a_deadZone * (c_deadZone - w_ref);

        //if (discriminant >= 0) Vc = (-b_deadZone + sqrt(discriminant)) / (2.0 * a_deadZone);
        
        double Vc_pos_start = 2.74;
        double Vc_pos_end = 2.77;

        double Vc_neg_start = 2.25;
        double Vc_neg_end = 2.23;
        
        if (vin > 0.01) Vc = Vc_pos_start + (Vc_pos_end - Vc_pos_start) / LIN_V_DEADZONE * vin;
        else if (vin < -0.01) Vc = Vc_neg_start +(Vc_neg_end - Vc_neg_start) / LIN_V_DEADZONE * vin;
        else Vc = DAQ_V_STANDARD;
         
    
        //Vc = -1080 * vin * vin* vin + 8.1 * vin + DAQ_V_STANDARD;

        //Vc = DAQ_V_STANDARD;
    }

    return Vc;
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

    fp = fopen("../data/LIN_DZ_COEF.csv", "r");

    if (fp == NULL) {
        printf("ERROR: [IN Linearization] COULD NOT OPEN FILE!\n");
        system("pause");
        exit(1);
    }

    fscanf(fp, "%lf,%lf,%lf", &a_deadZone, &b_deadZone, &c_deadZone);

    fclose(fp);
}