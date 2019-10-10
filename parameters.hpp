/*
 * parameters.hpp
 *
 *  Created on: Oct 10, 2019
 *      Author: elchaschab
 */

#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include "types.hpp"
#include "LiquidCrystal.h"
#include <cstddef>

struct UI {
	const size_t maxModes_ = 4;
	size_t currentMode_ = 0;

	floating_t vol0_ = 0;
	floating_t vol1_ = 0;
	floating_t vol2_ = 0;
	floating_t vol3_ = 0;
	floating_t vol4_ = 0;

	floating_t phase0_ = 0;
	floating_t phase1_ = 0;
	floating_t phase2_ = 0;
	floating_t phase3_ = 0;
	floating_t phase4_ = 0;

	floating_t attack_ = 0;
	floating_t decay_ = 0;
	floating_t sustain_ = 0;
	floating_t release_ = 0;

	floating_t lpf_ = 1.0;
	floating_t hpf_ = 0;
	floating_t blur_ = 0;
	floating_t sharpen_ = 0;

	LiquidCrystal& lcd_;

	UI(LiquidCrystal& lcd) : lcd_(lcd) {

	}
	void performUpdate(const char* name, floating_t& field, const floating_t& value) {
		field = value;
		lcd_.setCursor(0, 0);
		lcd_.print(String(name) + ": " + field);
	}

	void updateValue(const size_t& i, const floating_t& value) {
		switch (i) {
		case 0:
			performUpdate("Saw", vol0_, value);
			break;
		case 1:
			performUpdate("RSaw", vol1_, value);
			break;
		case 2:
			performUpdate("Triangle", vol2_, value);
			break;
		case 3:
			performUpdate("Sine", vol3_, value);
			break;
		case 4:
			performUpdate("Square", vol4_, value);
			break;
		case 5:
			performUpdate("Saw phase", phase0_, value);
			break;
		case 6:
			performUpdate("RSaw phase", phase1_, value);
			break;
		case 7:
			performUpdate("Triangle phase", phase2_, value);
			break;
		case 8:
			performUpdate("Sine phase", phase3_, value);
			break;
		case 9:
			performUpdate("Square phase", phase4_, value);
			break;
		case 10:
			performUpdate("Attack", attack_, value);
			break;
		case 11:
			performUpdate("Decay", decay_, value);
			break;
		case 12:
			performUpdate("Sustain", sustain_, value);
			break;
		case 13:
			performUpdate("Release", release_, value);
			break;
		case 14:
			performUpdate("LPF", lpf_, value);
			break;
		case 15:
			performUpdate("HPF", hpf_, value);
			break;
		case 16:
			performUpdate("Blur", blur_, value);
			break;
		case 17:
			performUpdate("Sharpen", sharpen_, value);
			break;
		default:
			break;
		}
	}
};

#endif /* PARAMETERS_HPP_ */
