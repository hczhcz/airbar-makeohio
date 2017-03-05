#include "all.hpp"

#include "stk/RtAudio.h"

// loop instruments
#include "stk/Flute.h"
#include "stk/Clarinet.h"
#include "stk/Saxofony.h"
#include "stk/Bowed.h"
// non-loop instruments
#include "stk/Plucked.h"
#include "stk/Mandolin.h"
#include "stk/Rhodey.h"

int tick(
    void *buffer, void *, unsigned int buffer_size, double,
    RtAudioStreamStatus, void *
) {
    static std::map<uint64_t, stk::Instrmnt *> instruments;

    double offset = flags[Flag::pitch_1_2] ? -45
        : flags[Flag::pitch_2_3] ? -33
        : flags[Flag::pitch_3_4] ? -21
        : flags[Flag::pitch_4_5] ? -9
        : flags[Flag::pitch_5_6] ? 3
        : flags[Flag::pitch_6_7] ? 15
        : -21;

    stk::StkFloat *samples = (stk::StkFloat *) buffer;

    for (unsigned int i = 0; i < buffer_size; ++i, ++samples) {
        std::map<uint64_t, Note> &notes {
            fetch_notes()
        };

        if (flags[Flag::note_smooth]) {
            if (flags[Flag::note_delay]) {
                notes = smooth_notes(
                    notes,
                    buffer_size / 44100., 60, 10, 1
                );
            } else {
                notes = smooth_notes(
                    notes,
                    buffer_size / 44100., 60, 10, 10
                );
            }
        }

        if (flags[Flag::vibe_enable]) {
            if (flags[Flag::vibe_smooth]) {
                if (flags[Flag::vibe_delay]) {
                    notes = apply_vibe(
                        notes,
                        smooth_vibe(
                            fetch_vibe(),
                            buffer_size / 44100., 1
                        )
                    );
                } else {
                    notes = apply_vibe(
                        notes,
                        smooth_vibe(
                            fetch_vibe(),
                            buffer_size / 44100., 10
                        )
                    );
                }
            } else {
                notes = apply_vibe(
                    notes,
                    fetch_vibe()
                );
            }
        };

        for (const auto &note: notes) {
            if (instruments.find(note.first) == instruments.end()) {
                instruments.insert({note.first, {
                    flags[Flag::inst_flute] ? (stk::Instrmnt *) new stk::Flute(30)
                        : flags[Flag::inst_clarinet] ? (stk::Instrmnt *) new stk::Clarinet(30)
                        : flags[Flag::inst_saxofony] ? (stk::Instrmnt *) new stk::Saxofony(30)
                        : flags[Flag::inst_bowed] ? (stk::Instrmnt *) new stk::Bowed(30)
                        : flags[Flag::inst_plucked] ? (stk::Instrmnt *) new stk::Plucked(30)
                        : flags[Flag::inst_mandolin] ? (stk::Instrmnt *) new stk::Mandolin(30)
                        : flags[Flag::inst_rhodey] ? (stk::Instrmnt *) new stk::Rhodey()
                        : (stk::Instrmnt *) new stk::Flute(30)
                }});

                instruments.at(note.first)->noteOn(
                    440 * pow(2, (offset + note.second.pitch) / 12), 1
                );
            }
        }

        *samples = 0;

        for (auto &instrument: instruments) {
            if (notes.find(instrument.first) == notes.end()) {
                instrument.second->noteOff(1);
                delete instrument.second;

                instruments.erase(instrument.first);
            } else {
                if (!flags[Flag::pitch_fixed]) {
                    instrument.second->setFrequency(
                        440 * pow(2, (offset + notes.at(instrument.first).pitch) / 12)
                    );
                }

                *samples += notes.at(instrument.first).volume
                    * instrument.second->tick();
            }
        }

        *samples /= sqrt(instruments.size() + 0.0001);
        *samples = fmin(fmax(*samples, -1), 1);
    }

    return 0;
}

int main() {
    stk::Stk::setSampleRate(44100);

    RtAudio out;
    RtAudio::StreamParameters parameters;

    parameters.deviceId = out.getDefaultOutputDevice();
    parameters.nChannels = 1;

    RtAudioFormat format = sizeof(stk::StkFloat) == 8
        ? RTAUDIO_FLOAT64
        : RTAUDIO_FLOAT32;
    unsigned int buffer_size = stk::RT_BUFFER_SIZE;

    out.openStream(
        &parameters, nullptr, format, (unsigned int) stk::Stk::sampleRate(),
        &buffer_size, &tick, nullptr
    );
    out.startStream();

    while (true) {
        stk::Stk::sleep(1000);
    }
}
