#include <map>
#include <vector>
#include <limits>

#include "MIDI.h"
#include "LiquidCrystal.h"
#include "CircularBuffer.h"
#include "Bounce.h"
#include "Gamma/Domain.h"
#include "USBHost_t36.h"

#include "defines.hpp"
#include "workarounds.hpp"
#include "state.hpp"
#include "signal.hpp"
#include "tone.hpp"
#include "instrumentstore.hpp"

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice MIDI(myusb);

size_t global_tick = 0;
CircularBuffer<sample_t, RING_BUFFER_SIZE> audio_buffer;
std::map<floating_t, Tone> tones_map;

Bounce prev_button = Bounce(11, 10);
Bounce next_button = Bounce(12, 10);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Filters filters(lcd);
InstrumentStore store(lcd);

floating_t apply_filters(const floating_t& s) {
	floating_t result = s;
	if (global_state.phaserDepth_ > 0) {
		result = filters.phaser_.next(result / 255.0) * 255.0;
	}

	if (global_state.waveguideDelay_ > 0) {
		result = filters.waveguide_.next(result / 255.0) * 255.0;
	}

	//FIXME doesn't seem to do anything
//	if(global_state.foldbackThreshold_ > 0) {
//		result = filters.foldback_.next(result / 255.0) * 255.0;
//	}

	if (global_state.echoDelay_ > 0) {
		result = filters.echo_.next(result / 255.0) * 255.0;
	}

	if (global_state.vibratoAmount_ > 0) {
		result = filters.vibrato_.next((result / 255.0) * 2.0 - 1.0) * 127.0 + 127.0;
	}

	if (global_state.flangerAmount_ > 0) {
		result = filters.flanger_.next(result / 255.0) * 255.0;
	}

	if (global_state.reverbAmount_ > 0) {
		result = filters.reverb_.next(result / 255.0) * 255.0;
	}

	if (global_state.bitcrushReduction_ > 0) {
		result = filters.bitcrush_.next((result / 255.0) * 2.0 - 1.0) * 127.0 + 127.0;
	}

	if (global_state.chebyModFrequency > 0) {
		result = filters.cheby_.next((result / 255.0) * 2.0 - 1.0) * 127.0 + 127.0;
	}

	return result;
}

bool is_filter_chain_bad() {
	for (size_t i = 0; i < 40960; ++i) {
		apply_filters(0);
	}
	bool silent = true;
	//feed the filter chain a signal and check if output is zero
	for (size_t i = 0; i < 40960; ++i) {
		if (apply_filters(i % 255 / 255.0) > 0.2) {
			silent = false;
			break;
		}
	}
	for (size_t i = 0; i < 40960; ++i) {
		apply_filters(0);
	}

	return silent;
}

void handleNoteOn(byte inChannel, byte inNumber, byte inVelocity) {
	digitalWrite(13, HIGH);
	floating_t f = pow(2.0, (inNumber - 69.0) / 12.0) * 440.0;

	ToneDescriptor desc;
	desc.lsaw_ = global_state.vol0_;
	desc.rsaw_ = global_state.vol1_;
	desc.triangle_ = global_state.vol2_;
	desc.sine_ = global_state.vol3_;
	desc.square_ = global_state.vol4_;
	desc.lsawPhase_ = global_state.phase0_;
	desc.rsawPhase_ = global_state.phase1_;
	desc.trianglePhase_ = global_state.phase2_;
	desc.sinePhase_ = global_state.phase3_;
	desc.squarePhase_ = global_state.phase4_;
	desc.attack_ = global_state.attack_;
	desc.decay_ = global_state.decay_;
	desc.sustain_ = global_state.sustain_;
	desc.release_ = global_state.release_;
	desc.samples_ = round(CLOCK_FREQ / f);

	tones_map[f] = Tone(desc);
}

byte lastCC = 0;
void handleControlChange(byte inChannel, byte inNumber, byte inValue) {
	if (inNumber == 20) {
		store.loadInstrument();
		return;
	} else if (inNumber == 21) {
		store.saveInstrument();
		return;
	} else if (inNumber == 22) {
		//reset instrument
		global_state = State();
		return;
	} else if (inNumber == 23) {
		tones_map.clear();
		return;
	} else if (inNumber == 112) {
		store.setInstrumentIdx(inValue % MAX_INSTRUMENTS);

		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Selected instrument");
		lcd.setCursor(0, 1);
		lcd.print(String("#") + store.getInstrumentIdx());
		return;
	} else if (inNumber == 114) {
//		do {
//			filters.randomize();
//		} while(is_filter_chain_bad());
		return;
	}

	if (inNumber == 113 && inValue == 127)
		filters.incMode();
	else if (inNumber == 115 && inValue == 127)
		filters.decMode();
	else
		filters.updateValue(inNumber, ((floating_t) inValue) / 127);
}

void handleProgramChange(byte inChannel, byte inNumber) {
//	if (inNumber < MAX_INSTRUMENTS)
//		store.setInstrumentIdx(inNumber);
//	else
//		store.setInstrumentIdx(0);
//
//	lcd.clear();
//	lcd.setCursor(0, 0);
//	lcd.print("Selected instrument");
//	lcd.setCursor(0, 1);
//	lcd.print(String("#") + store.getInstrumentIdx());
}

void handleNoteOff(byte inChannel, byte inNumber, byte inVelocity) {
	floating_t f = pow(2.0, (inNumber - 69.0) / 12.0) * 440.0;
	if (tones_map.find(f) != tones_map.end())
		tones_map[f].release();
	digitalWrite(13, LOW);
}

void setup() {
	randomSeed(analogRead(A1));

	if (!store.checkMagic())
		store.initEEPROM();

	gam::sampleRate(CLOCK_FREQ);

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
	MIDI.setHandleProgramChange(handleProgramChange);

	MIDI.begin();
}

void loop() {
	unsigned long start = micros();
	myusb.Task();
	MIDI.read();

	floating_t total = 0;
	size_t t = ++global_tick;

	std::vector<floating_t> deleteMe;
	size_t numContributors = 0;
	for (auto& p : tones_map) {
		off_t next = p.second.next(t);
		if (next == -1) {
			deleteMe.push_back(p.first);
			continue;
		}
		++numContributors;
		total += next;
	}

	for (auto& f : deleteMe) {
		tones_map.erase(f);
	}

	size_t pulseWidth;

	pulseWidth = round(total);

	if (pulseWidth > 255) {
		pulseWidth = 255;
	}

	floating_t val = apply_filters(pulseWidth);
	sample_t next = audio_buffer.pop();

	analogWrite(1, next);

	audio_buffer.push(filters.highPass_(filters.lowPass_(val)));

	if (global_tick + 10 > std::numeric_limits<size_t>().max())
		global_tick = 0;

	floating_t duration = micros() - start;
	floating_t interval = 1000000 / CLOCK_FREQ;
	if (duration > interval) {
		Serial.print("Underrun: ");
		Serial.println(duration - interval);
	} else {
		delayMicroseconds(round(interval - duration));
	}
}
