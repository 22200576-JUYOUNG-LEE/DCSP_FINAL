#include "Header.h"

double Linearization(double vin) {
    double vcmd = LIN_VOLT_START;

    double Vop1 = LIN_GYRO_START;

    double a1 = LIN_COEFFI_POSITIVE_A;
    double b1 = LIN_COEFFI_POSITIVE_B;
    double c1 = LIN_COEFFI_POSITIVE_C;

    double a2 = LIN_COEFFI_NEGATIVE_A;
    double b2 = LIN_COEFFI_NEGATIVE_B;
    double c2 = LIN_COEFFI_NEGATIVE_C;

    double klin = (Vop1 / LIN_V_DEADZONE);

    double w_ref = klin * vin;
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
