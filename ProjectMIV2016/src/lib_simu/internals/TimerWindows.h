
#ifndef TIMER_WINDOWS_H
#define TIMER_WINDOWS_H

#ifdef WIN32

#include "Timer.h"
#include <windows.h>

class TimerWindows : public OSTimer {

	LARGE_INTEGER ticksPerSecond;
	LARGE_INTEGER tick;   // A point in time
	LARGE_INTEGER time_start;   // For converting tick into real time

public:

	TimerWindows::TimerWindows() {
		// get the high resolution counter's accuracy
		QueryPerformanceFrequency(&ticksPerSecond);
	}

	virtual void restart() {
		// what time is it?
		QueryPerformanceCounter(&time_start);
	}

	//in milliseconds
	virtual float elapsedTime() {
		// what time is it?
		QueryPerformanceCounter(&tick);

		//deduce time in milliseconds
		return ((float)(tick.QuadPart - time_start.QuadPart)*1000.0f/(float)ticksPerSecond.QuadPart);
	}

};

#endif

#endif
