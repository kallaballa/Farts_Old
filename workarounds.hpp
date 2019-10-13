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

pid_t _getpid(void) {
	return 1;
}

int _gettimeofday(struct timeval *tv, struct timezone *tz) {
	return 0;
}
}

#endif /* WORKAROUNDS_HPP_ */
