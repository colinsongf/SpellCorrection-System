#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "Noncopyable.h"
#include <pthread.h>


namespace  tanfy
{

class MutexLock;

class Condition:Noncopyable
{
public:
	Condition(MutexLock &mutex);
	~Condition();
	
	void wait();
	void notify();
	void notifyall();	

private:
	MutexLock &mutex_;
	pthread_cond_t cond_;
};

} // end of namespace


#endif
