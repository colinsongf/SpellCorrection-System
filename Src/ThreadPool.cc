#include "../Inc/ThreadPool.h"
#include "../Inc/TaskThread.h"
#include "../Inc/Thread.h"
#include "../Inc/Buffer.h"
#include "../Inc/Task.h"
#include <iostream>
#include <vector>


namespace tanfy
{

ThreadPool::ThreadPool(ssize_t ThreadNum, ssize_t maxsize, Dict &mydict, Conf &myconf, Index &myindex)
	:buf_(maxsize),
	 ThreadNum_(ThreadNum),
	 isRunning_(false),
	 cacheUpdating_(false),
	 mydict_(mydict),
	 myindex_(myindex),
	 cachepool_(myconf)
{
}

ThreadPool::~ThreadPool()
{
	std::cout << "~ThreadPool()" << std::endl; //test
	if(isRunning_)
	{
		stop();
	}
	isRunning_ = false;
}

void ThreadPool::start()
{
	isRunning_ = true;
	
	size_t idx;
	for(idx = 0; idx < ThreadNum_; ++idx)
	{
		Thread *pth = new TaskThread(*this);
		pth->getCache().setMap(cachepool_);
		pth->start();
		vecThread_.push_back(pth);	
	}
}

void ThreadPool::stop()
{
	isRunning_ = false;
	
	buf_.wakeup_empty();
	std::vector<Thread *>::iterator ite = vecThread_.begin();
	for(; ite != vecThread_.end(); ++ite)
	{
		(*ite)->join();
		delete (*ite);
	}
	vecThread_.clear();
}

void ThreadPool::addTask(Task *mytask)
{
	buf_.push(mytask);
}

Task *ThreadPool::getTask()
{
	return buf_.pop();
}

void ThreadPool::ThreadFunc(Thread *pth)
{
	while(isRunning_)
	{
		if(!cacheUpdating_) //未处理cache时
		{
			Task *mytask = getTask();
			if(mytask)
			{
				mytask->setThread(pth);
				mytask->process();
			}
		}	
	}
}

tanfy::Dict &ThreadPool::getDict()
{
	return mydict_;
}

tanfy::Index &ThreadPool::getIndex()
{
	return myindex_;
}


void ThreadPool::updateCache()
{
	std::cout << "//===========================" << std::endl;
	std::cout << "//0.0 正在更新服务器数据..." << std::endl; 
	cacheUpdating_ = true;
	if(cacheUpdating_)
	{
		std::vector<Thread *>::iterator ite = vecThread_.begin();
		for(; ite != vecThread_.end(); ++ite)
		{
			cachepool_.update((*ite)->getCache());
		}
		
		cachepool_.writetoFile();
		for(ite = vecThread_.begin(); ite != vecThread_.end(); ++ite)
		{
			(*ite)->getCache().setMap(cachepool_);
		}	
	}
#if 0
	//test
	std::vector<Thread *>::iterator ite;
	for(ite = vecThread_.begin(); ite != vecThread_.end(); ++ite)
	{
		std::cout << "Cache is thread: " << std::endl;
		(*ite)->getCache().displayMap();
		std::cout << std::endl;
	}
	std::cout << "Cache in pool:" << std::endl;
	cachepool_.displayMap();
	exit(EXIT_FAILURE);
#endif

	cacheUpdating_ = false;
	std::cout << "//0.0 更新完成~" << std::endl; //test
	std::cout << "//===========================" << std::endl;
}













}//end of namespace
