#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Socket.h"
#include "InetAddress.h"
#include "EpollPoller.h"
#include "Noncopyable.h"


namespace tanfy
{

class ThreadPool;

class TcpServer: Noncopyable
{
public:
	typedef EpollPoller::EpollCallback TcpSerCallback;
	
	TcpServer(ThreadPool &thrpool, unsigned short port);
	TcpServer(ThreadPool &thrpool, const char *pIp, unsigned short port);
	~TcpServer();

	void start();
	void stop();	

	void setConnectionCb(TcpSerCallback cb);
	void setMessageCb(TcpSerCallback cb);
	void setCloseCb(TcpSerCallback cb);

private:
	Socket sockfd_;
	EpollPoller epollfd_;
	InetAddress seraddr_;
	bool isRunning_;
	
	TcpSerCallback connectionCb_;
	TcpSerCallback messageCb_;
	TcpSerCallback closeCb_;
};


}//end of namespace







#endif
