#include <map>
#include <limits>

typedef double floating_t;


String IN_DATA;
bool STATE = false;
floating_t FREQ = 440;
floating_t UPDATE_FREQ = 300000;

constexpr size_t NUM__OUTPUTS = 4;

class Signal {
  public:
    floating_t updateFreq_;
    floating_t freq_;

    Signal(floating_t updateFreq, floating_t freq) : updateFreq_(updateFreq), freq_(freq) {
    }

    virtual size_t next(size_t i) const = 0;
};
class LSawtooth : public Signal {
  public:
    LSawtooth(floating_t updateFreq, floating_t freq) : Signal(updateFreq, freq) {
    }

    virtual size_t next(size_t i) const {
      return round(255.0 - (255.0 / (updateFreq_ / freq_) * i));
    }
};

class RSawtooth : public Signal {
  public:
    RSawtooth(floating_t updateFreq, floating_t freq) : Signal(updateFreq, freq) {
    }

    virtual size_t next(size_t i) const {
      return round(255.0 / (updateFreq_ / freq_) * i);
    }
};

class Triangle : public Signal {
  public:
    Triangle(floating_t updateFreq, floating_t freq) : Signal(updateFreq, freq) {
    }

    virtual size_t next(size_t i) const {
      if (i < (size_t)round((updateFreq_ / freq_) / 2.0))
        return round((512 / (updateFreq_ / freq_) * i));
      else
        return round(512.0 - (512.0 / (updateFreq_ / freq_) * i));
    }
};

class Sine : public Signal {
  public:
    Sine(floating_t updateFreq, floating_t freq) : Signal(updateFreq, freq) {
    }

    virtual size_t next(size_t i) const {
      return 255.0 * ((sin((M_PI * 2) / (updateFreq_ / freq_) * i) + 1.0) / 2.0);
    }
};
class Square : public Signal {
  public:
    Square(floating_t updateFreq, floating_t freq) : Signal(updateFreq, freq) {
    }

    virtual size_t next(size_t i) const {
      if (i < (size_t)round((updateFreq_ / freq_) / 2.0))
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

constexpr size_t MAX_ATTACK = 500000;
constexpr size_t MAX_RELEASE = 500000;
class Tone {
  private:
    ToneDescriptor desc_;

    LSawtooth lst_;
    RSawtooth rst_;
    Triangle tr_;
    Sine sn_;
    Square sq_;

    size_t tick_ = 0;
    size_t endAttack_ = 0;
    size_t startrelease_ = std::numeric_limits<size_t>().max();
  public:
    Tone() : lst_(UPDATE_FREQ, FREQ),
      rst_(UPDATE_FREQ, FREQ),
      tr_(UPDATE_FREQ, FREQ),
      sn_(UPDATE_FREQ, FREQ),
      sq_(UPDATE_FREQ, FREQ)  {
    }
    
    Tone(ToneDescriptor& desc) :
      desc_(desc),
      lst_(desc.updateFreq_, desc.freq_),
      rst_(desc.updateFreq_, desc.freq_),
      tr_(desc.updateFreq_, desc.freq_),
      sn_(desc.updateFreq_, desc.freq_),
      sq_(desc.updateFreq_, desc.freq_),
      endAttack_(MAX_ATTACK * desc_.attack_),
      startrelease_(MAX_RELEASE * desc_.release_) {
    }

    Tone& operator=(const Tone& other) {
      desc_ = other.desc_;
      lst_ = LSawtooth(other.desc_.updateFreq_, other.desc_.freq_);
      rst_ = RSawtooth(other.desc_.updateFreq_, other.desc_.freq_);
      tr_ = Triangle(other.desc_.updateFreq_, other.desc_.freq_);
      sn_ = Sine(other.desc_.updateFreq_, other.desc_.freq_);
      sq_ = Square(other.desc_.updateFreq_, other.desc_.freq_);
      endAttack_ = other.endAttack_;
      startrelease_ = other.startrelease_;

      return *this;
    }

    void startRelease() {
      startrelease_ = tick_;
    }
    
    off_t next(size_t i) {
      floating_t attackCoef = (tick_ < endAttack_ && endAttack_ > startrelease_ ? ((double)tick_/((double)endAttack_ + 1.0)) : 1.0);
      floating_t releaseCoef = (tick_ > startrelease_? ((double)startrelease_/((double)tick_ + 1.0)) : 1.0);
      floating_t sig0 = attackCoef * releaseCoef * desc_.lsaw_ * (lst_.next((size_t)(i + (desc_.writes_ * desc_.lsawPhase_)) % desc_.writes_));
      floating_t sig1 = attackCoef * releaseCoef * desc_.rsaw_ * (rst_.next((size_t)(i + (desc_.writes_ * desc_.rsawPhase_)) % desc_.writes_));
      floating_t sig2 = attackCoef * releaseCoef * desc_.triangle_ * (tr_.next((size_t)(i + (desc_.writes_ * desc_.trianglePhase_)) % desc_.writes_));
      floating_t sig3 = attackCoef * releaseCoef * desc_.sine_ * (sn_.next((size_t)(i + (desc_.writes_ * desc_.sinePhase_)) % desc_.writes_));
      floating_t sig4 = attackCoef * releaseCoef * desc_.square_ * (sq_.next((size_t)(i + (desc_.writes_ * desc_.squarePhase_)) % desc_.writes_));
      floating_t sig5 = (sig0 + sig1 + sig2 + sig3 + sig4) / 5.0;
      ++tick_;
      if(tick_ >= 10000000)
        tick_ = 0;

      if(releaseCoef < 0.1)
        return -1;

      return sig5;
    }
};

void setup() {
  analogWriteFrequency(0, 750000);
  analogWriteFrequency(1, 750000);
  analogWriteFrequency(2, 750000);
  analogWriteFrequency(3, 750000);
  analogWriteFrequency(4, 750000);
  analogWriteFrequency(5, 750000);
  IN_DATA = "";

}

std::map<floating_t, Tone> tones;

size_t TICK = 0;

void loop() {
  floating_t phase0 = analogRead(A0) / 1023.0;
  floating_t phase1 = analogRead(A1) / 1023.0;
  floating_t phase2 = analogRead(A2) / 1023.0;
  floating_t phase3 = analogRead(A3) / 1023.0;
  floating_t phase4 = analogRead(A4) / 1023.0;

  floating_t vol0 = analogRead(A5) / 1023.0;
  floating_t vol1 = analogRead(A6) / 1023.0;
  floating_t vol2 = analogRead(A7) / 1023.0;
  floating_t vol3 = analogRead(A8) / 1023.0;
  floating_t vol4 = analogRead(A9) / 1023.0;

  vol0 = 0;
  vol1 = 0;
  vol2 = 0;
  vol3 = 0.3;
  vol4 = 0;

  while (Serial.available() > 0)
  {
    char received = Serial.read();
    IN_DATA += received;

    // Process message when new line character is recieved
    if (received == '\n')
    {
      Serial.print("Arduino Received: ");
      Serial.print(IN_DATA);
      FREQ = IN_DATA.toInt();
      IN_DATA = "";

      if (tones.find(FREQ) != tones.end()) {
        ToneDescriptor desc;
        desc.freq_ = FREQ;
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
        desc.attack_ = 1;
        desc.release_ = 1;
        desc.writes_ = round(UPDATE_FREQ / FREQ);
        tones[FREQ] = Tone(desc);
      } else {
        ToneDescriptor desc;
        desc.freq_ = FREQ;
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
        desc.attack_ = 1;
        desc.release_ = 1;
        desc.writes_ = round(UPDATE_FREQ / FREQ);

        tones[FREQ] = Tone(desc);
      }
    }
  }

  for (size_t i = 0; i < 1000; ++i) {
    size_t numTones = tones.size();
    floating_t total = 0;
    size_t val = ++TICK;
    for (auto& p : tones) {
      off_t next = p.second.next(val);
      if(next == -1)  {
        tones.erase(p.first);
        continue;
      }

      total += next;
    }
    if(numTones == 0)
      analogWrite(0, 0);
    else
      analogWrite(0, total / numTones);
  }

  STATE = !STATE;
  delayMicroseconds(1000000 / UPDATE_FREQ);
  if(TICK > 10000000)
    TICK = 0;
  ;
}
