#include "../Inc/TimerThread.h"
#include <iostream>


namespace tanfy
{

TimerThread::TimerThread(TimerThrCallback cb, time_t interval, time_t val)
	:timer_(cb, interval, val),
	 Thread()
{
}

void TimerThread::run()
{
//	std::cout << "start in timerthread" << std::endl; //test
	timer_.start();	
}

void TimerThread::join()
{
	timer_.stop();
	Thread::join();//不知道这样可行否。。。可行！
}


}//end of namespace
