#ifndef ENVELOP_HPP_
#define ENVELOP_HPP_

/*	Gamma - Generic processing library
 See COPYRIGHT file for authors and license information

 Example:	Env
 Author:		Lance Putnam, 2015

 Description:
 This demonstrates usage of the Env class, an n-stage exponential envelope.
 In particular, we demonstrate how to construct an AHD (attack-hold-decay)
 envelope using a 3-stage Env.

 |  /----------\
| /            \
|/              \
+-------------------> time
 A     H      D

 */
#include "Arduino.h"
#include "types.hpp"
#include "Gamma/Envelope.h"
using namespace gam;

class EnvelopeEffect {
public:

	ADSR<> env;		// ADSR envelope

	EnvelopeEffect() {
		env.attack(0.01);	// Set attack time in seconds
		env.decay(1);		// Set decay time in seconds
		env.sustain(0.5);	// Set sustain level
		env.release(5);		// Set release time in seconds
		env.amp(1);		// Set overall amplitude (default == 1)
	}

	void set(const floating_t& attackSecs, const floating_t& decaySecs, const floating_t& sustainLvl,
			const floating_t& releaseSecs) {
		env.attack(attackSecs);	// Set attack time in seconds
		env.decay(decaySecs);		// Set decay time in seconds
		env.sustain(sustainLvl);	// Set sustain level
		env.release(releaseSecs);
	}

	void release() {
		env.release();
	}

	floating_t next(const floating_t& s) {
		floating_t e = env();

		if(env.done())
			return -1;
		return s * e;
	}
};

#endif /* ENVELOP_HPP_ */
