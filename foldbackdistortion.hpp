/*
 * foldbackdistortion.hpp
 *
 *  Created on: Oct 11, 2019
 *      Author: elchaschab
 */

#ifndef FOLDBACKDISTORTION_HPP_
#define FOLDBACKDISTORTION_HPP_

#include "types.hpp"
#include <cmath>

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class FoldbackDistortion {
public:
	floating_t next(floating_t in, floating_t threshold) {
		if (in > threshold || in < -threshold) {
			in = fabs(fabs(fmod(in - threshold, threshold * 4)) - threshold * 2) - threshold;
		}
		return in;
	}
};

#endif /* FOLDBACKDISTORTION_HPP_ */
