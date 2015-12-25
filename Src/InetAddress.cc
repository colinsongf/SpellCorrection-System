#include "../Inc/InetAddress.h"
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


namespace tanfy
{

InetAddress::InetAddress(const struct sockaddr_in &addr)
	:addr_(addr)
{
}

InetAddress::InetAddress(unsigned short port)
{
	memset(&addr_, 0, sizeof(addr_));
	
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	addr_.sin_addr.s_addr = INADDR_ANY;
}

InetAddress::InetAddress(const char *pIp, unsigned short port)
{
	memset(&addr_, 0, sizeof(addr_));

	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	addr_.sin_addr.s_addr = inet_addr(pIp);	
}

struct sockaddr_in *InetAddress::getSockAddrPtr()
{
	return &addr_;
}

std::string InetAddress::getIp() const
{
	return (inet_ntoa(addr_.sin_addr));
}

unsigned short InetAddress::getPort() const
{
	return (ntohs(addr_.sin_port));
}

























}// end of namespace
