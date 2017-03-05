#include "all.hpp"

double smooth_vibe(
    double vibe,
    double time_div, double k
) {
    static double result = 0;

    if (vibe >= result) {
        double delta = time_div * k;
        result = fmin(result + delta, vibe);
    } else {
        double delta = time_div * k;
        result = fmax(result - delta, vibe);
    }

    return result;
}
