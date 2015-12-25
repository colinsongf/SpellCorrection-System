#include "../Inc/Socket.h"
#include "../Inc/InetAddress.h"
#include <iostream> //test
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


namespace tanfy
{

int CreateSocket()
{
	int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sockfd)
	{
		perror("Create Socket error");
		exit(EXIT_FAILURE);
	}
	return sockfd;
}


//===========================================================
//	class Socket
//===========================================================
Socket::Socket()
	:sockfd_(tanfy::CreateSocket()),
	 isRunning_(true)
{	
}

Socket::Socket(int sockfd)
	:sockfd_(sockfd),
	 isRunning_(true)
{
}

Socket::~Socket()
{
	if(isRunning_)
	{
		shutdownWrite();
	}
}

void Socket::ready(InetAddress &addr, int clinum)
{
	setReuseAddr(true);
	setReusePort(true);	
	bindAddress(addr);
	listen(clinum);
}

void Socket::bindAddress(InetAddress &addr)
{
	int ret = ::bind(sockfd_, (struct sockaddr *)addr.getSockAddrPtr(), sizeof(struct sockaddr));
	if(-1 == ret)
	{
		perror("bind error");
		exit(EXIT_FAILURE);
	}
}

void Socket::listen(int clinum)
{
	int ret = ::listen(sockfd_, clinum);
	if(-1 == ret)
	{
		perror("listen error");
		exit(EXIT_FAILURE);
	}
}

void Socket::setReuseAddr(bool flag)
{
	int optval = flag? 1:0;
	int ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval)));
	if(-1 == ret)
	{
		perror("setReuseAddr error");
		exit(EXIT_FAILURE);
	}
}

void Socket::setReusePort(bool flag)
{
#ifdef SO_REUSEPORT
	int optval = flag? 1:0;
	int ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval)));
	if(-1 == ret)
	{
		perror("setReusePort error");
		exit(EXIT_FAILURE);
	}
#endif
}

int Socket::accept()
{
	int acceptfd = ::accept(sockfd_, NULL, NULL);
	if(-1 == acceptfd)
	{
		perror("accept error");
	}
	return acceptfd;
}

tanfy::InetAddress Socket::getLocalAddr(int acceptfd)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	int len = sizeof(addr);

	int ret = getsockname(acceptfd, (struct sockaddr *)&addr, (socklen_t *)&len);
	if(-1 == ret)
	{
		perror("getLocalAddr error");
	}
	return InetAddress(addr);
}

tanfy::InetAddress Socket::getPeerAddr(int acceptfd)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	int len = sizeof(addr);

	int ret = getpeername(acceptfd, (struct sockaddr *)&addr, (socklen_t *)&len);
	if(-1 == ret)
	{
		perror("getPeerAddr error");
	}
	return InetAddress(addr);
}

void Socket::shutdownWrite()
{
	int ret = ::shutdown(sockfd_, SHUT_WR);
	if(-1 == ret)
	{
		perror("shutdown error");
	}
	else
	{
		isRunning_ = false;
	}	
}


}//end of namespace
