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


struct State {
	size_t currentMode_ = 0;

	floating_t vol0_ = 0;
	floating_t vol1_ = 0;
	floating_t vol2_ = 0;
	floating_t vol3_ = 0.5;
	floating_t vol4_ = 0;

	floating_t phase0_ = 0;
	floating_t phase1_ = 0;
	floating_t phase2_ = 0;
	floating_t phase3_ = 0;
	floating_t phase4_ = 0;

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
};

static State global_state;

floating_t State::* all_params_[39] = { &State::vol0_, &State::vol1_, &State::vol2_, &State::vol3_, &State::vol4_, &State::phase0_, &State::phase1_, &State::phase2_, &State::phase3_, &State::phase4_, &State::attack_,
			&State::decay_, &State::sustain_, &State::release_, &State::lpf_, &State::hpf_, &State::phaserRate_, &State::phaserFeedback_, &State::phaserDepth_, &State::waveguideFeedback_,
			&State::waveguideDelay_, &State::foldbackThreshold_, &State::echoDelay_, &State::echoFrequency_, &State::echoAttenuation_, &State::vibratoAmount_, &State::vibratoFreq_,
			&State::flangerAmount_, &State::flangerDelay_, &State::flangerFrequency_, &State::flangerFFW_, &State::flangerFBK_, &State::reverbAmount_, &State::reverbDecay_,
			&State::reverbDamping_, &State::bitcrushAmount_, &State::bitcrushReduction_, &State::bitcrushQuantization_, &State::chebyModFrequency };

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

	Filters(LiquidCrystal& lcd) :
		lcd_(lcd), waveguide_(RING_BUFFER_SIZE), echo_(CLOCK_FREQ), vibrato_(0,0) {
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
				performUpdate("LSaw phase", global_state.phase0_, value);
				break;
			case 58:
				performUpdate("RSaw phase", global_state.phase1_, value);
				break;
			case 59:
				performUpdate("Triangle phase", global_state.phase1_, value);
				break;
			case 60:
				performUpdate("Sine phase", global_state.phase2_, value);
				break;
			case 61:
				performUpdate("Square phase", global_state.phase3_, value);
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
		}
	}

	void displayMode() {
		lcd_.clear();
		lcd_.setCursor(0, 0);

		if (global_state.currentMode_ == 0) {
			lcd_.print("Volume/Phase");
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
		for(size_t i = 0; i < 39; ++i) {
			performUpdate("random", global_state.*all_params_[i], (random(0, 255) / 255.0) * 0.8 + 0.1);
		}
		updateAllFilters();
	}
};

#endif /* PARAMETERS_HPP_ */
