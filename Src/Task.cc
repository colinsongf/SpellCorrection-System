#include "../Inc/Task.h"
#include "../Inc/ThreadPool.h"
#include "../Inc/TcpConnection.h"
#include <string>


namespace tanfy
{

Task::Task(ThreadPool &thrpool, TcpConnection *tcpConn, std::string recvword)
	:thrpool_(thrpool),
	 tcpConn_(tcpConn),
	 pth_(NULL),
	 recvword_(recvword),
	 sendword_(std::string())
{}

void Task::addToThreadpool()
{
	thrpool_.addTask(this);
}

void Task::sendToClient()
{
	tcpConn_->sendmsg(sendword_);
}

void Task::setThread(Thread *pth)
{
	pth_ = pth;
}

}//end of namespace
