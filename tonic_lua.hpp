#ifndef TONIC_LUA_HPP_
#define TONIC_LUA_HPP_

#include "Tonic.h"
#include "kaguya/kaguya.hpp"
using namespace Tonic;

void bind(lua_State* L) {
	kaguya::State state(L);
	state["Synth"].setClass(
			kaguya::UserdataMetatable<Synth>()
			.setConstructors<Synth()>()
			.addFunction("setLimitOutput", &Synth::setLimitOutput)
			.addFunction("setOutputGen", &Synth::setOutputGen)
			.addOverloadedFunctions("addParameter",
					(ControlParameter (Synth::*)(string))&Synth::addParameter,
						(ControlParameter (Synth::*)(string, TonicFloat))&Synth::addParameter,
						(void (Synth::*)(ControlParameter))&Synth::addParameter));

	state["Generator"].setClass(
			kaguya::UserdataMetatable<Generator>()
			.setConstructors<Generator()>()
			.addFunction("isStereoOutput", &Generator::isStereoOutput)
			.addFunction("tick", &Generator::tick)
					.addOverloadedFunctions("__add",
					(Adder (Generator::*)(float))&Generator::operator+,
					(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
					(Adder (Generator::*)(Generator))&Generator::operator+)
					.addOverloadedFunctions("__sub",
					(Subtractor (Generator::*)(float))&Generator::operator-,
					(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
					(Subtractor (Generator::*)(Generator))&Generator::operator-)
					.addOverloadedFunctions("__mul",
					(Multiplier (Generator::*)(float))&Generator::operator*,
					(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
					(Multiplier (Generator::*)(Generator))&Generator::operator*)
					.addOverloadedFunctions("__div",
					(Divider (Generator::*)(float))&Generator::operator/,
					(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
					(Divider (Generator::*)(Generator))&Generator::operator/));


	state["Multiplier"].setClass(
			kaguya::UserdataMetatable<Multiplier, Generator>()
			.setConstructors<Multiplier()>()
			.addFunction("input", &Multiplier::input)
			.addFunction("numInputs", &Multiplier::numInputs)
			.addOverloadedFunctions("__add",
			(Adder (Generator::*)(float))&Generator::operator+,
			(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
			(Adder (Generator::*)(Generator))&Generator::operator+)
			.addOverloadedFunctions("__sub",
			(Subtractor (Generator::*)(float))&Generator::operator-,
			(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
			(Subtractor (Generator::*)(Generator))&Generator::operator-)
			.addOverloadedFunctions("__mul",
			(Multiplier (Generator::*)(float))&Generator::operator*,
			(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
			(Multiplier (Generator::*)(Generator))&Generator::operator*)
			.addOverloadedFunctions("__div",
			(Divider (Generator::*)(float))&Generator::operator/,
			(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
			(Divider (Generator::*)(Generator))&Generator::operator/));

	state["Adder"].setClass(
			kaguya::UserdataMetatable<Adder, Generator>()
			.setConstructors<Adder()>()
			.addFunction("input", &Adder::input)
			.addFunction("numInputs", &Adder::numInputs)
			.addOverloadedFunctions("__add",
			(Adder (Generator::*)(float))&Generator::operator+,
			(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
			(Adder (Generator::*)(Generator))&Generator::operator+)
			.addOverloadedFunctions("__sub",
			(Subtractor (Generator::*)(float))&Generator::operator-,
			(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
			(Subtractor (Generator::*)(Generator))&Generator::operator-)
			.addOverloadedFunctions("__mul",
			(Multiplier (Generator::*)(float))&Generator::operator*,
			(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
			(Multiplier (Generator::*)(Generator))&Generator::operator*)
			.addOverloadedFunctions("__div",
			(Divider (Generator::*)(float))&Generator::operator/,
			(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
			(Divider (Generator::*)(Generator))&Generator::operator/));

	state["Subtractor"].setClass(
			kaguya::UserdataMetatable<Subtractor, Generator>()
			.setConstructors<Subtractor()>()
			.addOverloadedFunctions("left",
					(Subtractor& (Subtractor::*)(Generator))&Subtractor::left,
					(Subtractor& (Subtractor::*)(float))&Subtractor::left,
					(Subtractor& (Subtractor::*)(ControlGenerator))&Subtractor::left)
			.addOverloadedFunctions("right",
					(Subtractor& (Subtractor::*)(Generator))&Subtractor::right,
					(Subtractor& (Subtractor::*)(float))&Subtractor::right,
					(Subtractor& (Subtractor::*)(ControlGenerator))&Subtractor::right)
			.addOverloadedFunctions("__add",
			(Adder (Generator::*)(float))&Generator::operator+,
			(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
			(Adder (Generator::*)(Generator))&Generator::operator+)
			.addOverloadedFunctions("__sub",
			(Subtractor (Generator::*)(float))&Generator::operator-,
			(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
			(Subtractor (Generator::*)(Generator))&Generator::operator-)
			.addOverloadedFunctions("__mul",
			(Multiplier (Generator::*)(float))&Generator::operator*,
			(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
			(Multiplier (Generator::*)(Generator))&Generator::operator*)
			.addOverloadedFunctions("__div",
			(Divider (Generator::*)(float))&Generator::operator/,
			(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
			(Divider (Generator::*)(Generator))&Generator::operator/));

	state["Divider"].setClass(
				kaguya::UserdataMetatable<Divider, Generator>()
				.setConstructors<Divider()>()
				.addOverloadedFunctions("left",
						(Divider& (Divider::*)(Generator))&Divider::left,
						(Divider& (Divider::*)(float))&Divider::left,
						(Divider& (Divider::*)(ControlGenerator))&Divider::left)
				.addOverloadedFunctions("right",
						(Divider& (Divider::*)(Generator))&Divider::right,
						(Divider& (Divider::*)(float))&Divider::right,
						(Divider& (Divider::*)(ControlGenerator))&Divider::right)
				.addOverloadedFunctions("__add",
				(Adder (Generator::*)(float))&Generator::operator+,
				(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
				(Adder (Generator::*)(Generator))&Generator::operator+)
				.addOverloadedFunctions("__sub",
				(Subtractor (Generator::*)(float))&Generator::operator-,
				(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
				(Subtractor (Generator::*)(Generator))&Generator::operator-)
				.addOverloadedFunctions("__mul",
				(Multiplier (Generator::*)(float))&Generator::operator*,
				(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
				(Multiplier (Generator::*)(Generator))&Generator::operator*)
				.addOverloadedFunctions("__div",
				(Divider (Generator::*)(float))&Generator::operator/,
				(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
				(Divider (Generator::*)(Generator))&Generator::operator/));

	state["FixedValue"].setClass(
			kaguya::UserdataMetatable<FixedValue, Generator>()
			.setConstructors<FixedValue(), FixedValue(float)>()
			.addOverloadedFunctions("setValue",
			(FixedValue& (FixedValue::*)(float))&FixedValue::setValue,
			(FixedValue& (FixedValue::*)(ControlGenerator))&FixedValue::setValue)
			.addOverloadedFunctions("__add",
			(Adder (Generator::*)(float))&Generator::operator+,
			(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
			(Adder (Generator::*)(Generator))&Generator::operator+)
			.addOverloadedFunctions("__sub",
			(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
			(Subtractor (Generator::*)(float))&Generator::operator-,
			(Subtractor (Generator::*)(Generator))&Generator::operator-)
			.addOverloadedFunctions("__mul",
			(Multiplier (Generator::*)(float))&Generator::operator*,
			(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
			(Multiplier (Generator::*)(Generator))&Generator::operator*)
			.addOverloadedFunctions("__div",
			(Divider (Generator::*)(float))&Generator::operator/,
			(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
			(Divider (Generator::*)(Generator))&Generator::operator/));

	state["RampedValue"].setClass(
			kaguya::UserdataMetatable<RampedValue, Generator>()
			.setConstructors<RampedValue()>()
			.addFunction("isFinished", &RampedValue::isFinished)
			.addOverloadedFunctions("target",
					(RampedValue& (RampedValue::*)(float))&RampedValue::target,
					(RampedValue& (RampedValue::*)(ControlGenerator))&RampedValue::target)
			.addOverloadedFunctions("length",
					(RampedValue& (RampedValue::*)(float))&RampedValue::length,
					(RampedValue& (RampedValue::*)(ControlGenerator))&RampedValue::length)
			.addOverloadedFunctions("value",
					(RampedValue& (RampedValue::*)(float))&RampedValue::value,
					(RampedValue& (RampedValue::*)(ControlGenerator))&RampedValue::value)
					.addOverloadedFunctions("__add",
					(Adder (Generator::*)(float))&Generator::operator+,
					(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
					(Adder (Generator::*)(Generator))&Generator::operator+)
					.addOverloadedFunctions("__sub",
					(Subtractor (Generator::*)(float))&Generator::operator-,
					(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
					(Subtractor (Generator::*)(Generator))&Generator::operator-)
					.addOverloadedFunctions("__mul",
					(Multiplier (Generator::*)(float))&Generator::operator*,
					(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
					(Multiplier (Generator::*)(Generator))&Generator::operator*)
					.addOverloadedFunctions("__div",
					(Divider (Generator::*)(float))&Generator::operator/,
					(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
					(Divider (Generator::*)(Generator))&Generator::operator/));

	state["SineWave"].setClass(
			kaguya::UserdataMetatable<SineWave, Generator>()
			.setConstructors<SineWave()>()
			.addOverloadedFunctions("freq",
					(SineWave& (SineWave::*)(Generator))&SineWave::freq,
					(SineWave& (SineWave::*)(float))&SineWave::freq,
					(SineWave& (SineWave::*)(ControlGenerator))&SineWave::freq)
					.addOverloadedFunctions("__add",
					(Adder (Generator::*)(float))&Generator::operator+,
					(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
					(Adder (Generator::*)(Generator))&Generator::operator+)
					.addOverloadedFunctions("__sub",
					(Subtractor (Generator::*)(float))&Generator::operator-,
					(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
					(Subtractor (Generator::*)(Generator))&Generator::operator-)
					.addOverloadedFunctions("__mul",
					(Multiplier (Generator::*)(float))&Generator::operator*,
					(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
					(Multiplier (Generator::*)(Generator))&Generator::operator*)
					.addOverloadedFunctions("__div",
					(Divider (Generator::*)(float))&Generator::operator/,
					(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
					(Divider (Generator::*)(Generator))&Generator::operator/));

	state["RectWave"].setClass(
			kaguya::UserdataMetatable<RectWave, Generator>()
			.setConstructors<RectWave()>()
			.addOverloadedFunctions("freq",
					(RectWave& (RectWave::*)(Generator))&RectWave::freq,
					(RectWave& (RectWave::*)(float))&RectWave::freq,
					(RectWave& (RectWave::*)(ControlGenerator))&RectWave::freq)
			.addOverloadedFunctions("pwm",
					(RectWave& (RectWave::*)(Generator))&RectWave::pwm,
					(RectWave& (RectWave::*)(float))&RectWave::pwm,
					(RectWave& (RectWave::*)(ControlGenerator))&RectWave::pwm)
					.addOverloadedFunctions("__add",
					(Adder (Generator::*)(float))&Generator::operator+,
					(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
					(Adder (Generator::*)(Generator))&Generator::operator+)
					.addOverloadedFunctions("__sub",
					(Subtractor (Generator::*)(float))&Generator::operator-,
					(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
					(Subtractor (Generator::*)(Generator))&Generator::operator-)
					.addOverloadedFunctions("__mul",
					(Multiplier (Generator::*)(float))&Generator::operator*,
					(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
					(Multiplier (Generator::*)(Generator))&Generator::operator*)
					.addOverloadedFunctions("__div",
					(Divider (Generator::*)(float))&Generator::operator/,
					(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
					(Divider (Generator::*)(Generator))&Generator::operator/));

	state["RectWaveBL"].setClass(
			kaguya::UserdataMetatable<RectWaveBL, Generator>()
			.setConstructors<RectWaveBL()>()
			.addOverloadedFunctions("freq",
					(RectWaveBL& (RectWaveBL::*)(Generator))&RectWaveBL::freq,
					(RectWaveBL& (RectWaveBL::*)(float))&RectWaveBL::freq,
					(RectWaveBL& (RectWaveBL::*)(ControlGenerator))&RectWaveBL::freq)
			.addOverloadedFunctions("pwm",
					(RectWaveBL& (RectWaveBL::*)(Generator))&RectWaveBL::pwm,
					(RectWaveBL& (RectWaveBL::*)(float))&RectWaveBL::pwm,
					(RectWaveBL& (RectWaveBL::*)(ControlGenerator))&RectWaveBL::pwm)
					.addOverloadedFunctions("__add",
					(Adder (Generator::*)(float))&Generator::operator+,
					(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
					(Adder (Generator::*)(Generator))&Generator::operator+)
					.addOverloadedFunctions("__sub",
					(Subtractor (Generator::*)(float))&Generator::operator-,
					(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
					(Subtractor (Generator::*)(Generator))&Generator::operator-)
					.addOverloadedFunctions("__mul",
					(Multiplier (Generator::*)(float))&Generator::operator*,
					(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
					(Multiplier (Generator::*)(Generator))&Generator::operator*)
					.addOverloadedFunctions("__div",
					(Divider (Generator::*)(float))&Generator::operator/,
					(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
					(Divider (Generator::*)(Generator))&Generator::operator/));

	state["Noise"].setClass(
				kaguya::UserdataMetatable<Noise, Generator>()
				.setConstructors<Noise()>()
				.addOverloadedFunctions("__add",
						(Adder (Generator::*)(float))&Generator::operator+,
						(Adder (Generator::*)(ControlGenerator))&Generator::operator+,
						(Adder (Generator::*)(Generator))&Generator::operator+)
						.addOverloadedFunctions("__sub",
						(Subtractor (Generator::*)(float))&Generator::operator-,
						(Subtractor (Generator::*)(ControlGenerator))&Generator::operator-,
						(Subtractor (Generator::*)(Generator))&Generator::operator-)
						.addOverloadedFunctions("__mul",
						(Multiplier (Generator::*)(float))&Generator::operator*,
						(Multiplier (Generator::*)(ControlGenerator))&Generator::operator*,
						(Multiplier (Generator::*)(Generator))&Generator::operator*)
						.addOverloadedFunctions("__div",
						(Divider (Generator::*)(float))&Generator::operator/,
						(Divider (Generator::*)(ControlGenerator))&Generator::operator/,
						(Divider (Generator::*)(Generator))&Generator::operator/));

	state["ControlGenerator"].setClass(
			kaguya::UserdataMetatable<ControlGenerator>()
			.setConstructors<ControlGenerator()>()
			.addFunction("smoothed", &ControlGenerator::smoothed)
			.addFunction("tick", &ControlGenerator::tick)
			.addOverloadedFunctions("__add",
			(ControlAdder (ControlGenerator::*)(float))&ControlGenerator::operator+,
			(ControlAdder (ControlGenerator::*)(ControlGenerator))&ControlGenerator::operator+)
			.addOverloadedFunctions("__sub",
			(ControlSubtractor (ControlGenerator::*)(float))&ControlGenerator::operator-,
			(ControlSubtractor (ControlGenerator::*)(ControlGenerator))&ControlGenerator::operator-)
			.addOverloadedFunctions("__mul",
			(ControlMultiplier (ControlGenerator::*)(float))&ControlGenerator::operator*,
			(ControlMultiplier (ControlGenerator::*)(ControlGenerator))&ControlGenerator::operator*)
			.addOverloadedFunctions("__div",
			(ControlDivider (ControlGenerator::*)(float))&ControlGenerator::operator/,
			(ControlDivider (ControlGenerator::*)(ControlGenerator))&ControlGenerator::operator/)
		);

	state["ControlParameter"].setClass(
			kaguya::UserdataMetatable<ControlParameter,ControlGenerator>()
			.setConstructors<ControlParameter()>()
			.addFunction("getName", &ControlParameter::getName)
			.addFunction("name", &ControlParameter::name)
			.addFunction("getDisplayName", &ControlParameter::getDisplayName)
			.addFunction("displayName", &ControlParameter::displayName)
			.addFunction("getValue", &ControlParameter::getValue)
			.addFunction("value", &ControlParameter::value)
			.addFunction("getMin", &ControlParameter::getMin)
			.addFunction("min", &ControlParameter::min)
			.addFunction("getMax", &ControlParameter::getMax)
			.addFunction("max", &ControlParameter::max)
			.addFunction("getParameterType", &ControlParameter::getParameterType)
			.addFunction("parameterType", &ControlParameter::parameterType)
			.addFunction("getIsLogarithmic", &ControlParameter::getIsLogarithmic)
			.addFunction("logarithmic", &ControlParameter::logarithmic)
			.addFunction("getNormalizedValue", &ControlParameter::getNormalizedValue)
			.addFunction("setNormalizedValue", &ControlParameter::setNormalizedValue)
			.addOverloadedFunctions("__add",
			(ControlAdder (ControlGenerator::*)(float))&ControlGenerator::operator+,
			(ControlAdder (ControlGenerator::*)(ControlGenerator))&ControlGenerator::operator+)
			.addOverloadedFunctions("__sub",
			(ControlSubtractor (ControlGenerator::*)(float))&ControlGenerator::operator-,
			(ControlSubtractor (ControlGenerator::*)(ControlGenerator))&ControlGenerator::operator-)
			.addOverloadedFunctions("__mul",
			(ControlMultiplier (ControlGenerator::*)(float))&ControlGenerator::operator*,
			(ControlMultiplier (ControlGenerator::*)(ControlGenerator))&ControlGenerator::operator*)
			.addOverloadedFunctions("__div",
			(ControlDivider (ControlGenerator::*)(float))&ControlGenerator::operator/,
			(ControlDivider (ControlGenerator::*)(ControlGenerator))&ControlGenerator::operator/));
}


#endif /* TONIC_LUA_HPP_ */
