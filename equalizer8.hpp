#ifndef EQUALIZER8_HPP_
#define EQUALIZER8_HPP_
#include "fft.hpp"

template <typename T, size_t TfftSize_> class Equalizer16 {
	farts::FFT<T, TfftSize_> fft_;
	float32_t weights_[TfftSize_ / 2];
public:
	Equalizer16() {
		for(size_t i = 0; i < TfftSize_ / 2; ++i)  {
			weights_[i] = 1.0;
		}
	}

	void updateWeight(const size_t& i, const floating_t& value) {
			weights_[i] = value;
	}

	void process(T& audio_buffer) {
		fft_.multiplySpectrum(audio_buffer, weights_);
	}
};



#endif /* EQUALIZER8_HPP_ */
