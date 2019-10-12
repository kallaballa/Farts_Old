#ifndef CHEBY_HPP_
#define CHEBY_HPP_

/*	Gamma - Generic processing library
 See COPYRIGHT file for authors and license information

 Example:	Chebyshev Waveshaping
 Author:		Lance Putnam, 2012

 Description:
 This demonstrates how to waveshape a sine wave into a sum of harmonic cosine
 waves using Chebyshev polynomials of the first kind.
 */

#include "types.hpp"
#include "Gamma/Effects.h"
#include "Gamma/Oscillator.h"

using namespace gam;

class ChebyEffect {
public:

	ChebyN<12> cheby;		// Chebyshev waveshaper with 12 harmonics
	LFO<> mod;				// Harmonic coefficient modulator
	floating_t modFreq = CLOCK_FREQ * 0.5 + 0.1;

	void setModulationFreq(const floating_t& f) {
		modFreq = f;
	}

	floating_t next(const floating_t& s) {
		// The waveshaper takes a sine wave in [-1, 1]

		mod.freq((CLOCK_FREQ * modFreq + 0.1) / cheby.size());
		// Create harmonic traveling wave
		for (unsigned k = 0; k < cheby.size(); ++k) {

			// Set amplitude of kth harmonic
			cheby.coef(k) = mod.para();
		}

		// Generate harmonics and divide by number of harmonics to prevent clipping
		return cheby(s) / cheby.size();
	}
};

#endif /* CHEBY_HPP_ */
