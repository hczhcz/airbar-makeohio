#include "all.hpp"

std::map<uint64_t, Note> &smooth_notes(
    const std::map<uint64_t, Note> &notes,
    double time_div, double k, double up, double down
) {
    static std::map<uint64_t, Note> results;

    for (const auto &note: notes) {
        if (results.find(note.first) == results.end()) {
            results.insert(note);
        }
    }

    for (auto &result: results) {
        double new_pitch;
        double new_volume;

        if (notes.find(result.first) == notes.end()) {
            new_pitch = result.second.pitch;
            new_volume = 0;
        } else {
            new_pitch = notes.at(result.first).pitch;
            new_volume = notes.at(result.first).volume;
        }

        if (new_pitch >= result.second.pitch) {
            result.second.pitch = fmin(
                result.second.pitch + k * time_div, new_pitch
            );
        } else {
            result.second.pitch = fmax(
                result.second.pitch - k * time_div, new_pitch
            );
        }

        if (new_volume >= result.second.volume) {
            result.second.volume = fmin(
                result.second.volume + up * time_div, new_volume
            );
        } else {
            result.second.volume = fmax(
                result.second.volume - down * time_div, new_volume
            );
        }

        if (result.second.volume < 0.001) { // notice: limit
            results.erase(result.first);
        }
    }

    return results;
}
