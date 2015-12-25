#include "../Inc/EpollPoller.h"
#include "../Inc/TcpConnection.h"
#include <iostream>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <memory>
#include <vector>
#include <map>


namespace tanfy
{

int createEpollfd()
{
	int epollfd = ::epoll_create1(0);
	if(-1 == epollfd)
	{
		perror("create epoll error");
		exit(EXIT_FAILURE);
	}
	return epollfd;
}

void addEpollfdRead(int epfd, int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	
	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	if(ret == -1)
	{
		perror("add in epollfd error");
		exit(EXIT_FAILURE);
	}
}

void delEpollfdRead(int epfd, int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	
	int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
	if(ret == -1)
	{
		perror("del in epollfd error");
		exit(EXIT_FAILURE);
	}
}

int acceptInEpollfd(int listenfd)
{
	int acceptfd = ::accept(listenfd, NULL, NULL);
	if(-1 == acceptfd)
	{
		perror("accept in epollfd error");
	}
	return acceptfd;
}

ssize_t recvpeek(int acceptfd, char *buf, int count)
{
	ssize_t recvnum;
	do
	{
		recvnum = ::recv(acceptfd, buf, count, MSG_PEEK);
	}while(-1 == recvnum && errno == EINTR);
	return recvnum;
}

bool acceptfdCloseCheck(int acceptfd)
{
	char recvbuf[256] = {0};
	ssize_t ret = tanfy::recvpeek(acceptfd, recvbuf, sizeof(recvbuf));
	return (0 == ret);	
}


//===========================================
//class EpollPoller
//===========================================
EpollPoller::EpollPoller(int listenfd, ThreadPool &thrpool)
	:listenfd_(listenfd),
	 epollfd_(tanfy::createEpollfd()),
	 thrpool_(thrpool),
	 isRunning_(false),
	 eventsList_(1024)
{
	tanfy::addEpollfdRead(epollfd_, listenfd);	
}

EpollPoller::~EpollPoller()
{
#if 0
	if(isRunning_)
	{
		unloop();
	}
#endif 
	::close(epollfd_);
}

void EpollPoller::loop()
{
	isRunning_ = true;
	while(isRunning_)
	{
		waitEpollfd();
	}	
}

void EpollPoller::unloop()
{
	isRunning_ = false;
}

void EpollPoller::waitEpollfd()
{
	int readynum;
	do
	{
		readynum = epoll_wait(epollfd_, &(*eventsList_.begin()), eventsList_.size(), 5000);
	}while(-1 == readynum && errno == EINTR);
	
	if(-1 == readynum)
	{
		perror("epoll wait error");
		exit(EXIT_FAILURE);
	}
	else if(0 == readynum)
	{
	//	std::cout << "epoll wait timeout" << std::endl; 
	}
	else
	{
		if(readynum == eventsList_.size())
			eventsList_.resize(eventsList_.size()*2);
		
		int idx;
		for(idx = 0; idx < readynum; ++idx)
		{
			if(eventsList_[idx].data.fd == listenfd_)
			{
				if(eventsList_[idx].events & EPOLLIN)
					handleConnection();
			}
			else
			{
				if(eventsList_[idx].events & EPOLLIN)
					handleMessage(eventsList_[idx].data.fd);
			}
		}		
	}
}

void EpollPoller::handleConnection()
{
	//0. 与客户端进行连接
	int acceptfd = tanfy::acceptInEpollfd(listenfd_);

	//1. 将此链接加入epollfd中，并将链接包装成TcpConnection来处理发送信息的操作
	tanfy::addEpollfdRead(epollfd_, acceptfd);

//	std::unique_ptr<TcpConnection> pConn(new TcpConnection(acceptfd));
	TcpConnectionPtr pConn(new TcpConnection(acceptfd, thrpool_));
	std::pair<ConnectionMap::iterator, bool> ret = connMap_.insert(std::make_pair(acceptfd, pConn));
	assert(ret.second == true);//断言
	
	//2. 输出链接信息
	pConn->setConnectionCb(connectionCb_);
	pConn->setMessageCb(messageCb_);
	pConn->setCloseCb(closeCb_);
	
	pConn->handleConnectionCb();	
}

void EpollPoller::handleMessage(int acceptfd)
{
	//0. 检测链接是否关闭
	bool ifClose = tanfy::acceptfdCloseCheck(acceptfd);
	
	ConnectionMap::iterator ite = connMap_.find(acceptfd);
	if(ifClose) //1.1关闭时，将该链接从map和epollfd中删除
	{
		delEpollfdRead(epollfd_, acceptfd);

		if(ite != connMap_.end())
		{
			(*ite).second->handleCloseCb();
			connMap_.erase(ite);
		}		
	}
	else //1.2 未关闭，接收信息
	{
		if(ite != connMap_.end())
		{
			(*ite).second->handleMessageCb();
		}
	}
}

void EpollPoller::setConnectionCb(EpollCallback cb)
{
	connectionCb_ = cb;
}

void EpollPoller::setMessageCb(EpollCallback cb)
{
	messageCb_ = cb;
}

void EpollPoller::setCloseCb(EpollCallback cb)
{
	closeCb_ = cb;
}

}//end of namespace
