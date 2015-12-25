#ifndef __TIMERTHREAD_H__
#define __TIMERTHREAD_H__

#include "Timer.h"
#include "Thread.h"
#include <functional>
#include <time.h>


namespace tanfy
{


class TimerThread: public Thread
{
public:
	typedef std::function<void()> TimerThrCallback;

	TimerThread(TimerThrCallback cb, time_t interval, time_t val);
		
	void join();

protected:
	void run();

private:
	Timer timer_;
};


}//end of namespace







#endif
