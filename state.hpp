#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include "types.hpp"
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

floating_t* all_params_[39] = { &global_state.vol0_, &global_state.vol1_, &global_state.vol2_, &global_state.vol3_, &global_state.vol4_, &global_state.phase0_, &global_state.phase1_, &global_state.phase2_, &global_state.phase3_, &global_state.phase4_, &global_state.attack_,
			&global_state.decay_, &global_state.sustain_, &global_state.release_, &global_state.lpf_, &global_state.hpf_, &global_state.phaserRate_, &global_state.phaserFeedback_, &global_state.phaserDepth_, &global_state.waveguideFeedback_,
			&global_state.waveguideDelay_, &global_state.foldbackThreshold_, &global_state.echoDelay_, &global_state.echoFrequency_, &global_state.echoAttenuation_, &global_state.vibratoAmount_, &global_state.vibratoFreq_,
			&global_state.flangerAmount_, &global_state.flangerDelay_, &global_state.flangerFrequency_, &global_state.flangerFFW_, &global_state.flangerFBK_, &global_state.reverbAmount_, &global_state.reverbDecay_,
			&global_state.reverbDamping_, &global_state.bitcrushAmount_, &global_state.bitcrushReduction_, &global_state.bitcrushQuantization_, &global_state.chebyModFrequency };

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
		highPass_.freq(global_state.hpf_ * CLOCK_FREQ);
		lowPass_.freq(global_state.lpf_ * CLOCK_FREQ);
		phaser_.setRate(global_state.phaserRate_);
		phaser_.setFeedback(global_state.phaserFeedback_);
		phaser_.setDepth(global_state.phaserDepth_);
		waveguide_.setFeedback(global_state.waveguideFeedback_);
		waveguide_.setDelay(global_state.waveguideDelay_);
		foldback_.setThreshold(global_state.foldbackThreshold_);
		echo_.setDelay(global_state.echoDelay_ * 9.0);
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
		cheby_.setModulationFreq(global_state.chebyModFrequency);
	}

	void updateValue(const size_t& i, const floating_t& value) {
		switch (global_state.currentMode_) {
		case 0:
			switch (i) {
			case 74:
				performUpdate("LSaw", global_state.vol0_, value);
				break;
			case 71:
				performUpdate("RSaw", global_state.vol1_, value);
				break;
			case 76:
				performUpdate("Triangle", global_state.vol2_, value);
				break;
			case 77:
				performUpdate("Sine", global_state.vol3_, value);
				break;
			case 93:
				performUpdate("Square", global_state.vol4_, value);
				break;
			case 73:
				performUpdate("LSaw phase", global_state.phase0_, value);
				break;
			case 75:
				performUpdate("RSaw phase", global_state.phase1_, value);
				break;
			case 18:
				performUpdate("Triangle phase", global_state.phase1_, value);
				break;
			case 19:
				performUpdate("Sine phase", global_state.phase2_, value);
				break;
			case 16:
				performUpdate("Square phase", global_state.phase3_, value);
				break;
			case 17:
			case 91:
			case 79:
			case 72:
			case 14:
			case 15:
			default:
				break;
			}
			break;
		case 1:
			switch (i) {
			case 74:
				performUpdate("Attack", global_state.attack_, value);
				break;
			case 71:
				performUpdate("Decay", global_state.decay_, value);
				break;
			case 76:
				performUpdate("Sustain", global_state.sustain_, value);
				break;
			case 77:
				performUpdate("Release", global_state.release_, value);
				break;
			case 93:
			case 73:
			case 75:
			case 18:
			case 19:
			case 16:
			case 17:
			case 91:
			case 79:
			case 72:
			case 14:
			case 15:
			default:
				break;
			}
			break;
		case 2:
			switch (i) {
			case 74:
				performUpdate("High Pass", global_state.hpf_, value);
				highPass_.freq(global_state.hpf_ * CLOCK_FREQ);
				break;
			case 71:
				performUpdate("Low Pass", global_state.lpf_, value);
				lowPass_.freq(global_state.lpf_ * CLOCK_FREQ);
				break;
			case 76:
			case 77:
			case 93:
			case 73:
			case 75:
			case 18:
			case 19:
			case 16:
			case 17:
			case 91:
			case 79:
			case 72:
			case 14:
			case 15:
			default:
				break;
			}
			break;
		case 3:
			switch (i) {
			case 74:
				performUpdate("Phase Rate", global_state.phaserRate_, value);
				phaser_.setRate(global_state.phaserRate_);
				break;
			case 71:
				performUpdate("Phase Feedback", global_state.phaserFeedback_, value);
				phaser_.setFeedback(global_state.phaserFeedback_);
				break;
			case 76:
				performUpdate("Phase Depth", global_state.phaserDepth_, value);
				phaser_.setDepth(global_state.phaserDepth_);
				break;
			case 77:
			case 93:
			case 73:
			case 75:
			case 18:
			case 19:
			case 16:
			case 17:
			case 91:
			case 79:
			case 72:
			case 14:
			case 15:
			default:
				break;
			}
			break;
		case 4:
			switch (i) {
			case 74:
				performUpdate("Waveguide Feedback", global_state.waveguideFeedback_, value);
				waveguide_.setFeedback(global_state.waveguideFeedback_);
				break;
			case 71:
				performUpdate("Waveguide Delay", global_state.waveguideDelay_, value);
				waveguide_.setDelay(global_state.waveguideDelay_);
				break;
			case 76:
			case 77:
			case 93:
			case 73:
			case 75:
			case 18:
			case 19:
			case 16:
			case 17:
			case 91:
			case 79:
			case 72:
			case 14:
			case 15:
			default:
				break;
			}
			break;
		case 5:
			switch (i) {
			case 74:
				performUpdate("Foldback Feedback", global_state.foldbackThreshold_, value);
				foldback_.setThreshold(global_state.foldbackThreshold_);
				break;
			case 71:
			case 76:
			case 77:
			case 93:
			case 73:
			case 75:
			case 18:
			case 19:
			case 16:
			case 17:
			case 91:
			case 79:
			case 72:
			case 14:
			case 15:
			default:
				break;
			}
			break;
		case 6:
			switch (i) {
			case 74:
				performUpdate("Echo Delay", global_state.echoDelay_, value);
				echo_.setDelay(value * 9.0);
				break;
			case 71:
				performUpdate("Echo Frequency", global_state.echoFrequency_, value);
				echo_.setFrequency(value * CLOCK_FREQ);
				break;
			case 76:
				performUpdate("Echo Attenuation", global_state.echoAttenuation_, value);
				echo_.setAttenuation(global_state.echoAttenuation_);
				break;
			case 77:
			case 93:
			case 73:
			case 75:
			case 18:
			case 19:
			case 16:
			case 17:
			case 91:
			case 79:
			case 72:
			case 14:
			case 15:
			default:
				break;
			}
			break;
		case 7:
			switch (i) {
			case 74:
				performUpdate("Vibrato Amount", global_state.vibratoAmount_, value);
				vibrato_.setAmount(1.0 / (800 * (1.0 - value)));
				break;
			case 71:
				performUpdate("Vibrato Frequency", global_state.vibratoFreq_, value);
				vibrato_.setFrequency(value * 10.0);
				break;
			case 76:
			case 77:
			case 93:
			case 73:
			case 75:
			case 18:
			case 19:
			case 16:
			case 17:
			case 91:
			case 79:
			case 72:
			case 14:
			case 15:
			default:
				break;
			}
			break;
			case 8:
				switch (i) {
				case 74:
					performUpdate("Amount", global_state.flangerAmount_, value);
					flanger_.setAmount(1.0 * (global_state.flangerAmount_ / 50));
					break;
				case 71:
					performUpdate("Delay", global_state.flangerDelay_, value);
					flanger_.setDelay(1.0 * (global_state.flangerDelay_ / 100));
					break;
				case 76:
					performUpdate("Frequency", global_state.flangerFrequency_, value);
					flanger_.setFrequency(global_state.flangerFrequency_);
					break;
				case 77:
					performUpdate("Feedforward", global_state.flangerFFW_, value);
					flanger_.setFeedforward(global_state.flangerFFW_ * 2 - 1);
					break;
				case 93:
					performUpdate("Feedback", global_state.flangerFBK_, value);
					flanger_.setFeedback(global_state.flangerFBK_ * 2 - 1);
					break;
				case 73:
				case 75:
				case 18:
				case 19:
				case 16:
				case 17:
				case 91:
				case 79:
				case 72:
				case 14:
				case 15:
				default:
					break;
				}
				break;
				case 9:
					switch (i) {
					case 74:
						performUpdate("Amount", global_state.reverbAmount_, value);
						reverb_.setAmount(1.0 * (global_state.reverbAmount_ / 50));
						break;
					case 71:
						performUpdate("Decay", global_state.reverbDecay_, value);
						reverb_.setDecay(global_state.reverbDecay_ * 16);
						break;
					case 76:
						performUpdate("Damping", global_state.reverbDamping_, value);
						reverb_.setDamping(global_state.reverbDamping_);
						break;
					case 77:
					case 93:
					case 73:
					case 75:
					case 18:
					case 19:
					case 16:
					case 17:
					case 91:
					case 79:
					case 72:
					case 14:
					case 15:
					default:
						break;
					}
					break;
					case 10:
						switch (i) {
						case 74:
							performUpdate("Amount", global_state.bitcrushAmount_, value);
							bitcrush_.setAmount(global_state.bitcrushAmount_);
							break;
						case 71:
							performUpdate("SR Reduction Fq", global_state.bitcrushReduction_, value);
							bitcrush_.setReductionFreq(global_state.bitcrushReduction_ * 1000);
							break;
						case 76:
							performUpdate("Quantization Fq", global_state.bitcrushQuantization_, value);
							bitcrush_.setQuantizationFreq(global_state.bitcrushQuantization_ * 1000);
							break;
						case 77:
						case 93:
						case 73:
						case 75:
						case 18:
						case 19:
						case 16:
						case 17:
						case 91:
						case 79:
						case 72:
						case 14:
						case 15:
						default:
							break;
						}
						break;
						case 11:
							switch (i) {
							case 74:
								performUpdate("Modulator Fq", global_state.chebyModFrequency, value);
								cheby_.setModulationFreq(global_state.chebyModFrequency);
								break;
							case 71:
							case 76:
							case 77:
							case 93:
							case 73:
							case 75:
							case 18:
							case 19:
							case 16:
							case 17:
							case 91:
							case 79:
							case 72:
							case 14:
							case 15:
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
			performUpdate("random", *all_params_[i], (random(0, 255) / 255.0) * 0.9 + 0.1);
		}
		updateAllFilters();
	}
};

#endif /* PARAMETERS_HPP_ */
