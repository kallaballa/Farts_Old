/*
 * foldbackdistortion.hpp
 *
 *  Created on: Oct 11, 2019
 *      Author: elchaschab
 */

#ifndef FOLDBACKDISTORTION_HPP_
#define FOLDBACKDISTORTION_HPP_

#include <cmath>
#include "defines.hpp"

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class FoldbackDistortion {
	floating_t threshold;
public:

	void setThreshold(const floating_t& t) {
		threshold = t;
	}

	floating_t next(const floating_t& in) {
		if (in > threshold || in < -threshold) {
			return fabs(fabs(fmod(in - threshold, threshold * 4)) - threshold * 2) - threshold;
		}
		return in;
	}
};

#endif /* FOLDBACKDISTORTION_HPP_ */
