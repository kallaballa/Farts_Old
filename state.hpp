#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include "phaser.hpp"
#include "waveguide.hpp"
#include "foldbackdistortion.hpp"
#include "echo.hpp"
#include "vibrato.hpp"
#include "flanger.hpp"
#include "envelope.hpp"
#include "reverb.hpp"
#include "bitcrush.hpp"
#include "cheby.hpp"
#include "LiquidCrystal.h"
#include <cstddef>
#include "defines.hpp"
#include "random.hpp"
#include "equalizer8.hpp"

struct State {
	size_t currentMode_ = 0;

	floating_t vol0_ = 0;
	floating_t vol1_ = 0;
	floating_t vol2_ = 0;
	floating_t vol3_ = 0.5;
	floating_t vol4_ = 0;

	floating_t tuning0_ = 1;
	floating_t tuning1_ = 1;
	floating_t tuning2_ = 1;
	floating_t tuning3_ = 1;
	floating_t tuning4_ = 1;

	floating_t attack_ = 0.01;
	floating_t decay_ = 0.01;
	floating_t sustain_ = 1;
	floating_t release_ = 0.01;

	floating_t lpf_ = 1.0;
	floating_t hpf_ = 0;

	floating_t phaserRate_ = 0;
	floating_t phaserFeedback_ = 0;
	floating_t phaserDepth_ = 0;

	floating_t waveguideFeedback_ = 0;
	floating_t waveguideDelay_ = 0;

	floating_t foldbackThreshold_ = 0;

	floating_t echoDelay_ = 0;
	floating_t echoFrequency_ = 0;
	floating_t echoAttenuation_ = 0;

	floating_t vibratoAmount_ = 0;
	floating_t vibratoFreq_ = 0;

	floating_t flangerAmount_ = 0;
	floating_t flangerDelay_ = 0;
	floating_t flangerFrequency_ = 0;
	floating_t flangerFFW_ = 0;
	floating_t flangerFBK_ = 0;

	floating_t reverbAmount_ = 0;
	floating_t reverbDecay_ = 0;
	floating_t reverbDamping_ = 0;

	floating_t bitcrushAmount_ = 0;
	floating_t bitcrushReduction_ = 0;
	floating_t bitcrushQuantization_ = 0;

	floating_t chebyModFrequency = 0;

	floating_t equalizer0 = 1.0;
	floating_t equalizer1 = 1.0;
	floating_t equalizer2 = 1.0;
	floating_t equalizer3 = 1.0;
	floating_t equalizer4 = 1.0;
	floating_t equalizer5 = 1.0;
	floating_t equalizer6 = 1.0;
	floating_t equalizer7 = 1.0;
	floating_t equalizer8 = 1.0;
	floating_t equalizer9 = 1.0;
	floating_t equalizer10 = 1.0;
	floating_t equalizer11 = 1.0;
	floating_t equalizer12 = 1.0;
	floating_t equalizer13 = 1.0;
	floating_t equalizer14 = 1.0;
	floating_t equalizer15 = 1.0;
};

static State global_state;

floating_t State::* equalizer_params_[16] = { &State::equalizer0, &State::equalizer1, &State::equalizer2,
		&State::equalizer3, &State::equalizer4, &State::equalizer5, &State::equalizer6, &State::equalizer7,
		&State::equalizer8, &State::equalizer9, &State::equalizer10, &State::equalizer11, &State::equalizer12,
		&State::equalizer13, &State::equalizer14, &State::equalizer15 };
floating_t State::* all_params_[47] = { &State::vol0_, &State::vol1_, &State::vol2_, &State::vol3_, &State::vol4_,
		&State::tuning0_, &State::tuning1_, &State::tuning2_, &State::tuning3_, &State::tuning4_, &State::attack_,
		&State::decay_, &State::sustain_, &State::release_, &State::lpf_, &State::hpf_, &State::phaserRate_,
		&State::phaserFeedback_, &State::phaserDepth_, &State::waveguideFeedback_, &State::waveguideDelay_,
		&State::foldbackThreshold_, &State::echoDelay_, &State::echoFrequency_, &State::echoAttenuation_,
		&State::vibratoAmount_, &State::vibratoFreq_, &State::flangerAmount_, &State::flangerDelay_,
		&State::flangerFrequency_, &State::flangerFFW_, &State::flangerFBK_, &State::reverbAmount_, &State::reverbDecay_,
		&State::reverbDamping_, &State::bitcrushAmount_, &State::bitcrushReduction_, &State::bitcrushQuantization_,
		&State::chebyModFrequency, &State::equalizer0, &State::equalizer1, &State::equalizer2, &State::equalizer3,
		&State::equalizer4, &State::equalizer5, &State::equalizer6, &State::equalizer7 };

struct Filters {
	LiquidCrystal& lcd_;
	OnePole<> lowPass_;
	OnePole<> highPass_;
	Phaser phaser_;
	WaveGuide waveguide_;
	FoldbackDistortion foldback_;
	EchoEffect echo_;
	VibratoEffect vibrato_;
	FlangerEffect flanger_;
	ReverbEffect reverb_;
	BitcrushEffect bitcrush_;
	ChebyEffect cheby_;
	Equalizer16<CircularBuffer<sample_t, RING_BUFFER_SIZE>, RING_BUFFER_SIZE> eq8_;

	Filters(LiquidCrystal& lcd) :
			lcd_(lcd), waveguide_(1024), echo_(CLOCK_FREQ), vibrato_(0, 0) {
		lowPass_.type(gam::LOW_PASS);
		highPass_.type(gam::HIGH_PASS);
	}

	void performUpdate(const char* name, floating_t& field, const floating_t& value) {
		field = value;
		lcd_.clear();
		lcd_.setCursor(0, 0);
		lcd_.print(name);
		lcd_.setCursor(0, 1);
		lcd_.print(field);
	}

	void updateAllFilters() {
		highPass_.freq(global_state.hpf_ * (CLOCK_FREQ / 10.0));
		lowPass_.freq(global_state.lpf_ * (CLOCK_FREQ / 10.0));
		phaser_.setRate(global_state.phaserRate_);
		phaser_.setFeedback(global_state.phaserFeedback_);
		phaser_.setDepth(global_state.phaserDepth_);
		waveguide_.setFeedback(global_state.waveguideFeedback_);
		waveguide_.setDelay(global_state.waveguideDelay_);
		foldback_.setThreshold(global_state.foldbackThreshold_);
		echo_.setDelay(global_state.echoDelay_ * 4.0);
		echo_.setFrequency(global_state.echoFrequency_ * CLOCK_FREQ);
		echo_.setAttenuation(global_state.echoAttenuation_);
		vibrato_.setAmount(1.0 / (800 * (1.0 - global_state.vibratoAmount_)));
		vibrato_.setFrequency(global_state.vibratoFreq_ * 10.0);
		flanger_.setAmount(1.0 * (global_state.flangerAmount_ / 50));
		flanger_.setDelay(1.0 * (global_state.flangerDelay_ / 100));
		flanger_.setFrequency(global_state.flangerFrequency_);
		flanger_.setFeedforward(global_state.flangerFFW_ * 2 - 1);
		flanger_.setFeedback(global_state.flangerFBK_ * 2 - 1);
		reverb_.setAmount(1.0 * (global_state.reverbAmount_ / 50));
		reverb_.setDecay(global_state.reverbDecay_ * 16);
		reverb_.setDamping(global_state.reverbDamping_);
		bitcrush_.setAmount(global_state.bitcrushAmount_);
		bitcrush_.setReductionFreq(global_state.bitcrushReduction_ * 1000);
		bitcrush_.setQuantizationFreq(global_state.bitcrushQuantization_ * 1000);
		cheby_.setModulationFreq(CLOCK_FREQ * global_state.chebyModFrequency + 0.1);

		for (size_t i = 0; i < 16; ++i)
			eq8_.updateWeight(i, global_state.*equalizer_params_[i]);
	}

	void updateValue(const size_t& i, const floating_t& value) {
		switch (global_state.currentMode_) {
		case 0:
			switch (i) {
			case 52:
				performUpdate("LSaw", global_state.vol0_, value);
				break;
			case 53:
				performUpdate("RSaw", global_state.vol1_, value);
				break;
			case 54:
				performUpdate("Triangle", global_state.vol2_, value);
				break;
			case 55:
				performUpdate("Sine", global_state.vol3_, value);
				break;
			case 56:
				performUpdate("Square", global_state.vol4_, value);
				break;
			case 57:
				performUpdate("LSaw tuning", global_state.tuning0_, value);
				break;
			case 58:
				performUpdate("RSaw tuning", global_state.tuning1_, value);
				break;
			case 59:
				performUpdate("Triangle tuning", global_state.tuning1_, value);
				break;
			case 60:
				performUpdate("Sine tuning", global_state.tuning2_, value);
				break;
			case 61:
				performUpdate("Square tuning", global_state.tuning3_, value);
				break;
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 1:
			switch (i) {
			case 52:
				performUpdate("Attack", global_state.attack_, value);
				break;
			case 53:
				performUpdate("Decay", global_state.decay_, value);
				break;
			case 54:
				performUpdate("Sustain", global_state.sustain_, value);
				break;
			case 55:
				performUpdate("Release", global_state.release_, value);
				break;
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 2:
			switch (i) {
			case 52:
				performUpdate("High Pass", global_state.hpf_, value);
				highPass_.freq(global_state.hpf_ * CLOCK_FREQ);
				break;
			case 53:
				performUpdate("Low Pass", global_state.lpf_, value);
				lowPass_.freq(global_state.lpf_ * CLOCK_FREQ);
				break;
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 3:
			switch (i) {
			case 52:
				performUpdate("Phase Rate", global_state.phaserRate_, value);
				phaser_.setRate(global_state.phaserRate_);
				break;
			case 53:
				performUpdate("Phase Feedback", global_state.phaserFeedback_, value);
				phaser_.setFeedback(global_state.phaserFeedback_);
				break;
			case 54:
				performUpdate("Phase Depth", global_state.phaserDepth_, value);
				phaser_.setDepth(global_state.phaserDepth_);
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 4:
			switch (i) {
			case 52:
				performUpdate("Waveguide Feedback", global_state.waveguideFeedback_, value);
				waveguide_.setFeedback(global_state.waveguideFeedback_);
				break;
			case 53:
				performUpdate("Waveguide Delay", global_state.waveguideDelay_, value);
				waveguide_.setDelay(global_state.waveguideDelay_);
				break;
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 5:
			switch (i) {
			case 52:
				performUpdate("Foldback Feedback", global_state.foldbackThreshold_, value);
				foldback_.setThreshold(global_state.foldbackThreshold_);
				break;
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 6:
			switch (i) {
			case 52:
				performUpdate("Echo Delay", global_state.echoDelay_, value);
				echo_.setDelay(value * 9.0);
				break;
			case 53:
				performUpdate("Echo Frequency", global_state.echoFrequency_, value);
				echo_.setFrequency(value * CLOCK_FREQ);
				break;
			case 54:
				performUpdate("Echo Attenuation", global_state.echoAttenuation_, value);
				echo_.setAttenuation(global_state.echoAttenuation_);
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 7:
			switch (i) {
			case 52:
				performUpdate("Vibrato Amount", global_state.vibratoAmount_, value);
				vibrato_.setAmount(1.0 / (800 * (1.0 - value)));
				break;
			case 53:
				performUpdate("Vibrato Frequency", global_state.vibratoFreq_, value);
				vibrato_.setFrequency(value * 10.0);
				break;
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 8:
			switch (i) {
			case 52:
				performUpdate("Amount", global_state.flangerAmount_, value);
				flanger_.setAmount(1.0 * (global_state.flangerAmount_ / 50));
				break;
			case 53:
				performUpdate("Delay", global_state.flangerDelay_, value);
				flanger_.setDelay(1.0 * (global_state.flangerDelay_ / 100));
				break;
			case 54:
				performUpdate("Frequency", global_state.flangerFrequency_, value);
				flanger_.setFrequency(global_state.flangerFrequency_);
				break;
			case 55:
				performUpdate("Feedforward", global_state.flangerFFW_, value);
				flanger_.setFeedforward(global_state.flangerFFW_ * 2 - 1);
				break;
			case 56:
				performUpdate("Feedback", global_state.flangerFBK_, value);
				flanger_.setFeedback(global_state.flangerFBK_ * 2 - 1);
				break;
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 9:
			switch (i) {
			case 52:
				performUpdate("Amount", global_state.reverbAmount_, value);
				reverb_.setAmount(1.0 * (global_state.reverbAmount_ / 50));
				break;
			case 53:
				performUpdate("Decay", global_state.reverbDecay_, value);
				reverb_.setDecay(global_state.reverbDecay_ * 16);
				break;
			case 54:
				performUpdate("Damping", global_state.reverbDamping_, value);
				reverb_.setDamping(global_state.reverbDamping_);
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 10:
			switch (i) {
			case 52:
				performUpdate("Amount", global_state.bitcrushAmount_, value);
				bitcrush_.setAmount(global_state.bitcrushAmount_);
				break;
			case 53:
				performUpdate("SR Reduction Fq", global_state.bitcrushReduction_, value);
				bitcrush_.setReductionFreq(global_state.bitcrushReduction_ * 1000);
				break;
			case 54:
				performUpdate("Quantization Fq", global_state.bitcrushQuantization_, value);
				bitcrush_.setQuantizationFreq(global_state.bitcrushQuantization_ * 1000);
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 11:
			switch (i) {
			case 52:
				performUpdate("Modulator Fq", global_state.chebyModFrequency, value);
				cheby_.setModulationFreq(CLOCK_FREQ * global_state.chebyModFrequency + 0.1);
				break;
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 12:
			switch (i) {
			case 52:
				performUpdate("Band 0", global_state.equalizer0, value);
				eq8_.updateWeight(0, global_state.equalizer0);
				break;
			case 53:
				performUpdate("Band 1", global_state.equalizer1, value);
				eq8_.updateWeight(1, global_state.equalizer1);
				break;
			case 54:
				performUpdate("Band 2", global_state.equalizer2, value);
				eq8_.updateWeight(2, global_state.equalizer2);
				break;
			case 55:
				performUpdate("Band 3", global_state.equalizer3, value);
				eq8_.updateWeight(3, global_state.equalizer3);
				break;
			case 56:
				performUpdate("Band 4", global_state.equalizer4, value);
				eq8_.updateWeight(4, global_state.equalizer4);
				break;
			case 57:
				performUpdate("Band 5", global_state.equalizer5, value);
				eq8_.updateWeight(5, global_state.equalizer5);
				break;
			case 58:
				performUpdate("Band 6", global_state.equalizer6, value);
				eq8_.updateWeight(6, global_state.equalizer6);
				break;
			case 59:
				performUpdate("Band 7", global_state.equalizer7, value);
				eq8_.updateWeight(7, global_state.equalizer7);
				break;
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		case 13:
			switch (i) {
			case 52:
				performUpdate("Band 8", global_state.equalizer8, value);
				eq8_.updateWeight(8, global_state.equalizer8);
				break;
			case 53:
				performUpdate("Band 9", global_state.equalizer9, value);
				eq8_.updateWeight(9, global_state.equalizer9);
				break;
			case 54:
				performUpdate("Band 10", global_state.equalizer10, value);
				eq8_.updateWeight(10, global_state.equalizer10);
				break;
			case 55:
				performUpdate("Band 11", global_state.equalizer11, value);
				eq8_.updateWeight(11, global_state.equalizer11);
				break;
			case 56:
				performUpdate("Band 12", global_state.equalizer12, value);
				eq8_.updateWeight(12, global_state.equalizer12);
				break;
			case 57:
				performUpdate("Band 13", global_state.equalizer13, value);
				eq8_.updateWeight(13, global_state.equalizer13);
				break;
			case 58:
				performUpdate("Band 14", global_state.equalizer14, value);
				eq8_.updateWeight(14, global_state.equalizer14);
				break;
			case 59:
				performUpdate("Band 15", global_state.equalizer15, value);
				eq8_.updateWeight(15, global_state.equalizer15);
				break;
			case 60:
			case 61:
			case 62:
			case 63:
			case 85:
			case 86:
			default:
				break;
			}
			break;
		}
	}

	void displayMode() {
		lcd_.clear();
		lcd_.setCursor(0, 0);

		if (global_state.currentMode_ == 0) {
			lcd_.print("Volume/Tuning");
		} else if (global_state.currentMode_ == 1) {
			lcd_.print("Envelope");
		} else if (global_state.currentMode_ == 2) {
			lcd_.print("Filters");
		} else if (global_state.currentMode_ == 3) {
			lcd_.print("Phaser");
		} else if (global_state.currentMode_ == 4) {
			lcd_.print("Waveguide");
		} else if (global_state.currentMode_ == 5) {
			lcd_.print("Foldback");
		} else if (global_state.currentMode_ == 6) {
			lcd_.print("Echo");
		} else if (global_state.currentMode_ == 7) {
			lcd_.print("Vibrato");
		} else if (global_state.currentMode_ == 8) {
			lcd_.print("Flanger");
		} else if (global_state.currentMode_ == 9) {
			lcd_.print("Reverb");
		} else if (global_state.currentMode_ == 10) {
			lcd_.print("Bitcrush");
		} else if (global_state.currentMode_ == 11) {
			lcd_.print("Chebyshev");
		} else if (global_state.currentMode_ == 12) {
			lcd_.print("Equalizer 0-7");
		} else if (global_state.currentMode_ == 13) {
			lcd_.print("Equalizer 8-15");
		}
	}
	void decMode() {
		if (global_state.currentMode_ == 0)
			global_state.currentMode_ = MAX_MODES - 1;
		else
			--global_state.currentMode_;

		displayMode();
	}
	void incMode() {
		if (global_state.currentMode_ + 1 == MAX_MODES)
			global_state.currentMode_ = 0;
		else
			++global_state.currentMode_;

		displayMode();
	}

	void randomize() {
		for (size_t i = 5; i < 39; ++i) {
			performUpdate("random", global_state.*all_params_[i], randomWeight());
		}
		updateAllFilters();
	}
};

#endif /* PARAMETERS_HPP_ */
