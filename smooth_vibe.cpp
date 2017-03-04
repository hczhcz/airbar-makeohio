#include "all.hpp"

double smooth_vibe(
    double vibe,
    uint64_t time_div, double up, double down
) {
    static double result = 0;

    if (result >= 0) {
        if (vibe >= result) {
            double delta = 0.001 * time_div * up;
            result = fmin(result + delta, vibe);
        } else {
            double delta = 0.001 * time_div * down;
            result = fmax(result - delta, vibe);
        }
    } else {
        if (vibe <= result) {
            double delta = 0.001 * time_div * up;
            result = fmax(result - delta, vibe);
        } else {
            double delta = 0.001 * time_div * down;
            result = fmin(result + delta, vibe);
        }
    }

    return result;
}
