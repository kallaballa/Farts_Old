#ifndef WORKAROUNDS_HPP_
#define WORKAROUNDS_HPP_

#include <unistd.h>
//workaround for linker error
unsigned __exidx_start;
unsigned __exidx_end;

//another linker workaround -.-
extern "C" {
int _kill(pid_t pid, int signum) {
	return 0;
}

int _unlink(
   const char *filename
) {
	return 0;
};
int _open(
   const char *filename,
   int oflag,
   int pmode
) {
	return 0;
}

pid_t _getpid(void) {
	return 1;
}

int _gettimeofday(struct timeval *tv, struct timezone *tz) {
	return 0;
}
}

extern "C" {
void lua_compat_printf(char *fmt, ...) {
	Serial.print("SPF:");
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 128, fmt, args);
	va_end(args);
	Serial.print(buf);
}

void lua_compat_print(const char *s) {
	Serial.print(s);
}
}
#ifdef TEENSYDUINO
#include <TimeLib.h>

time_t getTeensy3Time() {
	return Teensy3Clock.get();
}
#endif

#endif /* WORKAROUNDS_HPP_ */
