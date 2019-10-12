/*
 * Phaser.hpp
 *
 *  Created on: Oct 10, 2019
 *      Author: elchaschab
 */

#ifndef PHASER_HPP_
#define PHASER_HPP_

#include <cmath>
#include "types.hpp"

class Phaser {
public:
	Phaser()  //initialise to some usefull defaults...
	:
			_fb(.7f), _lfoPhase(0.f), _depth(1.f), _zm1(0.f) {
		setRange(440.f, 1600.f);
		setRate(.5f);
	}

	void setRange(float fMin, float fMax) { // Hz
		_dmin = fMin / (CLOCK_FREQ / 2.f);
		_dmax = fMax / (CLOCK_FREQ / 2.f);
	}

	void setRate(float rate) { // cps
		_lfoInc = 2.f * M_PI * (rate / CLOCK_FREQ);
	}

	void setFeedback(float fb) { // 0 -> <1.
		_fb = fb;
	}

	void setDepth(float depth) {  // 0 -> 1.
		_depth = depth;
	}

	float update(float inSamp) {
		//calculate and update phaser sweep lfo...
		float d = _dmin + (_dmax - _dmin) * ((sin(_lfoPhase) + 1.f) / 2.f);
		_lfoPhase += _lfoInc;
		if (_lfoPhase >= M_PI * 2.f)
			_lfoPhase -= M_PI * 2.f;

		//update filter coeffs
		for (int i = 0; i < 6; i++)
			_alps[i].Delay(d);

		//calculate output
		float y = _alps[0].Update(
				_alps[1].Update(_alps[2].Update(_alps[3].Update(_alps[4].Update(_alps[5].Update(inSamp + _zm1 * _fb))))));
		_zm1 = y;

		return inSamp + y * _depth;
	}
private:
	class AllpassDelay {
	public:
		AllpassDelay() :
				_a1(0.f), _zm1(0.f) {
		}

		void Delay(float delay) { //sample delay time
			_a1 = (1.f - delay) / (1.f + delay);
		}

		float Update(float inSamp) {
			float y = inSamp * -_a1 + _zm1;
			_zm1 = y * _a1 + inSamp;

			return y;
		}
	private:
		float _a1, _zm1;
	};

	AllpassDelay _alps[6];

	float _dmin, _dmax; //range
	float _fb; //feedback
	float _lfoPhase;
	float _lfoInc;
	float _depth;

	float _zm1;
};

#endif /* PHASER_HPP_ */
