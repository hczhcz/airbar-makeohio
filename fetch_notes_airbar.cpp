#include "all.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/hidraw.h>

#define DEV_SEARCH_MAX 16
#define DEV_NAME "Neonode AirBar 15.6"
#define DEV_WIDTH 3452
#define DEV_HEIGHT 1942
#define DEV_OFFSET 1850
#define DEV_RANGE 1900
#define OUTPUT_PITCH_RANGE 24

struct __attribute__((__packed__)) AirBarData {
    uint8_t finger_max;
    uint8_t finger_count;
    uint16_t time;
    struct __attribute__((__packed__)) Finger {
        bool active: 1;
        uint8_t id: 7;
        uint16_t x: 16;
        uint16_t y: 16;
        uint16_t w: 16;
        uint16_t h: 16;
    } fingers[2];
};

int get_device() {
    fprintf(stderr, "Getting device\n");

    for (size_t i = 0; i < DEV_SEARCH_MAX; ++i) {
        char buffer[256];

        snprintf(buffer, 256, "/dev/hidraw%zu", i);

        int device = open(buffer, O_RDONLY | O_NONBLOCK);

        if (device != -1) {
            if (ioctl(device, HIDIOCGRAWNAME(256), buffer) != -1) {
                if (strncmp(buffer, DEV_NAME, strlen(DEV_NAME)) == 0) {
                    fprintf(stderr, "Device: hidraw%zu\n", i);

                    return device;
                }
            }

            close(device);
        }
    }

    return -1;
}

std::map<uint64_t, Note> &fetch_notes() {
    static std::map<uint64_t, Note> results;

    static int device = -1;

    if (device == -1 || fcntl(device, F_GETFD) == -1) {
        device = get_device();
    }

    if (device != -1) {
        AirBarData data;

        while (read(device, &data, sizeof(AirBarData)) == sizeof(AirBarData)) {
            results.clear();

            for (size_t i = 0; i < data.finger_count; ++i) {
                if (data.fingers[i].active) {
                    uint64_t id = data.fingers[i].id;

                    double pitch = double(data.fingers[i].x)
                        * OUTPUT_PITCH_RANGE
                        / DEV_WIDTH;
                    double volume = double(data.fingers[i].y - DEV_OFFSET)
                        / (DEV_RANGE - DEV_OFFSET);

                    if (volume > 1) {
                        volume = 1;
                    }

                    if (volume > 0) {
                        results.insert({id, Note {pitch, volume}});
                    }
                }
            }
        }
    } else {
        results.clear();
    }

    return results;
}
