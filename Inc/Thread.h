#ifndef __THREAD_H__
#define __THREAD_H__

#include "Noncopyable.h"
#include "Cache.h"
#include <pthread.h>


namespace tanfy
{

class Thread:Noncopyable
{
public:
	Thread();
	virtual ~Thread();

	void start();
	virtual void join();
	Cache& getCache()
	{	
		return cachethr_;
	}
	
protected:
	virtual void run() = 0;

private:
	static void* ThreadFunc(void *arg);

private:
	pthread_t pthid_;
	bool isRunning_;	
	Cache cachethr_;
};

}//end of namespace






#endif
