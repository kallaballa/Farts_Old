#ifndef FFT_HPP_
#define FFT_HPP_

#include "defines.hpp"
#include "Gamma/scl.h"

#include <cstddef>
#include "arm_math.h"
#include "arm_const_structs.h"

namespace farts {

template <typename T, size_t size_> class FFT {
	float32_t fftBuffer1_[size_];
	float32_t fftBuffer2_[size_];

	arm_rfft_fast_instance_f32 s_;

public:
	FFT() {
		arm_rfft_fast_init_f32(&s_, size_);
	}

	void forward(T& audio_buffer) {
		for (size_t i = 0; i < size_; ++i) {
			fftBuffer1_[i] = ((float32_t)audio_buffer[i] / (float32_t)SAMPLE_MAX) * gam::scl::hann(M_PI * 2 * (i / size_));
		}

		arm_rfft_fast_f32(&s_, fftBuffer1_, fftBuffer1_, 0);

		for (size_t i = 0; i < size_; ++i) {
			audio_buffer[i] = fftBuffer1_[i] * SAMPLE_MAX;
		}
	}

	void inverse(T& audio_buffer) {
		for (size_t i = 0; i < size_; ++i) {
			fftBuffer1_[i] = (audio_buffer[i] / SAMPLE_MAX);
		}

		arm_rfft_fast_f32(&s_, fftBuffer1_, fftBuffer1_, 1);

		for (size_t i = 0; i < size_; ++i) {
			audio_buffer[i] = fftBuffer1_[i] * SAMPLE_MAX;
		}
	}

	void multiplySpectrum(T& audio_buffer, const float32_t* weights) {
		for (size_t i = 0; i < size_; ++i) {
			fftBuffer1_[i] = audio_buffer[i];
		}

		arm_rfft_fast_f32(&s_, fftBuffer1_, fftBuffer2_, 0);

		for (size_t i = 0; i < size_; ++i) {
			fftBuffer2_[i] *= weights[i / 2];
		}

		arm_rfft_fast_f32(&s_, fftBuffer2_, fftBuffer1_, 1);

		for (size_t i = 0; i < size_; ++i) {
			audio_buffer[i] = fftBuffer1_[i];
		}
	}
};
}

#endif /* FFT_HPP_ */
