/*	Gamma - Generic processing library
	See COPYRIGHT file for authors and license information
	
Example:	Echo
Author:		Lance Putnam, 2012

Description:
This demonstrates how to create an echo effect from a delay line. The output of
the delay line is fed back into the input to create a series of exponentially
decaying echoes. A low-pass "loop" filter is inserted into the feedback path to
simulate high-frequency damping due to air absorption.
*/

#ifndef ECHO_HPP_
#define ECHO_HPP_

#include "Gamma/Delay.h"
#include "Gamma/Filter.h"
#include "Gamma/Oscillator.h"
#include <cstdlib>
#include <unistd.h>
#include "defines.hpp"

using namespace gam;

class EchoEffect {
private:
	Delay<> delay;	// Delay line
	OnePole<> lpf;
	floating_t attenuation_ = 0.8;
public:
	EchoEffect(size_t clockFreq){
		delay.maxDelay(0.6);
		lpf.type(gam::LOW_PASS);
		lpf.freq(2000);
	}

	void setDelay(const floating_t& fractionOfMaxDelay) {
		delay.delay(fractionOfMaxDelay * delay.maxDelay());
	}

	void setFrequency(const floating_t& freq) {
		lpf.freq(freq);
	}

	void setAttenuation(const floating_t& att) {
		attenuation_ = att;
	}
	floating_t next(const floating_t& s){
			// Read the end of the delay line to get the echo
			floating_t echo = delay();

			// Low-pass filter and attenuate the echo
			echo = lpf(echo) * attenuation_;

			//sum of dry and wet signals
			floating_t sum = s + echo;

			// Write sum of current sample and echo to delay line
			delay(sum);

			return sum;
	}
};

#endif
