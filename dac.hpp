#ifndef DAC_HPP_
#define DAC_HPP_

#include "Arduino.h"
#include "defines.hpp"

class DAC {
	int bigStepPin_;
	int smallStepPin_;
public:
	DAC(int bigStepPin, int smallStepPin) :
			bigStepPin_(bigStepPin), smallStepPin_(smallStepPin) {
		analogWriteFrequency(bigStepPin, 750000);
		analogWriteFrequency(smallStepPin, 750000);
		analogWriteResolution(16);
	}

	void write(const sample_t& v) {
		analogWrite(bigStepPin_,  (v >> 16) & 0XFFFF);
		analogWrite(smallStepPin_, v & 0XFFFF);
	}
};

#endif /* DAC_HPP_ */
