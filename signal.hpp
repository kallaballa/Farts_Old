/*
 * signal.hpp
 *
 *  Created on: Oct 10, 2019
 *      Author: elchaschab
 */

#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

#include "types.hpp"

class Signal {
public:
	floating_t updateFreq_;
	floating_t freq_;

	Signal(floating_t updateFreq, floating_t freq) :
			updateFreq_(updateFreq), freq_(freq) {
	}

	virtual ~Signal() {
	}

	virtual size_t next(floating_t i) const = 0;
};
class LSawtooth: public Signal {
public:
	LSawtooth(floating_t updateFreq, floating_t freq) :
			Signal(updateFreq, freq) {
	}

	virtual size_t next(floating_t i) const {
		return round(255.0 - (255.0 * i));
	}
};

class RSawtooth: public Signal {
public:
	RSawtooth(floating_t updateFreq, floating_t freq) :
			Signal(updateFreq, freq) {
	}

	virtual size_t next(floating_t i) const {
		return round(255.0 * i);
	}
};

class Triangle: public Signal {
public:
	Triangle(floating_t updateFreq, floating_t freq) :
			Signal(updateFreq, freq) {
	}

	virtual size_t next(floating_t i) const {
		if (i < 0.5)
			return round((512.0 * i));
		else
			return round(512.0 - (512.0 * i));
	}
};

class Sine: public Signal {
public:
	Sine(floating_t updateFreq, floating_t freq) :
			Signal(updateFreq, freq) {
	}

	virtual size_t next(floating_t i) const {
		return 255.0 * ((sin((M_PI * 2) * i) + 1.0) / 2.0);
	}
};
class Square: public Signal {
public:
	Square(floating_t updateFreq, floating_t freq) :
			Signal(updateFreq, freq) {
	}

	virtual size_t next(floating_t i) const {
		if (i < 0.5)
			return 255;
		else
			return 0;
	}
};

#endif /* SIGNAL_HPP_ */
