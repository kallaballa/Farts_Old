#include <map>
#include <vector>
#include <limits>

#include "MIDI.h"
#include "LiquidCrystal.h"
#include "CircularBuffer.h"
#include "Bounce.h"
#include "USBHost_t36.h"
#include "Tonic.h"
#include "tonic_lua.hpp"

#include "defines.hpp"
#include "workarounds.hpp"
#include "dac.hpp"
#include "signal.hpp"
#include "tone.hpp"
#include "instrumentstore.hpp"
#include "luawrapper.hpp"
#include "script.hpp"


lua_State* LuaWrapper::L_global_ = nullptr;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice usbMidi(myusb);

size_t global_tick = 0;
std::map<floating_t, Tone> tones_map;
CircularBuffer<sample_t, RING_BUFFER_SIZE> audio_buffer;

Bounce prev_button = Bounce(11, 10);
Bounce next_button = Bounce(12, 10);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
InstrumentStore store(lcd);
DAC dac(29, 25);
LuaWrapper* lua;
static Synth synth;

void handleNoteOn(byte inChannel, byte inNumber, byte inVelocity) {
	digitalWrite(13, HIGH);
	floating_t f = pow(2.0, (inNumber - 69.0) / 12.0) * 440.0;

	ToneDescriptor desc;


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
//		global_state = State();
		return;
	} else if (inNumber == 23) {
		tones_map.clear();
		return;
	} else if (inNumber == 24) {
//		global_state = history.back();
		return;
	} else if (inNumber == 25) {
//		global_state = history.forward();
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


		return;
	}

//	if (inNumber == 113 && inValue == 127) {
//		history.commit(global_state);
//		filters.incMode();
//	} else if (inNumber == 115 && inValue == 127) {
//		history.commit(global_state);
//		filters.decMode();
//	} else
//		filters.updateValue(inNumber, ((floating_t) inValue) / 127);
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
		tones_map.erase(f);
	digitalWrite(13, LOW);
}
void setup() {
  Tonic::setSampleRate(CLOCK_FREQ);

	if (!store.checkMagic())
		store.initEEPROM();

	lcd.begin(16, 2);

	analogWriteFrequency(1, 750000);
	pinMode(13, OUTPUT);
	pinMode(11, INPUT_PULLUP);
	pinMode(12, INPUT_PULLUP);

	digitalWrite(13, HIGH);
	usbMidi.setHandleControlChange(handleControlChange);
	usbMidi.setHandleNoteOff(handleNoteOff);
	usbMidi.setHandleNoteOn(handleNoteOn);
	usbMidi.setHandleProgramChange(handleProgramChange);

	usbMidi.begin();
	MIDI.setHandleControlChange(handleControlChange);
	MIDI.setHandleNoteOff(handleNoteOff);
	MIDI.setHandleNoteOn(handleNoteOn);
	MIDI.setHandleProgramChange(handleProgramChange);

	MIDI.begin(MIDI_CHANNEL_OMNI);
	for(size_t i = 0; i < RING_BUFFER_SIZE; ++i) {
		audio_buffer.push(0);
	}
//	while(!Serial) {};
	Serial.println("begin");
	Serial.flush();
	lua = new LuaWrapper();
//	lua->runString("print(\"ok\")");
	Serial.println("s1");
	Serial.flush();

	bind(LuaWrapper::getState());
	kaguya::State state(LuaWrapper::getState());
	state["synth"] = &synth;
	state.dostring(SCRIPT);
	Serial.flush();
}

void loop() {
  unsigned long start = micros();
	myusb.Task();
	usbMidi.read();
	MIDI.read();

	++global_tick;

	float data[1] = { 1.0 };
	synth.fillBufferOfFloats(data, 1, 1);
	dac.write(data[0] * SAMPLE_MAX);


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
