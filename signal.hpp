#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

#include "defines.hpp"
#include "random.hpp"

namespace farts {
struct Signal {
	Signal() {
	}

	virtual ~Signal() {
	}

	virtual floating_t next(const floating_t& i) = 0;
};
struct LSawtooth: public Signal {
public:
	LSawtooth() :	Signal() {
	}

	virtual floating_t next(const floating_t& i) {
		return (1 - i);
	}
};

struct RSawtooth: public Signal {
public:
	RSawtooth() :	Signal() {
	}

	virtual floating_t next(const floating_t& i) {
		return (i);
	}
};

struct Triangle: public Signal {
public:
	Triangle() : Signal() {
	}

	virtual floating_t next(const floating_t& i) {
		if (i < 0.5)
			return (i * 2);
		else
			return (2 - (i * 2));
	}
};

struct Sine: public Signal {
public:
	Sine() : Signal() {
	}

	virtual floating_t next(const floating_t& i) {
		return (sin(M_PI * 2.0 * i) + 1.0) / 2.0;
	}
};
struct Square: public Signal {
public:
	Square() :	Signal() {
	}

	virtual floating_t next(const floating_t& i) {
		if (i < 0.5)
			return 1;
		else
			return 0;
	}
};

std::random_device randomDev;

struct WhiteNoise: public Signal {
public:
	WhiteNoise() :	Signal() {
	}

	virtual floating_t next(const floating_t& i) {
		return randomWeight();
	}
};
}
#endif /* SIGNAL_HPP_ */
