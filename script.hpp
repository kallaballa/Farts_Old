#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

const char * BL_OSC_TEST = R"SCRIPTY( 
  local blend = synth:addParameter("blend"):min(0):max(1);
	local freqSweep = SineWave():freq(0.25) * 1000 + 1080;
	local smoothBlend = blend:smoothed();
	local output = (SineWave():freq(freqSweep) * (FixedValue(1.0) - smoothBlend)) + (SineWave():freq(freqSweep) * smoothBlend);
	synth:setOutputGen(output)
 )SCRIPTY";




#endif /* SCRIPT_HPP_ */

