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
// effects
// #include "stk/Envelope.h"
#include "stk/NRev.h"
#include "stk/JCRev.h"
#include "stk/Echo.h"
#include "stk/BiQuad.h"
#include "stk/Chorus.h"

struct Effects {
    // stk::Envelope envelope;
    stk::NRev nrev;
    stk::JCRev jcrev;
    stk::Echo echo;
    stk::BiQuad biquad;
    stk::Chorus chorus;

    Effects() {
        // envelope.setTarget(1.0);
        // envelope.setRate(0.001);

        nrev.setEffectMix(0.2);
        jcrev.setEffectMix(0.2);

        echo.setDelay(50);
        echo.setMaximumDelay(round(stk::Stk::sampleRate() * 1.5));
        echo.setEffectMix(0.5);

        biquad.setResonance(440.0, 0.98, true);

        chorus.setModFrequency(1800.0);
        chorus.setModDepth(0.2);
        chorus.setEffectMix(0.5);
    }
};

int tick(
    void *buffer, void *, unsigned int buffer_size, double,
    RtAudioStreamStatus, void *
) {
    static std::map<uint64_t, stk::Instrmnt *> instruments;
    static Effects effects;

    fetch_flag();

    double offset = flags[Flag::pitch_1_2] ? -45
        : flags[Flag::pitch_2_3] ? -33
        : flags[Flag::pitch_3_4] ? -21
        : flags[Flag::pitch_4_5] ? -9
        : flags[Flag::pitch_5_6] ? 3
        : flags[Flag::pitch_6_7] ? 15
        : -21;

    stk::StkFloat *samples = (stk::StkFloat *) buffer;

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
        }
    }

    for (unsigned int i = 0; i < buffer_size; ++i, ++samples) {
        *samples = 0;

        for (auto &instrument: instruments) {
            *samples += notes.at(instrument.first).volume
                * instrument.second->tick();
        }

        *samples /= sqrt(instruments.size() + 0.0001);

        if (flags[Flag::fx_over]) {
            *samples *= pow(std::abs(*samples), -0.6);
        }
        if (flags[Flag::fx_dist]) {
            *samples *= 3;
        }

        *samples = fmin(fmax(*samples, -1), 1);

        if (flags[Flag::fx_nrev]) {
            *samples = effects.nrev.tick(*samples);
        } else {
            // effects.nrev.tick(*samples);
        }
        if (flags[Flag::fx_jcrev]) {
            *samples = effects.jcrev.tick(*samples);
        } else {
            // effects.jcrev.tick(*samples);
        }
        if (flags[Flag::fx_echo]) {
            *samples = effects.echo.tick(*samples);
        } else {
            // effects.echo.tick(*samples);
        }
        if (flags[Flag::fx_biquad]) {
            *samples = effects.biquad.tick(*samples);
        } else {
            // effects.biquad.tick(*samples);
        }
        if (flags[Flag::fx_chorus]) {
            *samples = effects.chorus.tick(*samples);
        } else {
            // effects.chorus.tick(*samples);
        }

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
