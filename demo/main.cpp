#include <iostream>

#include "stk/Flute.h"
#include "stk/Clarinet.h"
#include "stk/Saxofony.h"
#include "stk/Bowed.h"
// auto decreasing
#include "stk/Plucked.h"
#include "stk/Mandolin.h"
#include "stk/Rhodey.h"

#include "stk/Envelope.h"
#include "stk/NRev.h"
#include "stk/JCRev.h"
#include "stk/Delay.h"
#include "stk/BiQuad.h"
#include "stk/Chorus.h"

#include "stk/RtAudio.h"

using namespace std;
using namespace stk;

enum Instrmnts {
	flute,
	clarinet,
	saxofony,
	bowed,
	plucked,
	mandolin,
	rhodey,
};

struct TickData {
	Instrmnt *inst;
	Envelope envelope;
	NRev nrev;
	JCRev jcrev;
	Delay delay;
	BiQuad biquad;
	Chorus chorus;

	TickData(Instrmnts i) {
		switch (i) {
			case flute:
				inst = new Flute(200); break;
			case clarinet:
				inst = new Clarinet(200); break;
			case saxofony:
				inst = new Saxofony(200); break;
			case bowed:
				inst = new Bowed(200); break;
			case plucked:
				inst = new Plucked(200); break;
			case mandolin:
				inst = new Mandolin(200); break;
			case rhodey:
				inst = new Rhodey(); break;
			default:
				throw;
		}

		envelope.setRate(0.001);

		//nrev.setT60(0.7);
		nrev.setEffectMix(0.45);
		//jcrev.setT60(0.7);
		jcrev.setEffectMix(0.45);

		//delay.setDelay(200);
		//delay.setMaximumDelay((unsigned long int)(Stk::sampleRate() * 1.1));
		//delay.setGain(1.0);

		biquad.setResonance(440.0, 0.98, true);
		//biquad.setResonance(880.0, 0.98, true);
		
		chorus.setModFrequency(1800.0);
		chorus.setModDepth(0.2);
		chorus.setEffectMix(0.20);
	}
};

int tick(void *outputBuffer, void *, unsigned int nBufferFrames, double, RtAudioStreamStatus, void *userData) {
	TickData *data = (TickData *) userData;
	StkFloat *samples = (StkFloat *) outputBuffer;

	for (unsigned int i = 0; i < nBufferFrames; i++) {
		*samples = data->inst->tick();
		*samples = data->biquad.tick(*samples);
		//*samples = data->delay.tick(*samples);
		//*samples = data->envelope.tick() * data->chorus.tick(*samples);
		//*samples = data->envelope.tick() * data->jcrev.tick(*samples);
		*samples = data->envelope.tick() * data->nrev.tick(*samples);
		if (*samples > 1.0) {
			cerr << "Overflow" << endl;
			*samples = 1.0;
		} else if (*samples < -1.0) {
			*samples = -1.0;
			cerr << "Underflow" << endl;
		}
		samples++;
	}

	return 0;
}

int main() {
	Stk::setSampleRate(44100.0);

	RtAudio out;
	RtAudio::StreamParameters parameters;
	parameters.deviceId = out.getDefaultOutputDevice();
	parameters.nChannels = 1;
	RtAudioFormat format = (sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

	unsigned int bufferFrames = RT_BUFFER_SIZE;
	TickData data = TickData(clarinet);
	out.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, (void *)&data );
	out.startStream();

	StkFloat freq[] = {440.0, 493.88, 554.37, 587.33, 659.26, 740.0, 830.61, 880.0, 440.0, 493.88, 554.37, 587.33, 659.26, 740.0, 830.61, 880.0,440.0, 493.88, 554.37, 587.33, 659.26, 740.0, 830.61, 880.0,440.0, 493.88, 554.37, 587.33, 659.26, 740.0, 830.61, 880.0,440.0, 493.88, 554.37, 587.33, 659.26, 740.0, 830.61, 880.0,};
	//StkFloat freq[] = {1108.73,880.0,987.77,659.26,659.26,659.26,};

	for (unsigned long i = 0; i < sizeof(freq) / sizeof(StkFloat); i++) {
		data.envelope.setTarget(1.0);
		data.inst->noteOn(freq[i], 0.5);
		Stk::sleep(200);
		//data.delay.clear();
	}
		data.inst->noteOff(0.5);
		Stk::sleep(1000);

	// Cleanup
	out.closeStream();
	delete data.inst;
	return 0;
}

