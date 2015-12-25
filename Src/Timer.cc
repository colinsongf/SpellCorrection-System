#include "../Inc/Timer.h"
#include <sys/timerfd.h>
#include <poll.h>
#include <unistd.h>
#include <iostream>

namespace tanfy
{

int createTimer()
{
	int timerfd = timerfd_create(CLOCK_REALTIME, 0);
	if(-1 == timerfd)
	{
		perror("create timer error");
		exit(EXIT_FAILURE);
	}
//	std::cout << "create timer success" << std::endl; //test	
	return timerfd;
}

void setTimer(int timerfd, time_t interval, time_t val)
{
	struct itimerspec new_value;
	new_value.it_interval.tv_sec = interval;
	new_value.it_interval.tv_nsec = 0;
	new_value.it_value.tv_sec = val;
	new_value.it_value.tv_nsec = 0;

	int ret = timerfd_settime(timerfd, 0, &new_value, NULL);	
	if(-1 == ret)
	{
		perror("set timer error");
		exit(EXIT_FAILURE);
	}
}

void stopTimer(int timerfd)
{
	setTimer(timerfd, 0, 0);
}

ssize_t readTimer(int timerfd)
{
	size_t howmany;
	ssize_t readnum = ::read(timerfd, &howmany, sizeof(howmany));
	if(readnum != sizeof(howmany))
	{
		perror("read timer error");
		exit(EXIT_FAILURE);
	}
	return howmany;
}




//====================================
//class Timer
//====================================
Timer::Timer(TimerCallback cb, time_t interval, time_t val)
	:cb_(cb),
	 timerfd_(tanfy::createTimer()),
	 interval_(interval),
	 val_(val),
	 isRunning_(false)
	
{}

Timer::~Timer()
{
	std::cout << "~Timer()" << std::endl;
	if(isRunning_)
		stop();
}

void Timer::start()
{
//	std::cout << "Timer::start()" << std::endl; //test
	tanfy::setTimer(timerfd_, interval_, val_);

	isRunning_ = true;
	
	struct pollfd pfd;
	pfd.fd = timerfd_;
	pfd.events = POLLIN;

	while(isRunning_)
	{
		int ret = poll(&pfd, 1, 5000);
		if(-1 == ret)
		{
			if(errno == EINTR)
				continue;
			else
			{
				perror("poll error");
				exit(EXIT_FAILURE);
			}
		}
		else if(0 == ret)
		{
		//	std::cout << "poll wait timeout" << std::endl; //test
		}
		else
		{
			if(pfd.revents == POLLIN)
			{	
				readTimer(timerfd_);
				cb_();
			}						
		}
	}	
}

void Timer::stop()
{
	std::cout << "Timer::stop()" << std::endl;
	stopTimer(timerfd_);
	cb_();
	isRunning_ = false;
}

















}//end of namespace
