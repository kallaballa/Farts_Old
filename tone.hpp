/*
 * tone.hpp
 *
 *  Created on: Oct 10, 2019
 *      Author: elchaschab
 */

#ifndef TONE_HPP_
#define TONE_HPP_

#include "types.hpp"
#include "signal.hpp"
#include <cstddef>
#include <limits>
#include <cmath>

struct ToneDescriptor {
	floating_t freq_;
	floating_t updateFreq_;
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
	size_t writes_;
};

constexpr size_t MAX_ATTACK = 100000;
constexpr size_t MAX_DECAY = 100000;
constexpr size_t MAX_SUSTAIN = 1.0;
constexpr size_t MAX_RELEASE = 100000;
class Tone {
private:
	ToneDescriptor desc_;

	LSawtooth lst_;
	RSawtooth rst_;
	Triangle tr_;
	Sine sn_;
	Square sq_;

	size_t localTick_ = 0;
	floating_t endAttack_ = 0;
	floating_t startRelease_ = std::numeric_limits<floating_t>().max();
	floating_t releaseDuration_ = 0;
	floating_t decayDuration_ = 1.0;
	floating_t sustainLevel_ = 1.0;
public:
	Tone() :
			lst_(0, 0), rst_(0, 0), tr_(0, 0), sn_(0, 0), sq_(0, 0) {
	}

	Tone(ToneDescriptor& desc) :
			desc_(desc), lst_(desc.updateFreq_, desc.freq_), rst_(desc.updateFreq_, desc.freq_), tr_(desc.updateFreq_,
					desc.freq_), sn_(desc.updateFreq_, desc.freq_), sq_(desc.updateFreq_, desc.freq_), endAttack_(
					MAX_ATTACK * desc_.attack_), releaseDuration_(MAX_RELEASE * desc.release_), decayDuration_(
					MAX_DECAY * desc.decay_), sustainLevel_(MAX_SUSTAIN * desc.sustain_) {
	}

	Tone& operator=(const Tone& other) {
		desc_ = other.desc_;
		lst_ = LSawtooth(other.desc_.updateFreq_, other.desc_.freq_);
		rst_ = RSawtooth(other.desc_.updateFreq_, other.desc_.freq_);
		tr_ = Triangle(other.desc_.updateFreq_, other.desc_.freq_);
		sn_ = Sine(other.desc_.updateFreq_, other.desc_.freq_);
		sq_ = Square(other.desc_.updateFreq_, other.desc_.freq_);
		endAttack_ = other.endAttack_;
		releaseDuration_ = other.releaseDuration_;
		decayDuration_ = other.decayDuration_;
		sustainLevel_ = other.sustainLevel_;

		return *this;
	}

	void startRelease() {
		startRelease_ = localTick_;
	}

	off_t next(size_t i) {
		floating_t attackCoef = (
				(localTick_ < endAttack_ && startRelease_ == std::numeric_limits<floating_t>().max()) ?
						((floating_t) localTick_ / ((floating_t) endAttack_ + 1.0)) : 1.0);
		floating_t decayCoef = (
				localTick_ > endAttack_ && startRelease_ == std::numeric_limits<floating_t>().max() ?
						sustainLevel_ + (sustainLevel_ * ((decayDuration_ - (localTick_ - endAttack_)) / decayDuration_)) : 1.0);
		floating_t releaseCoef = (
				(localTick_ >= startRelease_) ? (releaseDuration_ - (localTick_ - startRelease_)) / releaseDuration_ : 1.0);
		floating_t sig0 = 0.33 * desc_.lsaw_
				* (lst_.next(
						((size_t) round(i + (desc_.writes_ * desc_.lsawPhase_)) % desc_.writes_) / (floating_t) desc_.writes_));
		floating_t sig1 = 0.33 * desc_.rsaw_
				* (rst_.next(
						((size_t) round(i + (desc_.writes_ * desc_.rsawPhase_)) % desc_.writes_) / (floating_t) desc_.writes_));
		floating_t sig2 = 0.33 * desc_.triangle_
				* (tr_.next(
						((size_t) round(i + (desc_.writes_ * desc_.trianglePhase_)) % desc_.writes_) / (floating_t) desc_.writes_));
		floating_t sig3 = 0.33 * desc_.sine_
				* (sn_.next(
						((size_t) round(i + (desc_.writes_ * desc_.sinePhase_)) % desc_.writes_) / (floating_t) desc_.writes_));
		floating_t sig4 = 0.33 * desc_.square_
				* (sq_.next(
						((size_t) round(i + (desc_.writes_ * desc_.squarePhase_)) % desc_.writes_) / (floating_t) desc_.writes_));
		floating_t sig5 = (sig0 + sig1 + sig2 + sig3 + sig4) * releaseCoef * attackCoef;

		++localTick_;
		if (localTick_ >= (std::numeric_limits<size_t>().max() - 10))
			localTick_ = 0;

		if (releaseCoef < 0.1)
			return -1;

		return round(sig5);
	}
};

#endif /* TONE_HPP_ */
