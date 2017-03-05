#include "all.hpp"

double smooth_vibe(
    double vibe,
    double time_div, double k
) {
    static double result = 0;

    if (vibe >= result) {
        result = fmin(result + k * time_div, vibe);
    } else {
        result = fmax(result - k * time_div, vibe);
    }

    return result;
}
