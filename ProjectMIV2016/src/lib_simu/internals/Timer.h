
#ifndef TIMER_H
#define TIMER_H

/*! \class OSTimer
 * \brief Mutli-platform microsecond accurate timer defintions
 */
class OSTimer {

public:

	/*!
	 * \brief Starts or restarts the timer
	 */
	virtual void restart() = 0;

	/*!
	 * \brief Returns, in milliseconds the time elapsed since the last call to restart()
	 */
	virtual float elapsedTime() = 0;

};

#include "TimerWindows.h"
#include "TimerApple.h"
#include "TimerLinux.h"

#ifdef WIN32
typedef TimerWindows	Timer;
#else
#ifdef __APPLE_CC__
typedef TimerApple		Timer;
#else
typedef TimerLinux		Timer;
#endif
#endif

#endif //TIMER_H
