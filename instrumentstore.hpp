#ifndef INSTRUMENTSTORE_HPP_
#define INSTRUMENTSTORE_HPP_

#include "Arduino.h"
#include "EEPROM.h"
#include "LiquidCrystal.h"
#include <cstddef>

class InstrumentStore {
private:
	size_t instrumentIdx_ = 0;
	LiquidCrystal& lcd_;

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

	void getInstrument(const size_t& i, State& s) {
		if (i > MAX_INSTRUMENTS) {
			Serial.println("Invalid instrument index");
			return;
		}

		EEPROM.get(sizeof(Magic) + sizeof(State) * i, s);
	}

	void saveInstrument(const size_t& i) {
		if (i > MAX_INSTRUMENTS) {
			Serial.println("Invalid instrument index");
			return;
		}

		EEPROM.put(sizeof(Magic) + sizeof(State) * i, global_state);
		lcd_.clear();
		lcd_.setCursor(0, 0);
		lcd_.print("Saved instrument");
		lcd_.setCursor(0, 1);
		lcd_.print(String("#") + i);
	}

	void loadInstrument(const size_t& i) {
		getInstrument(i, global_state);
		lcd_.clear();
		lcd_.setCursor(0, 0);
		lcd_.print("Restored instrument");
		lcd_.setCursor(0, 1);
		lcd_.print(String("#") + i);
		global_state.currentMode_ = 0;
	}


public:
	InstrumentStore(LiquidCrystal& lcd) :
			lcd_(lcd) {
	}
	bool checkMagic() {
		Magic m;
		EEPROM.get(0, m);
		if (m.isValid()) {
			Serial.println("Found valid magic");
		} else {
			Serial.println("Found invalid magic");
		}
		return m.isValid();
	}

	void initEEPROM() {
		Serial.println("Initializing EEPROM");
		Magic m = { 'F', 'a', 'R', 't', 'S' };
		EEPROM.put(0, m);
		for (size_t i = 0; i < MAX_INSTRUMENTS; ++i) {
			saveInstrument(i);
		}
	}

	void setInstrumentIdx(const size_t& i) {
		instrumentIdx_ = i;
	}

	void incrInstrumentIdx() {
		if (instrumentIdx_ + 1 == MAX_INSTRUMENTS)
			instrumentIdx_ = 0;
		else
			++instrumentIdx_;
	}

	void decrInstrumentIdx() {
		if (instrumentIdx_ == 0)
			instrumentIdx_ = MAX_INSTRUMENTS - 1;
		else
			--instrumentIdx_;
	}

	size_t getInstrumentIdx() {
		return instrumentIdx_;
	}

	void saveInstrument() {
		saveInstrument(instrumentIdx_);
	}

	void loadInstrument() {
		loadInstrument(instrumentIdx_);
	}
};

#endif /* INSTRUMENTSTORE_HPP_ */
