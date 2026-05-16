#include "myHeader.h"

#define   iter_MAX   (int) 23

void Validation(TaskHandle taskAI, TaskHandle taskAO, double Vgyro_offset) {

    double      Final_time_triangle = 26;
    double      Final_time_sin = 22;

    const char* OutDirName = "Validation";
    char        OutFileName[100] = { "" };

    printf("\nValidation - Triangle\n");
    for (int iter = 0; iter < iter_MAX; iter++) {

        sprintf(OutFileName, "Vali_tri_iter%d.out", iter + 1);

        PauseDAQ(taskAO);
        PauseDAQ(taskAO);
        Dynamic_function(taskAI, taskAO, Vgyro_offset, Final_time_triangle, OutDirName, OutFileName, Vali_triangle);
    }

    printf("\nValidation - sin\n");
    for (int iter = 0; iter < iter_MAX; iter++) {

        sprintf(OutFileName, "Vali_sin_iter%d.out", iter + 1);

        PauseDAQ(taskAO);
        Dynamic_function(taskAI, taskAO, Vgyro_offset, Final_time_sin, OutDirName, OutFileName, Vali_sin);
    }
}

double Linear_func1(double Vcmd) {
    double Vc = Vcmd_offset;

    double a = 0.2725;
    double b = 0.6451;
    double c = 2.6870;

    double d = -0.306366;
    double e = 0.581539;
    double f = 2.309843;

    if (Vcmd >= -0.05 && Vcmd < 0.05) return Vc;
    else if (Vcmd > 0.05) return Vc = a * Vcmd * Vcmd + b * Vcmd + c;
    else return Vc = d * Vcmd * Vcmd + e * Vcmd + f;

}

double Linear_func2(double vin, double vdz) {
    double vcmd = 2.75;

    double Vop1 = 90.6066;

    double a = -313.5497;
    double b = 3016.6342;
    double c = -5849.5654;

    double a1 = 373.0041;
    double b1 = -329.4004;
    double c1 = -1203.8487;

    double klin = (Vop1 / vdz);

    double w_ref = klin * vin;
    double discriminant = 0.0;

    if (vin > vdz) {

        discriminant = b * b - 4.0 * a * (c - w_ref); // a*V^2 + b*V + (c - w_ref) = 0

        if (discriminant >= 0) vcmd = (-b + sqrt(discriminant)) / (2.0 * a);
    }
    else if (vin < -vdz) {

        discriminant = b1 * b1 - 4.0 * a1 * (c1 - w_ref); // a1*V^2 + b1*V + (c1 - w_ref) = 0

        if (discriminant >= 0) vcmd = (-b1 + sqrt(discriminant)) / (2.0 * a1);
    }
    else vcmd = 2.5; // Deadzone

    return vcmd;
}


double Vali_triangle(double Vcmd, double time, double Wgyro, double Vpoten) {

    double slope = 0.7 / 5; // need to modify

    if (time < 1) return 0;
    else if (time < 6) return Vcmd + slope * SAMPLING_TIME;
    else if (time < 16) return Vcmd - slope * SAMPLING_TIME;
    else if (time < 26) return Vcmd + slope * SAMPLING_TIME;
    else return Vcmd = 0;

}

double Vali_sin(double Vcmd, double time, double Wgyro, double Vpoten) {

    if (time < 1.0) return 0;
    else if (time < 20.0) return 0.7 * sin(2 * UNIT_PI * (time - 1.0) / 15.0);
    else return 0;

}