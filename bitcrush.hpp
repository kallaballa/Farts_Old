#ifndef LIBRARIES_BITCRUSH_HPP_
#define LIBRARIES_BITCRUSH_HPP_

/*	Gamma - Generic processing library
 See COPYRIGHT file for authors and license information

 Example:	Bitcrushing
 Author:		Lance Putnam, 2012

 Description:
 This demonstrates a bitcrushing effect which adds a lo-fi, crunchy element to a
 sound. It is a combination of sample rate and bit reduction (quantization) of
 the input.
 */

#include "types.hpp"
#include "Gamma/Effects.h"
#include "Gamma/Oscillator.h"
using namespace gam;

class BitcrushEffect {
public:
	LFO<> modSR;			// Sample rate reduction modulator
	LFO<> modQn;			// Quantization modulator
	Quantizer<> qnt;		// The bitcrush effect
	floating_t amount_ = 0;

	BitcrushEffect() {
	}

	void setAmount(const floating_t& a) {
		amount_ = a;
	}

	void setReductionFreq(const floating_t& f) {
		modSR.freq(f);
	}

	void setQuantizationFreq(const floating_t& f) {
		modQn.freq(f);
	}

	floating_t next(const floating_t& s) {
		// Set sample rate quantization
		qnt.freq(modSR.triU() * 4000 + 1400);

		// Set amplitude quantization
		qnt.step(modQn.paraU() * amount_);

		// Apply the bitcrush
		return qnt(s);
	}
};

#endif /* LIBRARIES_BITCRUSH_HPP_ */
