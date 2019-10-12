/*
 * waveguide.hpp
 *
 *  Created on: Oct 10, 2019
 *      Author: elchaschab
 */

#ifndef WAVEGUIDE_HPP_
#define WAVEGUIDE_HPP_

#include <vector>
#include <cstddef>
#include <cmath>

class WaveGuide {
public:
	const size_t bufferSize_;
	WaveGuide(size_t bufferSize) : bufferSize_(bufferSize), buffer_(bufferSize) {
		clear();
	}
	virtual ~WaveGuide() {
	}
	;

	void clear() {
		counter = 0;
		for (int s = 0; s < bufferSize_; s++)
			buffer_[s] = 0;
	}

	inline float feed(float const in, float const feedback, double const delay) {
		// calculate delay offset
		double back = (double) counter - delay;

		// clip lookback buffer-bound
		if (back < 0.0)
			back = bufferSize_ + back;

		// compute interpolation left-floor
		int const index0 = floor(back);

		// compute interpolation right-floor
		int index_1 = index0 - 1;
		int index1 = index0 + 1;
		int index2 = index0 + 2;

		// clip interp. buffer-bound
		if (index_1 < 0)
			index_1 = bufferSize_ - 1;
		if (index1 >= bufferSize_)
			index1 = 0;
		if (index2 >= bufferSize_)
			index2 = 0;

		// get neighbourgh samples
		float const y_1 = buffer_[index_1];
		float const y0 = buffer_[index0];
		float const y1 = buffer_[index1];
		float const y2 = buffer_[index2];

		// compute interpolation x
		float const x = (float) back - (float) index0;

		// calculate
		float const c0 = y0;
		float const c1 = 0.5f * (y1 - y_1);
		float const c2 = y_1 - 2.5f * y0 + 2.0f * y1 - 0.5f * y2;
		float const c3 = 0.5f * (y2 - y_1) + 1.5f * (y0 - y1);

		float const output = ((c3 * x + c2) * x + c1) * x + c0;

		// add to delay buffer
		buffer_[counter] = in + output * feedback;

		// increment delay counter
		counter++;

		// clip delay counter
		if (counter >= bufferSize_)
			counter = 0;

		// return output
		return output;
	}

	std::vector<float> buffer_;
	int counter;
};

#endif /* WAVEGUIDE_HPP_ */
