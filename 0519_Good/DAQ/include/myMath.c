#include "myHeader.h"

double Cal_avg(int num_data, double Ybar_pre, double Y) {
    double Ybar;

    Ybar = (1.0 - 1.0 / num_data) * Ybar_pre
        + (1.0 / num_data) * Y;

    return Ybar;
}
