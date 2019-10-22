#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

constexpr const char * BL_OSC_TEST = R"_LUA_SCRIPT_( 
  local blend = synth:addParameter("blend"):min(0):max(1);
	local freqSweep = SineWave():freq(0.25) * 1000 + 1080;
	local smoothBlend = blend:smoothed();
	local output = (SineWave():freq(freqSweep) * (FixedValue(1.0) - smoothBlend)) + (SineWave():freq(freqSweep) * smoothBlend);
	synth:setOutputGen(output)
 )_LUA_SCRIPT_";

constexpr const char * COMPR_DUCK_TEST = R"_LUA_SCRIPT_( 
		local hpNoise = LPF12():cutoff(8000):input(HPF24():cutoff(2000.0):input((Noise() * dBToLin(-18.0))))
		print(hpNoise)
  local tones = SineWave():freq(180):mul(dBToLin(-6.0)):add(SineWave():freq(332):mul(dBToLin(-18.0)));
	print(tones)
    --local hpNoise = stream(stream((Noise() * dBToLin(-18.0)),HPF24():cutoff(2000.0)),LPF12():cutoff(8000));
		--print(hpNoise);
 )_LUA_SCRIPT_";
#endif /* SCRIPT_HPP_ */

