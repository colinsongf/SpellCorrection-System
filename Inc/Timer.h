#ifndef __TIMER_H__
#define __TIMER_H__

#include <functional>
#include <sys/timerfd.h>


namespace tanfy
{


class Timer
{
public:
	typedef std::function<void()> TimerCallback;
		
	Timer(TimerCallback cb, time_t interval, time_t val);
	~Timer();
	void start();
	void stop();

private:
	int timerfd_;
	time_t interval_;
	time_t val_;
	bool isRunning_;
	TimerCallback cb_;
};


}//end of namespace





#endif
