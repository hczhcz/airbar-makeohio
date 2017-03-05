#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <cstring>

#include <map>

// 0 <= pitch <= 24
// 0 <= volume <= 1
// -1 <= vibe <= 1

namespace Flag {
    enum {
        inst_flute, // default
        inst_clarinet,
        inst_saxofony,
        inst_bowed,
        inst_plucked,
        inst_mandolin,
        inst_rhodey,
        pitch_1_2,
        pitch_2_3,
        pitch_3_4, // default
        pitch_4_5,
        pitch_5_6,
        pitch_6_7,
        pitch_round,
        pitch_fixed,
        note_smooth,
        note_delay,
        vibe_enable,
        vibe_smooth,
        vibe_delay,
        fx_over,
        fx_dist,
        fx_nrev,
        fx_jcrev,
        fx_delay,
        fx_biquad,
        fx_chorus
    };
};

struct Note {
    double pitch;
    double volume;
};

extern bool flags[256];

void fetch_flag();
std::map<uint64_t, Note> &fetch_notes();
double fetch_vibe();

std::map<uint64_t, Note> &smooth_notes(
    const std::map<uint64_t, Note> &notes,
    double time_div, double k, double up, double down
);
double smooth_vibe(
    double vibe,
    double time_div, double k
);

std::map<uint64_t, Note> &apply_vibe(
    const std::map<uint64_t, Note> &notes, double vibe
);
