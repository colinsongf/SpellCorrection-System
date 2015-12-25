#include "../Inc/SocketIO.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


namespace tanfy
{

SocketIO::SocketIO(int acceptfd)
	:acceptfd_(acceptfd)
{
}

ssize_t SocketIO::readn(char *buf, size_t count)
{
	size_t leftnum = count;

	while(leftnum > 0)
	{
		ssize_t readnum = ::read(acceptfd_, buf, leftnum);
		
		if(-1 == readnum)
		{
			if(errno == EINTR)
				continue;
			perror("read error");
			break;
		}	
		else if(0 == readnum)
		{
			std::cout << "there is nothing to read" << std::endl;
			break;
		}
		else
		{
			leftnum -= readnum;
			buf += readnum;			
		}
	}
	return (count - leftnum);
}

ssize_t SocketIO::writen(const char *buf, size_t count)
{
	size_t leftnum = count;
	const char *pbuf = buf;

	while(leftnum > 0)
	{
		ssize_t writenum = ::write(acceptfd_, pbuf, leftnum);
		if(-1 == writenum)
		{
			if(errno == EINTR)
				continue;
			perror("write error");
			break;
		}
		else if(0 == writenum)
		{
			std::cout << "there is nothing to write" << std::endl;
			break;
		}		
		else
		{
			leftnum -= writenum;
			pbuf += writenum;
		}
	}
	return (count - leftnum);	
}

ssize_t SocketIO::recvpeek(char *buf, size_t count)
{
	ssize_t recvnum;
	do
	{
		recvnum = ::recv(acceptfd_, buf, count, MSG_PEEK);
	}while(-1 == recvnum && errno == EINTR);
	if(-1 == recvnum)
	{
		perror("recv error");
	}
	else if(0 == recvnum)
	{
		std::cout << "there is nothing to read" << std::endl;		
	}
	return recvnum;
}

ssize_t SocketIO::readline(char *buf, size_t maxcount)
{
	size_t leftnum = maxcount -1;
	char *pbuf = buf;

	while(leftnum > 0)
	{
		//0. 预览数据
		ssize_t peeknum = recvpeek(pbuf, leftnum);
		if(peeknum <= 0)
			return EXIT_FAILURE;
		
		//1. 查找\n
		ssize_t idx;
		ssize_t readnum;
		ssize_t neednum;
		for(idx = 0; idx < peeknum; ++idx)
		{
			if(pbuf[idx] == '\n')
			{
				neednum = idx+1;
				readnum = readn(pbuf, neednum);
				if(readnum != neednum)
					return EXIT_FAILURE;
				
				pbuf += readnum;
				leftnum -= readnum;
				*pbuf = '\0';
				return (maxcount - leftnum);		
			}
		}
		neednum = peeknum;
		readnum = readn(pbuf, neednum);
		if(readnum != neednum)
			return EXIT_FAILURE;
		pbuf += readnum;
		leftnum -= readnum;			
	}
	*pbuf = '\0';	
	return (maxcount - leftnum);
}

}//end of namespace
