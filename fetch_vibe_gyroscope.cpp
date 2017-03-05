#include "all.hpp"

#include <unistd.h>
#include <fcntl.h>

#define TTY_SEARCH_MAX 16

static int get_device() {
    for (size_t i = 0; i < TTY_SEARCH_MAX; ++i) {
        char buffer[256];

        snprintf(buffer, 256, "/dev/ttyACM%zu", i);

        int device = open(buffer, O_RDONLY | O_NONBLOCK);

        if (device != -1) {
            fprintf(stderr, "Gyroscope: ttyACM%zu\n", i);

            return device;
        }
    }

    return -1;
}

double fetch_vibe() {
    static result = 0;

    static int device = -1;

    if (device == -1) {
       device = get_device();
       usleep(10 * 1000);
    }

    if (device != -1) {
        double sum = 0;
        size_t count = 0;

        int16_t action;

        while (read(device, &action, sizeof(action)) == sizeof(action)) {
            sum += action / 32768.;
            ++count;
        }

        result = sum / (count + 0.001);
        result *= std::abs(result);
    } else {
        result = 0;
    }

    return 0;
}
