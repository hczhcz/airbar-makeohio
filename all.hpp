#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <cstring>

#include <map>

// 0 <= pitch <= 24
// 0 <= volume <= 1
// -1 <= vibe <= 1

enum class Flag {

};

struct Note {
    double pitch;
    double volume;
};

std::map<uint64_t, Note> &fetch_notes();
double fetch_vibe();
bool fetch_flag(Flag c);

std::map<uint64_t, Note> &smooth_notes(
    const std::map<uint64_t, Note> &notes,
    double time_div, double p, double up, double down
);
double smooth_vibe(
    double vibe,
    double time_div, double up, double down
);

std::map<uint64_t, Note> &apply_vibe(
    const std::map<uint64_t, Note> &notes, double vibe
);
