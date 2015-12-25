#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"
#include "Noncopyable.h"
#include <functional>
#include <string>


namespace tanfy
{

class ThreadPool;

class TcpConnection;
typedef TcpConnection* TcpConnectionPtr;

class TcpConnection:Noncopyable
{
public:
	typedef std::function<void(TcpConnectionPtr)> TcpConnCallback;
	
	TcpConnection(int acceptfd, ThreadPool &thrpool);
	~TcpConnection();

	std::string recvmsg();
	void sendmsg(const std::string &str);
	void shutdown();	
	std::string toString();
	
	ThreadPool &getThrpool()
	{
		return thrpool_;
	}

	void setConnectionCb(TcpConnCallback cb);
	void setMessageCb(TcpConnCallback cb);
	void setCloseCb(TcpConnCallback cb);
	void handleConnectionCb();
	void handleMessageCb();
	void handleCloseCb();	

private:
	Socket acceptfd_;
	SocketIO socketIO_;
	InetAddress localAddr_;
	InetAddress peerAddr_;
	ThreadPool &thrpool_; //链接运行所在的线程池
	bool isRunning_;

	TcpConnCallback connectionCb_;
	TcpConnCallback messageCb_;
	TcpConnCallback closeCb_;
};


}//end of namespace






#endif
