#include <map>
#include <vector>
#include <limits>

#include "MIDI.h"
#include "LiquidCrystal.h"
#include "CircularBuffer.h"
#include "EEPROM.h"
#include "Bounce.h"
#include "Gamma/Domain.h"

#include "types.hpp"
#include "state.hpp"
#include "signal.hpp"
#include "tone.hpp"

struct Magic {
	char one_;
	char two_;
	char three_;
	char four_;
	char five_;

	bool isValid() {
		return one_ == 'F' && two_ == 'a' && three_ == 'R' && four_ == 't' && five_ == 'S';
	}
};

//workaround for linker error
unsigned __exidx_start;
unsigned __exidx_end;

//another linker workaround -.-
extern "C" {
int _kill(pid_t pid, int signum) {
	return 0;
}

pid_t _getpid(void) {
	return 1;
}

int _gettimeofday(struct timeval *tv, struct timezone *tz) {
	return 0;
}
}

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

size_t global_tick = 0;
CircularBuffer<sample_t, RING_BUFFER_SIZE> audio_buffer;
std::map<floating_t, Tone> tones_map;

Bounce prev_button = Bounce(11, 10);
Bounce next_button = Bounce(12, 10);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Filters filters(lcd);

size_t instrumentIndex = 0;

bool checkMagic() {
	Magic m;
	EEPROM.get(0, m);
	if(m.isValid()) {
		Serial.println("Found valid magic");
	} else {
		Serial.println("Found invalid magic");
	}
	return m.isValid();
}

void getInstrument(const size_t& i, State& s)  {
	if(i > MAX_INSTRUMENTS) {
		Serial.println("Invalid instrument index");
		return;
	}

	if(EEPROM.length() < sizeof(Magic) + sizeof(State) * (i + 1)) {
		lcd.clear();
		lcd.print("OOB on EEPROM");
	}

	EEPROM.get(sizeof(Magic) + sizeof(State) * i, s);
}

void saveInstrument(const size_t& i) {
	if(i > MAX_INSTRUMENTS) {
		Serial.println("Invalid instrument index");
		return;
	}
	if(EEPROM.length() < sizeof(Magic) + sizeof(State) * (i + 1)) {
		lcd.clear();
		lcd.print("EEPROM full");
	}

	EEPROM.put(sizeof(Magic) + sizeof(State) * i, global_state);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Saved instrument");
	lcd.setCursor(0,1);
	lcd.print(String("#") + instrumentIndex);
}

void loadInstrument(const size_t& i) {
	getInstrument(i, global_state);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Restored instrument");
	lcd.setCursor(0,1);
	lcd.print(String("#") + instrumentIndex);
	global_state.currentMode_ = 0;
}

void initEEPROM() {
	Serial.println("Initializing EEPROM");
	Magic m = {'F', 'a', 'R', 't', 'S'};
	EEPROM.put(0, m);
	for(size_t i = 0; i < MAX_INSTRUMENTS; ++i) {
		saveInstrument(i);
	}
}

void handleNoteOn(byte inChannel, byte inNumber, byte inVelocity) {
	if(inNumber == 36) {
		if(instrumentIndex >= 0)
			loadInstrument(instrumentIndex);
		return;
	} else if(inNumber == 37) {
		if(instrumentIndex >= 0)
			saveInstrument(instrumentIndex);
		return;
	}

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
	if(inNumber == 112) {
		if (inValue == 65) {
			if (instrumentIndex + 1 == MAX_INSTRUMENTS)
				instrumentIndex = 0;
			else
				++instrumentIndex;
		} else if(inValue == 63) {
			if (instrumentIndex == 0)
				instrumentIndex = MAX_INSTRUMENTS - 1;
			else
				--instrumentIndex;
		} else
			return;

		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Selected instrument");
		lcd.setCursor(0,1);
		lcd.print(String("#") + instrumentIndex);
		return;
	} else if(inNumber == 114) {
		//do something with second knob
	}
	if(inNumber == 113 && inValue == 127)
		filters.incMode();
	else if(inNumber == 115 && inValue == 127)
		filters.decMode();
	else
		filters.updateValue(inNumber, ((floating_t)inValue) / 127);
}

void handleProgramChange(byte inChannel, byte inNumber) {
	if(inNumber < MAX_INSTRUMENTS)
		instrumentIndex = inNumber;
	else
		instrumentIndex = 0;

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Selected instrument");
	lcd.setCursor(0,1);
	lcd.print(String("#") + instrumentIndex);
}

void handleNoteOff(byte inChannel, byte inNumber, byte inVelocity) {
	floating_t f = pow(2.0, (inNumber - 69.0) / 12.0) * 440.0;
	if (tones_map.find(f) != tones_map.end())
		tones_map[f].release();
	digitalWrite(13, LOW);
}

void setup() {
	randomSeed(analogRead(A8));

	if(!checkMagic())
		initEEPROM();

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

	MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
	unsigned long start = micros();

	MIDI.read();

	size_t numTones = tones_map.size();
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

	if (numTones == 0)
		pulseWidth = 0;
	else
		pulseWidth = round(total);

	if (pulseWidth > 255) {
		pulseWidth = 255;
	}

	floating_t val = pulseWidth;
	if(global_state.phaserDepth_ > 0) {
			val = filters.phaser_.next(val / 255.0) * 255.0;
	}

	if(global_state.waveguideDelay_ > 0) {
		val = filters.waveguide_.next(pulseWidth / 255.0) * 255.0;
	}

	if(global_state.foldbackThreshold_ > 0) {
		val = (filters.foldback_.next((val / 255.0) * 2.0 - 1.0)) * 127.0 + 127.0;
	}

	if(global_state.echoDelay_ > 0) {
		val = filters.echo_.next(val / 255.0) * 255.0;
	}

	if(global_state.vibratoAmount_ > 0) {
		val = filters.vibrato_.next(val / 255.0) * 255.0;
	}

	if(global_state.flangerAmount_ > 0) {
		val = filters.flanger_.next(val / 255.0) * 255.0;
	}

	if(global_state.reverbAmount_ > 0) {
		val = filters.reverb_.next(val / 255.0) * 255.0;
	}

	if(global_state.bitcrushReduction_ > 0) {
		val = filters.bitcrush_.next(val / 255.0) * 255.0;
	}

	if(global_state.chebyModFrequency > 0) {
		val = filters.cheby_.next(val / 255.0) * 255.0;
	}

	sample_t next = audio_buffer.pop();
	analogWrite(1, next);
	audio_buffer.push(filters.highPass_(filters.lowPass_(val)));

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
