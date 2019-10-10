#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

#include "types.hpp"

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
		return round(255.0 - (255.0 * i));
	}
};

struct RSawtooth: public Signal {
public:
	RSawtooth() :	Signal() {
	}

	virtual size_t next(floating_t i) const {
		return round(255.0 * i);
	}
};

struct Triangle: public Signal {
public:
	Triangle() : Signal() {
	}

	virtual size_t next(floating_t i) const {
		if (i < 0.5)
			return round((512.0 * i));
		else
			return round(512.0 - (512.0 * i));
	}
};

struct Sine: public Signal {
public:
	Sine() : Signal() {
	}

	virtual size_t next(floating_t i) const {
		return 255.0 * ((sin((M_PI * 2) * i) + 1.0) / 2.0);
	}
};
struct Square: public Signal {
public:
	Square() :	Signal() {
	}

	virtual size_t next(floating_t i) const {
		if (i < 0.5)
			return 255;
		else
			return 0;
	}
};

#endif /* SIGNAL_HPP_ */
