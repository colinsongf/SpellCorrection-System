#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "MutexLock.h"
#include "Condition.h"
#include <queue>


namespace tanfy
{

class Task;

class Buffer
{
public:
	Buffer(size_t maxsize);
	
	void push(Task *mytask);
	Task *pop();
	void wakeup_empty();

private:
	bool empty();
	bool full();

private:
	MutexLock mutex_;
	Condition notFull_;
	Condition notEmpty_;
	std::queue<Task *> que_;	
	size_t maxsize_;	
	bool ifWakeEmpty_;
};

}//end of namespace



#endif
