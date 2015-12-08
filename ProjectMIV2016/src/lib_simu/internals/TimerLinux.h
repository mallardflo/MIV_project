#ifndef TIMER_LINUX_H
#define TIMER_LINUX_H

#ifdef LINUX

#include "Timer.h"

class TimerLinux : public OSTimer {

public:

	TimerLinux::TimerLinux() {

	}

	virtual void restart() {

	}

	//in milliseconds
	virtual float elapsedTime() {


		//deduce time in milliseconds
		return 0.0f;
	}

};

#endif

#endif
