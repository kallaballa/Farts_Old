#include "Arduino.h"
#include "MIDI.h"
#include "LiquidCrystal.h"
#include "Bounce.h"
#include <map>
#include <vector>
#include <limits>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

typedef double floating_t;

size_t maxModes = 3;
size_t currentMode = 0;
//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//LiquidCrystal LCD(rs, en, d4, d5, d6, d7);

Bounce prevButton = Bounce(11, 10);
Bounce nextButton = Bounce(12, 10);

constexpr floating_t UPDATE_FREQ = 100000;
size_t TICK = 0;

class Signal {
public:
	floating_t updateFreq_;
	floating_t freq_;

	Signal(floating_t updateFreq, floating_t freq) :
			updateFreq_(updateFreq), freq_(freq) {
	}

	virtual ~Signal() {
	}

	virtual size_t next(floating_t i) const = 0;
};
class LSawtooth: public Signal {
public:
	LSawtooth(floating_t updateFreq, floating_t freq) :
			Signal(updateFreq, freq) {
	}

	virtual size_t next(floating_t i) const {
		return round(255.0 - (255.0 / (updateFreq_ / freq_) * i));
	}
};

class RSawtooth: public Signal {
public:
	RSawtooth(floating_t updateFreq, floating_t freq) :
			Signal(updateFreq, freq) {
	}

	virtual size_t next(floating_t i) const {
		return round(255.0 / (updateFreq_ / freq_) * i);
	}
};

class Triangle: public Signal {
public:
	Triangle(floating_t updateFreq, floating_t freq) :
			Signal(updateFreq, freq) {
	}

	virtual size_t next(floating_t i) const {
		if (i < (size_t) round((updateFreq_ / freq_) / 2.0))
			return round((512.0 / (updateFreq_ / freq_) * i));
		else
			return round(512.0 - (512.0 / (updateFreq_ / freq_) * i));
	}
};

class Sine: public Signal {
public:
	Sine(floating_t updateFreq, floating_t freq) :
			Signal(updateFreq, freq) {
	}

	virtual size_t next(floating_t i) const {
		return 255.0 * ((sin((M_PI * 2) / (updateFreq_ / freq_) * i) + 1.0) / 2.0);
	}
};
class Square: public Signal {
public:
	Square(floating_t updateFreq, floating_t freq) :
			Signal(updateFreq, freq) {
	}

	virtual size_t next(floating_t i) const {
		if (i < 0.5)
			return 255;
		else
			return 0;
	}
};

struct ToneDescriptor {
	floating_t freq_;
	floating_t updateFreq_;
	floating_t lsaw_;
	floating_t rsaw_;
	floating_t triangle_;
	floating_t sine_;
	floating_t square_;
	floating_t lsawPhase_;
	floating_t rsawPhase_;
	floating_t trianglePhase_;
	floating_t sinePhase_;
	floating_t squarePhase_;
	floating_t attack_;
	floating_t release_;
	size_t writes_;
};

constexpr size_t MAX_ATTACK = 100000;
constexpr size_t MAX_RELEASE = 100000;
class Tone {
private:
	ToneDescriptor desc_;

	LSawtooth lst_;
	RSawtooth rst_;
	Triangle tr_;
	Sine sn_;
	Square sq_;

	size_t tick_ = 0;
	floating_t endAttack_ = 0;
	floating_t startRelease_ = std::numeric_limits<floating_t>().max();
	floating_t releaseDuration_ = 0;
public:
	Tone() :
			lst_(0, 0), rst_(0, 0), tr_(0, 0), sn_(0, 0), sq_(0, 0) {
	}

	Tone(ToneDescriptor& desc) :
			desc_(desc), lst_(desc.updateFreq_, desc.freq_), rst_(desc.updateFreq_, desc.freq_), tr_(desc.updateFreq_,
					desc.freq_), sn_(desc.updateFreq_, desc.freq_), sq_(desc.updateFreq_, desc.freq_), endAttack_(
					MAX_ATTACK * desc_.attack_), releaseDuration_(MAX_RELEASE * desc.release_) {
	}

	Tone& operator=(const Tone& other) {
		desc_ = other.desc_;
		lst_ = LSawtooth(other.desc_.updateFreq_, other.desc_.freq_);
		rst_ = RSawtooth(other.desc_.updateFreq_, other.desc_.freq_);
		tr_ = Triangle(other.desc_.updateFreq_, other.desc_.freq_);
		sn_ = Sine(other.desc_.updateFreq_, other.desc_.freq_);
		sq_ = Square(other.desc_.updateFreq_, other.desc_.freq_);
		endAttack_ = other.endAttack_;
		releaseDuration_ = other.releaseDuration_;
		return *this;
	}

	void startRelease() {
		startRelease_ = tick_;
	}

	off_t next(size_t i) {
		floating_t attackCoef = (
				(tick_ < endAttack_ && endAttack_ < startRelease_) ?
						((floating_t) tick_ / ((floating_t) endAttack_ + 1.0)) : 1.0);

		floating_t releaseCoef = (
				(tick_ >= startRelease_) ? (releaseDuration_ - (tick_ - startRelease_)) / releaseDuration_ : 1.0);
		floating_t sig0 = desc_.lsaw_
				* (lst_.next(desc_.writes_ / (round(i + (desc_.writes_ * desc_.lsawPhase_)) % desc_.writes_)));
		floating_t sig1 = desc_.rsaw_
				* (rst_.next(desc_.writes_ / (round(i + (desc_.writes_ * desc_.rsawPhase_)) % desc_.writes_)));
		floating_t sig2 = desc_.triangle_
				* (tr_.next(desc_.writes_ / (round(i + (desc_.writes_ * desc_.trianglePhase_)) % desc_.writes_)));
		floating_t sig3 = desc_.sine_
				* (sn_.next(desc_.writes_ / (round(i + (desc_.writes_ * desc_.sinePhase_)) % desc_.writes_)));
		floating_t sig4 = desc_.square_
				* (sq_.next(desc_.writes_ / (round(i + (desc_.writes_ * desc_.squarePhase_)) % desc_.writes_)));
		floating_t sig5 = (sig0 + sig1 + sig2 + sig3 + sig4) * releaseCoef * attackCoef;

		++tick_;
		if (tick_ >= (std::numeric_limits<size_t>().max() - 10))
			tick_ = 0;

		if (releaseCoef < 0.1)
			return -1;

		return round(sig5);
	}
};

std::map<floating_t, Tone> tones_map;
floating_t vol0 = 0;
floating_t vol1 = 0;
floating_t vol2 = 0;
floating_t vol3 = 1.0;
floating_t vol4 = 0;

floating_t phase0 = 0;
floating_t phase1 = 0;
floating_t phase2 = 0;
floating_t phase3 = 0;
floating_t phase4 = 0;

floating_t attack = 0;
floating_t release = 0;

void handleNoteOn(byte inChannel, byte inNumber, byte inVelocity) {
	digitalWrite(13, HIGH);

	floating_t f = pow(2.0, (inNumber - 69.0) / 12.0) * 440;
//	Serial.print(inNumber);
//	Serial.print(" ");
//	Serial.println(f);

	Serial.println(currentMode);

	if (currentMode == 0) {
		vol0 = analogRead(A4) / 1023.0;
		vol1 = analogRead(A3) / 1023.0;
		vol2 = analogRead(A2) / 1023.0;
		vol3 = analogRead(A1) / 1023.0;
		vol4 = analogRead(A0) / 1023.0;
	} else if (currentMode == 1) {
		phase0 = analogRead(A4) / 1023.0;
		phase1 = analogRead(A3) / 1023.0;
		phase2 = analogRead(A2) / 1023.0;
		phase3 = analogRead(A1) / 1023.0;
		phase4 = analogRead(A0) / 1023.0;
	} else if (currentMode == 2) {
		attack = analogRead(A4) / 1023.0;
		release = analogRead(A3) / 1023.0;
	} else
		Serial.println("Invalid Mode");

	ToneDescriptor desc;
	desc.freq_ = f;
	desc.updateFreq_ = UPDATE_FREQ;
	desc.lsaw_ = vol0;
	desc.rsaw_ = vol1;
	desc.triangle_ = vol2;
	desc.sine_ = vol3;
	desc.square_ = vol4;
	desc.lsawPhase_ = phase0;
	desc.rsawPhase_ = phase1;
	desc.trianglePhase_ = phase2;
	desc.sinePhase_ = phase3;
	desc.squarePhase_ = phase4;
	desc.attack_ = attack;
	desc.release_ = release;
	desc.writes_ = round(UPDATE_FREQ / f);
	tones_map[f] = Tone(desc);
}

void handleNoteOff(byte inChannel, byte inNumber, byte inVelocity) {
	floating_t f = pow(2.0, (inNumber - 69.0) / 12.0) * 440;
	if (tones_map.find(f) != tones_map.end())
		tones_map[f].startRelease();
	digitalWrite(13, LOW);
}

void setup() {

//  LCD.begin(16, 2);
//  LCD.print("hello, world!");
	analogWriteFrequency(1, 750000);
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);
	MIDI.setHandleNoteOff(handleNoteOff);
	MIDI.setHandleNoteOn(handleNoteOn);
	MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
	if (prevButton.update()) {
		Serial.println("update1");
		if (prevButton.fallingEdge()) {
			if (currentMode == 0)
				currentMode = maxModes - 1;
			else
				--currentMode;
		}
	} else if (nextButton.update()) {
		Serial.println("update2");

		if (nextButton.fallingEdge()) {
			if (currentMode == (maxModes - 1))
				currentMode = 0;
			else
				++currentMode;
		}
	}
	MIDI.read();
	unsigned long start = micros();

	size_t numTones = tones_map.size();
	floating_t total = 0;
	size_t val = ++TICK;

	std::vector<floating_t> deleteMe;
	for (auto& p : tones_map) {
		off_t next = p.second.next(val);
		if (next == -1) {
			deleteMe.push_back(p.first);
			continue;
		}

		total += next;
	}

	for (auto& f : deleteMe) {
		tones_map.erase(f);
	}

	size_t pulseWidth;

	if (numTones == 0)
		pulseWidth = 0;
	else
		pulseWidth = round(((floating_t )total));

	analogWrite(1, pulseWidth);

	floating_t duration = micros() - start;
	floating_t interval = 1000000 / UPDATE_FREQ;
	if (duration > interval) {
		Serial.println("Overrun");
	} else {
		delayMicroseconds(round(interval - duration));
	}

	if (TICK > 10000000)
		TICK = 0;
}
