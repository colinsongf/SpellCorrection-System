#ifndef __SOCKETIO_H__
#define __SOCKETIO_H__

#include <stdio.h>


namespace tanfy
{

class SocketIO
{
public:
	SocketIO(int acceptfd);
	
	ssize_t readn(char *buf, size_t count);
	ssize_t writen(const char *buf, size_t count);
	ssize_t readline(char *buf, size_t maxcount);

private:
	ssize_t recvpeek(char *buf, size_t count);

private:
	int acceptfd_;
};



}//end of namespace




#endif
