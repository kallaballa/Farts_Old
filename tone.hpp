#ifndef TONE_HPP_
#define TONE_HPP_

#include "signal.hpp"
#include <cstddef>
#include <limits>
#include <cmath>
#include "defines.hpp"

struct ToneDescriptor {
	floating_t lsaw_;
	floating_t rsaw_;
	floating_t triangle_;
	floating_t sine_;
	floating_t square_;
	floating_t lsawPhase_;
	floating_t rsawPhase_;
	floating_t trianglePhase_;
	floating_t sinePhase_;
	floating_t squarePhase_;
	floating_t attack_;
	floating_t decay_;
	floating_t sustain_;
	floating_t release_;
	size_t samples_;
};

class Tone {
private:
	ToneDescriptor desc_;

	farts::LSawtooth lst_;
	farts::RSawtooth rst_;
	farts::Triangle tr_;
	farts::Sine sn_;
	farts::Square sq_;
	EnvelopeEffect envelope_;
	size_t localTick_ = 0;
public:
	Tone() {
		envelope_.set(desc_.attack_ * 5, desc_.decay_ * 5, desc_.sustain_, desc_.release_ * 5);
	}

	Tone(ToneDescriptor& desc) :
			desc_(desc) {
		envelope_.set(desc_.attack_ * 5, desc_.decay_ * 5, desc_.sustain_, desc_.release_ * 5);
	}

	~Tone() {
		release();
	}

	Tone& operator=(const Tone& other) {
		desc_ = other.desc_;
		lst_ = farts::LSawtooth();
		rst_ = farts::RSawtooth();
		tr_ = farts::Triangle();
		sn_ = farts::Sine();
		sq_ = farts::Square();
		envelope_ = EnvelopeEffect();
		envelope_.set(desc_.attack_ * 5, desc_.decay_ * 5, desc_.sustain_, desc_.release_ * 5);
		return *this;
	}

	void release() {
		envelope_.release();
	}

	off_t next(size_t i) {
		floating_t sig0 = 0.33 * desc_.lsaw_
				* (lst_.next(
						((size_t) round(i + (desc_.samples_ * desc_.lsawPhase_)) % desc_.samples_) / (floating_t) desc_.samples_));
		floating_t sig1 = 0.33 * desc_.rsaw_
				* (rst_.next(
						((size_t) round(i + (desc_.samples_ * desc_.rsawPhase_)) % desc_.samples_) / (floating_t) desc_.samples_));
		floating_t sig2 = 0.33 * desc_.triangle_
				* (tr_.next(
						((size_t) round(i + (desc_.samples_ * desc_.trianglePhase_)) % desc_.samples_) / (floating_t) desc_.samples_));
		floating_t sig3 = 0.33 * desc_.sine_
				* (sn_.next(
						((size_t) round(i + (desc_.samples_ * desc_.sinePhase_)) % desc_.samples_) / (floating_t) desc_.samples_));
		floating_t sig4 = 0.33 * desc_.square_
				* (sq_.next(
						((size_t) round(i + (desc_.samples_ * desc_.squarePhase_)) % desc_.samples_) / (floating_t) desc_.samples_));
		floating_t sig5 = ((sig0 + sig1 + sig2 + sig3 + sig4));

		++localTick_;
		if (localTick_ >= (std::numeric_limits<size_t>().max() - 10))
			localTick_ = 0;

		return round(envelope_.next(sig5));
	}
};

#endif /* TONE_HPP_ */
