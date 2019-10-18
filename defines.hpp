#ifndef DEFINES_HPP_
#define DEFINES_HPP_

typedef uint64_t sample_t;
typedef double floating_t;

constexpr uint64_t SAMPLE_MAX = std::numeric_limits<uint32_t>::max();
constexpr uint64_t SAMPLE_MAX_2 = SAMPLE_MAX / 2;

constexpr floating_t CLOCK_FREQ = 50000;
constexpr size_t RING_BUFFER_SIZE = 32;
constexpr size_t MAX_INSTRUMENTS = 7;
constexpr size_t MAX_MODES = 14;

#endif /* DEFINES_HPP_ */
