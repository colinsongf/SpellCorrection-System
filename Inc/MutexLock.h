#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__

#include "Noncopyable.h"
#include <pthread.h>

namespace tanfy
{

class MutexLock:Noncopyable
{
public:
	MutexLock();
	~MutexLock();

	void lock();
	void unlock();
	pthread_mutex_t *getMutexPtr();

private:
	pthread_mutex_t mutex_;	
};

}//end of namespace

#endif
