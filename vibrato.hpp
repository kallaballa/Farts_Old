#ifndef VIBRATO_HPP_
#define VIBRATO_HPP_
/*	Gamma - Generic processing library
	See COPYRIGHT file for authors and license information

Example:	Vibrato Effect
Author:		Lance Putnam, 2012

Description:
This demonstrates how to create a vibrato effect by slowly modulating the delay
time of a delay line. Since the vibrato uses a delay line, it can be applied to
any sound source.
*/
#include "Gamma/Delay.h"
#include "Gamma/Oscillator.h"

#include "defines.hpp"

using namespace gam;

class VibratoEffect{
public:
	VibratoEffect(float modAmount=1./400, float modFreq=5)
	:	modAmount(modAmount),
		delay(0.1, 0), mod(modFreq)
	{}

	void setFrequency(const floating_t& f) {
		mod.set(f,0,0.5);
	}

	void setAmount(const floating_t& a) {
		modAmount=a;
	}

	floating_t next(const floating_t& s){
		delay.delay(mod.hann()*modAmount + 0.0001);
		return delay(s);
	}

	float modAmount;
	Delay<> delay;
	LFO<> mod;
};

#endif
