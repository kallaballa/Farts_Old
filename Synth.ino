#include "MIDI.h"
#include "simpleDSP_fft.h"
#include "simpleDSP_fir.h"
#include "simpleDSP_Global.h"
#include "simpleDSP_iir.h"
#include "LiquidCrystal.h"
#include "CircularBuffer.h"
#include "Bounce.h"
#include "Filters.h"
#include <map>
#include <vector>
#include <limits>

#include "parameters.hpp"
#include "signal.hpp"
#include "tone.hpp"

typedef unsigned char sample_t;
typedef double floating_t;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

constexpr floating_t CLOCK_FREQ = 100000;
constexpr size_t RING_BUFFER_SIZE = 1024;

size_t global_tick = 0;

Bounce prev_button = Bounce(11, 10);
Bounce next_button = Bounce(12, 10);

FilterOnePole low_pass(LOWPASS, CLOCK_FREQ);
FilterOnePole high_pass(HIGHPASS, 0);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

CircularBuffer<sample_t, RING_BUFFER_SIZE> audio_buffer;

UI ui(lcd);

std::map<floating_t, Tone> tones_map;

void handleNoteOn(byte inChannel, byte inNumber, byte inVelocity) {
	digitalWrite(13, HIGH);
	floating_t f = pow(2.0, (inNumber - 69.0) / 12.0) * 440.0;

	ToneDescriptor desc;
	desc.lsaw_ = ui.vol0_;
	desc.rsaw_ = ui.vol1_;
	desc.triangle_ = ui.vol2_;
	desc.sine_ = ui.vol3_;
	desc.square_ = ui.vol4_;
	desc.lsawPhase_ = ui.phase0_;
	desc.rsawPhase_ = ui.phase1_;
	desc.trianglePhase_ = ui.phase2_;
	desc.sinePhase_ = ui.phase3_;
	desc.squarePhase_ = ui.phase4_;
	desc.attack_ = ui.attack_;
	desc.decay_ = ui.decay_;
	desc.sustain_ = ui.sustain_;
	desc.release_ = ui.release_;
	desc.writes_ = round(CLOCK_FREQ / f);
	tones_map[f] = Tone(desc);
}

void handleControlChange(byte inChannel, byte inNumber, byte inValue) {
	ui.updateValue(inNumber, inValue);
	low_pass.setFrequency(CLOCK_FREQ * ui.lpf_);
	high_pass.setFrequency(CLOCK_FREQ * ui.hpf_);
}

void handleNoteOff(byte inChannel, byte inNumber, byte inVelocity) {
	floating_t f = pow(2.0, (inNumber - 69.0) / 12.0) * 440.0;
	if (tones_map.find(f) != tones_map.end())
		tones_map[f].startRelease();
	digitalWrite(13, LOW);
}

void setup() {
	for (size_t i = 0; i < RING_BUFFER_SIZE; ++i) {
		audio_buffer.push(0);
	}
	lcd.begin(16, 2);

	analogWriteFrequency(1, 750000);
	pinMode(13, OUTPUT);
	pinMode(11, INPUT_PULLUP);
	pinMode(12, INPUT_PULLUP);

	digitalWrite(13, HIGH);
	MIDI.setHandleControlChange(handleControlChange);
	MIDI.setHandleNoteOff(handleNoteOff);
	MIDI.setHandleNoteOn(handleNoteOn);
	MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
	unsigned long start = micros();

	if (prev_button.update()) {
		if (prev_button.fallingEdge()) {
		}
	}
	if (next_button.update()) {
		if (next_button.fallingEdge()) {
		}
	}

	MIDI.read();

	size_t numTones = tones_map.size();
	floating_t total = 0;
	size_t t = ++global_tick;

	std::vector<floating_t> deleteMe;
	for (auto& p : tones_map) {
		off_t next = p.second.next(t);
		if (next == -1) {
			deleteMe.push_back(p.first);
			continue;
		}

		total += next;
	}

	for (auto& f : deleteMe) {
		tones_map.erase(f);
	}

	size_t pulseWidth;

	if (numTones == 0)
		pulseWidth = 0;
	else
		pulseWidth = round(total);

	if (pulseWidth > 255) {
		pulseWidth = 255;
	}

	low_pass.input(pulseWidth);
	high_pass.input(low_pass.output());

	sample_t next = audio_buffer.pop();
	analogWrite(1, next);
	audio_buffer.push(high_pass.output());

	if (global_tick + 10 > std::numeric_limits<size_t>().max())
		global_tick = 0;

	floating_t duration = micros() - start;
	floating_t interval = 1000000 / CLOCK_FREQ;
	if (duration > interval) {
		Serial.print("Overrun: ");
		Serial.println(duration - interval);
	} else {
		delayMicroseconds(round(interval - duration));
	}
}
