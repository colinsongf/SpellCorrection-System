#include "../Inc/TaskThread.h"
#include "../Inc/ThreadPool.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


//此类仍然要改动
namespace tanfy
{

TaskThread::TaskThread(ThreadPool &thrpool)
	:thrpool_(thrpool)
{	
}

void TaskThread::run()
{
#if 0
	::srand(::time(NULL));
	int num, count = 20;
	while(count --)
	{
		num = ::rand()%100;
		std::cout << "product a num = " << num << std::endl;
		::sleep(1);
	}	
#endif
	thrpool_.ThreadFunc(this);
}



}//end of namespace
