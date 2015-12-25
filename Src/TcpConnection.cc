#include "../Inc/TcpConnection.h"
#include "../Inc/Socket.h"
#include "../Inc/SocketIO.h"
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>

namespace tanfy
{

TcpConnection::TcpConnection(int acceptfd, ThreadPool &thrpool)
	:acceptfd_(acceptfd),
	 socketIO_(acceptfd),
	 localAddr_(Socket::getLocalAddr(acceptfd)),
	 peerAddr_(Socket::getPeerAddr(acceptfd)),
	 thrpool_(thrpool),
	 isRunning_(true)
{
}

TcpConnection::~TcpConnection()
{
	if(isRunning_)
		shutdown();
}

std::string TcpConnection::recvmsg()
{
	char buf[1024] = {0};
	ssize_t ret = socketIO_.readline(buf, sizeof(buf));
	if(0 == ret)
		return std::string();
	buf[strlen(buf)-1] = '\0'; //去掉字符串最后的'\n'
	return std::string(buf);
}

void TcpConnection::sendmsg(const std::string &str)
{
	ssize_t ret = socketIO_.writen(str.c_str(), str.size());
	if(ret != str.size())
	{
		std::cout << "sendmsg error" << std::endl;
	}	
}

void TcpConnection::shutdown()
{
	acceptfd_.shutdownWrite();
	isRunning_ = false;	
}

std::string TcpConnection::toString()
{
	char buf[1024]={0};
	sprintf(buf, "%s:%u ---> %s:%u", localAddr_.getIp().c_str(), 
									 localAddr_.getPort(),
									 peerAddr_.getIp().c_str(),
									 peerAddr_.getPort());
	return std::string(buf);
}



void TcpConnection::setConnectionCb(TcpConnCallback cb)
{
	connectionCb_ = cb;
}

void TcpConnection::setMessageCb(TcpConnCallback cb)
{
	messageCb_ = cb;
}

void TcpConnection::setCloseCb(TcpConnCallback cb)
{
	closeCb_ = cb;
}

void TcpConnection::handleConnectionCb()
{
	if(connectionCb_)
	{
		connectionCb_(this);
	}
}

void TcpConnection::handleMessageCb()
{
	if(messageCb_)
	{
		messageCb_(this);
	}
}

void TcpConnection::handleCloseCb()
{
	if(closeCb_)
	{
		closeCb_(this);
	}
}







}//end of namespace
