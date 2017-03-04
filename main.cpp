#include "all.hpp"

// tmp
#include <unistd.h>
#include <iostream>
#define TIME_DIV 100

int main() {
    while (true) {
        const std::map<uint64_t, Note> &data {
            apply_vibe(
                smooth_notes(
                    fetch_notes(),
                    TIME_DIV, 5, 10, 5
                ),
                smooth_vibe(
                    fetch_vibe(),
                    TIME_DIV, 5, 5
                )
            )
        };

        for (auto i: data) {
            std::cerr << i.second.pitch << " " << i.second.volume << "\t";
        }
        std::cerr << std::endl;


        usleep(1000 * TIME_DIV);
    }
}
