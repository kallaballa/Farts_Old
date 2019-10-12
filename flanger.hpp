#ifndef FLANGER_HPP_
#define FLANGER_HPP_

/*	Gamma - Generic processing library
 See COPYRIGHT file for authors and license information

 Example:	Flanger Effect
 Author:		Lance Putnam, 2012

 Description:
 This demonstrates how to create a flanging effect by slowly modulating the delay
 time of a delay line.
 */
#include "types.hpp"
#include "Gamma/Delay.h"
#include "Gamma/Oscillator.h"
using namespace gam;

class FlangerEffect {
public:
	FlangerEffect(floating_t delay = 1. / 500, floating_t modAmount = 1. / 1000, floating_t modFreq = 0.5) :
			delay(delay), modAmount(modAmount), comb(1. / 20, delay, 1, 0), mod(modFreq) {
	}

	floating_t next(floating_t i) {
		comb.delay(delay + mod.cos() * modAmount);
		return comb(i);
	}

	void setAmount(const floating_t& a) {
		modAmount = a;
	}

	void setFrequency(const floating_t& f) {
		mod = f;
	}

	void setDelay(const floating_t& d) {
		delay = d;
	}

	void setFeedback(const floating_t& fbk) {
		comb.fbk(fbk);
	}
	void setFeedforward(const floating_t& ffd) {
		comb.ffd(ffd);
	}

	floating_t delay, modAmount;
	Comb<floating_t, ipl::AllPass> comb;
	LFO<> mod;
};

#endif /* FLANGER_HPP_ */
