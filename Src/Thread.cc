#include "../Inc/Thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <iostream>


namespace tanfy
{

Thread::Thread()
	:pthid_(0),
	 isRunning_(false)
{
}

Thread::~Thread()
{
	std::cout << "~Thread()" <<std::endl;//test
	if(isRunning_)
	{
		join();
		isRunning_ = false;
	}	
}

void Thread::start()
{
	int ret = pthread_create(&pthid_, NULL, ThreadFunc, this);
	if(ret != 0)
	{
		perror("Thread start error");
		exit(EXIT_FAILURE);
	}
	isRunning_ = true;
}

void Thread::join()
{
	std::cout << "Thread::join()" << std::endl;
	int ret = pthread_join(pthid_, NULL);	
	if(ret != 0)
	{
		perror("Thread join error");
		exit(EXIT_FAILURE);
	}
	isRunning_ = false;
}

void *Thread::ThreadFunc(void *arg)
{
	Thread *pth = static_cast<Thread *>(arg);
	if(pth)
	{
		pth->run();
	}
}


}//end of namespace
