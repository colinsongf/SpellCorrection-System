#ifndef __TASK_H__
#define __TASK_H__

#include <string>


namespace tanfy
{

class ThreadPool;

class TcpConnection;

class Thread;

class Task
{
public:
	Task(ThreadPool &thrpool, TcpConnection *tcpConn, std::string recvword);

	void addToThreadpool(); //将任务丢入线程池中
	void sendToClient(); //将结果传给客户端
	void setThread(Thread *pth);//线程取任务后，保存该线程；	
	virtual void process() = 0; //线程池中处理任务：由子线程完成

protected:
	std::string recvword_;//源数据
	std::string sendword_;//处理完成数据
	Thread *pth_;//执行此任务的线程
	ThreadPool &thrpool_;
	TcpConnection *tcpConn_;//进行此任务的客户端连接
};

}//end of namespace

#endif
