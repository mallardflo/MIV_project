#ifndef TIMER_APPLE_H
#define TIMER_APPLE_H

#ifdef __APPLE_CC__

#include "Timer.h"

class TimerApple : public OSTimer {

public:

	TimerApple::TimerApple() {

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
