#ifndef __TASKTHREAD_H__
#define __TASKTHREAD_H__

#include "Thread.h"

namespace tanfy
{

class ThreadPool;

class TaskThread :public Thread
{
public:
	TaskThread(ThreadPool &thrpool);

protected:
	void run();

private:
	ThreadPool &thrpool_;

};


}//end of namespace


#endif
