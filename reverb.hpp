/*
 * reverb.hpp
 *
 *  Created on: Oct 12, 2019
 *      Author: elchaschab
 */

#ifndef REVERB_HPP_
#define REVERB_HPP_

/*	Gamma - Generic processing library
	See COPYRIGHT file for authors and license information

Example:	Reverb
Author:		Lance Putnam, 2015

Description:
This demonstrates how to add reverberation, in the form of late reflections, to
a sound. The particular reverberation model used here is the Schroeder model
which consists of a parallel-comb, series-allpass configuration of delay lines.
*/

#include "types.hpp"
#include "Gamma/Oscillator.h"
#include "Gamma/Spatial.h"
using namespace gam;

class ReverbEffect {
public:
	unsigned seed;		// RNG seed
	ReverbMS<> reverb;	// Schroeder reverberator
	//ReverbMS<float, Loop1P1Z> reverb; // Use a 1-pole, 1-zero loop filter
	floating_t decay_ = 8;
	floating_t damping_ = 0.2;
	floating_t amount_ = 0.2;

	ReverbEffect(){
		seed = 1;

		// Specify a reverb preset
		//reverb.resize(gam::JCREVERB); // Chowning (4-comb, 3-allpass)
		reverb.resize(gam::FREEVERB); // Jezar (8-comb, 4-allpass)

		// Or invent your own...
		//reverb.resize({1289, 2951, 2013}, {499, 951}); // pretty smooth
		//reverb.resize({3229, 3281, 4833}, {4487, 4097}); // crunchy
		//reverb.resize({3387, 3255, 3121}, {583, 491}); // colored
		//reverb.resize({3431, 4403, 5813}, {1645, 1299});

		//reverb.resize({1323, 1807, 1945, 2045}, {1559, 797, 297}); // pretty smooth
		//reverb.resize({1305, 1503, 1581, 1837}, {709, 535, 237}); // also smooth
		//reverb.resize({3013, 3799, 3479, 1799}, {335, 689, 907}); // less diffuse

		// Set decay length, in seconds
		reverb.decay(decay_);

		// Set high-frequency damping factor in [0, 1]
		reverb.damping(damping_);
	}

	void setDecay(const floating_t& d) {
		decay_ = d;
	}

	void setDamping(const floating_t& d) {
		damping_ = d;
	}

	void setAmount(const floating_t& a) {
		amount_ = a;
	}

	floating_t next(const floating_t& s){
		// Pass grain through reverberator and mix with dry source
		return s + reverb(s) * amount_;
	}
};

#endif /* REVERB_HPP_ */
