#include "../Inc/TcpServer.h"
#include <iostream>


namespace tanfy
{

TcpServer::TcpServer(ThreadPool &thrpool, unsigned short port)
	:seraddr_(port),
	 sockfd_(),
	 epollfd_(sockfd_.getfd(), thrpool)
{
	sockfd_.ready(seraddr_, 10);
	isRunning_ = true;	
}

TcpServer::TcpServer(ThreadPool &thrpool, const char *pIp, unsigned short port)
	:seraddr_(pIp, port),
	 sockfd_(),
	 epollfd_(sockfd_.getfd(), thrpool)
{
	sockfd_.ready(seraddr_, 10);
	isRunning_ = true;
}

TcpServer::~TcpServer()
{
	std::cout << "~TcpServer()" <<std::endl;//test
	if(isRunning_)
		stop();
}

void TcpServer::start()
{
	epollfd_.loop();	
}

void TcpServer::stop()
{
	epollfd_.unloop();
}

void TcpServer::setConnectionCb(TcpSerCallback cb)
{
	epollfd_.setConnectionCb(cb);
}

void TcpServer::setMessageCb(TcpSerCallback cb)
{
	epollfd_.setMessageCb(cb);
}

void TcpServer::setCloseCb(TcpSerCallback cb)
{
	epollfd_.setCloseCb(cb);
}

}//end of namespace
