void __attribute__ ((constructor)) premain()
{
//	pinMode(13, OUTPUT);
//	digitalWrite(13, HIGH);
}

#include "MIDI.h"
#include "LiquidCrystal.h"
#include "USBHost_t36.h"
#include "Tonic.h"
#include "tonic_lua.hpp"

#include "defines.hpp"
#include "workarounds.hpp"
#include "dac.hpp"
#include "instrumentstore.hpp"
#include "script.hpp"
#include "luawrapper.hpp"
//
//struct DMAMEMAllocator {
//	static DMAMEM char * heap[102400];
//	size_t last_ = 0;
//  typedef void *pointer;
//   std::map<uint32_t,size_t> pointerSizes_;
//
//  typedef size_t size_type;
//  pointer allocate(size_type n) {
//
//  	if(last_ + n> 102400) {
//  		Serial.println("alloc OOB");
//  		Serial.flush();
//  	}
//  	pointer p = heap+last_;
//  	pointerSizes_[(uint32_t)p] = n;
//  	if(pointerSizes_.find((uint32_t)p) == pointerSizes_.end()) {
//  		Serial.println("___ALLOCERROR___");
//  		Serial.flush();
//  	}
//
//  	last_ += n;
//  	return p;
//  }
//
//  pointer reallocate(pointer p, size_type n) {
//  	if(!p)
//  		return allocate(n);
//
//
//		if (pointerSizes_.find((uint32_t) p) == pointerSizes_.end()) {
//			Serial.print("----- NOT FOUND -----");
//		 	Serial.flush();
//	  	return 0;
//		} else {
//	  	if(last_ + n > 102400) {
//	  		Serial.println("realloc OOB");
//	  		Serial.flush();
//	  	}
//	  	size_t oldsize;
//			pointer p1 = heap+last_;
//			oldsize = pointerSizes_[(uint32_t)p];
//			size_t cpysize;
//
//			if(n < oldsize)
//				cpysize = n;
//			else
//				cpysize = oldsize;
//
//			size_t newsize = n;
//
//			last_ += newsize;
//
//			memcpy(p1,p, cpysize);
//			pointerSizes_[(uint32_t)p1] = newsize;
//	  	return p1;
//		}
//  	return 0;
//  }
//
//  void deallocate(pointer p, size_type n) {
////  	Serial.println("free");
//
////    KAGUYA_UNUSED(n);
////    std::free(p);
//  }
//};
//
//DMAMEM char * DMAMEMAllocator::heap[102400];


MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice usbMidi(myusb);

size_t global_tick = 0;

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
InstrumentStore store(lcd);
DAC dac(29, 25);
static Synth synth;

void handleNoteOn(byte inChannel, byte inNumber, byte inVelocity) {
	digitalWrite(13, HIGH);
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
	digitalWrite(13, LOW);
}

void setup() {
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	Serial.begin(115200);
	while(!Serial) {};
	Serial.print("START");

  Tonic::setSampleRate(CLOCK_FREQ);

	if (!store.checkMagic())
		store.initEEPROM();

	lcd.begin(16, 2);

	analogWriteFrequency(1, 750000);
	pinMode(11, INPUT_PULLUP);
	pinMode(12, INPUT_PULLUP);

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


}

void loop() {
//	std::shared_ptr<DMAMEMAllocator> ptr(new DMAMEMAllocator());
	kaguya::State state;
	state.openlibs();

	bind(state);
	Serial.println("s2");
	Serial.flush();

	Serial.println("s3");
	Serial.flush();

	state["synth"] = &synth;
	runString(state.state_, COMPR_DUCK_TEST);
	Serial.flush();
	return;

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
