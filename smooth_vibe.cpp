#include "all.hpp"

double smooth_vibe(
    double vibe,
    double time_div, double up, double down
) {
    static double result = 0;

    if (result >= 0) {
        if (vibe >= result) {
            double delta = time_div * up;
            result = fmin(result + delta, vibe);
        } else {
            double delta = time_div * down;
            result = fmax(result - delta, vibe);
        }
    } else {
        if (vibe <= result) {
            double delta = time_div * up;
            result = fmax(result - delta, vibe);
        } else {
            double delta = time_div * down;
            result = fmin(result + delta, vibe);
        }
    }

    return result;
}
