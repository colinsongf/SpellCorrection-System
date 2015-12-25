#ifndef __EPOLLPOLLER_H__
#define __EPOLLPOLLER_H__

#include "Noncopyable.h"
#include "TcpConnection.h"
#include <sys/epoll.h>
#include <functional>
#include <vector>
#include <map>

namespace tanfy
{

class ThreadPool;

class EpollPoller: Noncopyable
{
public:
	typedef std::function<void(TcpConnectionPtr)> EpollCallback;
	
	EpollPoller(int listenfd, ThreadPool &thrpool);
	~EpollPoller();
	
	void loop();
	void unloop();

	void setConnectionCb(EpollCallback cb);
	void setMessageCb(EpollCallback cb);
	void setCloseCb(EpollCallback cb);

private:
	void waitEpollfd();
	void handleConnection();
	void handleMessage(int acceptfd);

private:
	int listenfd_;
	int epollfd_;
	ThreadPool &thrpool_;
	bool isRunning_;	
	
	typedef std::vector<struct epoll_event> EventList;
	EventList eventsList_;
	typedef std::map<int, TcpConnectionPtr> ConnectionMap;
	ConnectionMap connMap_;

	EpollCallback connectionCb_;
	EpollCallback messageCb_;
	EpollCallback closeCb_;

};

}//end of namespace






#endif



