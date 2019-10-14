#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

#include "defines.hpp"

namespace farts {
struct Signal {
	Signal() {
	}

	virtual ~Signal() {
	}

	virtual size_t next(floating_t i) const = 0;
};
struct LSawtooth: public Signal {
public:
	LSawtooth() :	Signal() {
	}

	virtual size_t next(floating_t i) const {
		return round(SAMPLE_MAX - (SAMPLE_MAX * i));
	}
};

struct RSawtooth: public Signal {
public:
	RSawtooth() :	Signal() {
	}

	virtual size_t next(floating_t i) const {
		return round(SAMPLE_MAX * i);
	}
};

struct Triangle: public Signal {
public:
	Triangle() : Signal() {
	}

	virtual size_t next(floating_t i) const {
		if (i < 0.5)
			return round((SAMPLE_MAX * (i * 2)));
		else
			return round(SAMPLE_MAX - (SAMPLE_MAX * (i * 2)));
	}
};

struct Sine: public Signal {
public:
	Sine() : Signal() {
	}

	virtual size_t next(floating_t i) const {
		return SAMPLE_MAX * ((sin((M_PI * 2.0) * i) + 1.0) / 2.0);
	}
};
struct Square: public Signal {
public:
	Square() :	Signal() {
	}

	virtual size_t next(floating_t i) const {
		if (i < 0.5)
			return SAMPLE_MAX;
		else
			return 0;
	}
};
}
#endif /* SIGNAL_HPP_ */
