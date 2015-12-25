#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <vector>
#include <stdio.h>
#include "Buffer.h"
#include "Noncopyable.h"
#include "Cache.h"


namespace tanfy
{

class Thread;
class Task;
class Dict;
class Conf;
class Index;

class ThreadPool: Noncopyable
{
public:
	friend class TaskThread;
	
	ThreadPool(ssize_t ThreadNum, ssize_t maxsize, Dict &mydict, Conf &myconf, Index &myindex);
	~ThreadPool();

	void start();
	void stop();
	void addTask(Task *mytask);
	Dict &getDict();
	Index &getIndex();
	void updateCache();

private:
	Task *getTask();	
	void ThreadFunc(Thread *pth);

private:
	Buffer buf_;
	size_t ThreadNum_;
	bool isRunning_;
	bool cacheUpdating_;
	std::vector<Thread *> vecThread_;
	Dict &mydict_;
	Index &myindex_;
	Cache cachepool_;
};



}//end of tanfy






#endif
