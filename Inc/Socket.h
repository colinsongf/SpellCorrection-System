#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "Noncopyable.h"


namespace tanfy
{

class InetAddress;

class Socket:Noncopyable
{
public:
	Socket();
	Socket(int sockfd);
	~Socket();	

	int getfd()
	{
		return sockfd_;
	}

	void ready(InetAddress &addr, int clinum);
	int accept();
	
	static InetAddress getLocalAddr(int acceptfd);
	static InetAddress getPeerAddr(int acceptfd);
	void shutdownWrite();

private:
	void bindAddress(InetAddress &addr);
	void listen(int clinum);
	void setReuseAddr(bool flag);
	void setReusePort(bool flag);
	
private:
	int sockfd_;
	bool isRunning_;
};

}//end of namespace





#endif
