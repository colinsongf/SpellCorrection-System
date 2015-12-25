#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__

#include <netinet/in.h>
#include <string>


namespace tanfy
{

class InetAddress
{
public:
	InetAddress(const struct sockaddr_in &addr);
	InetAddress(unsigned short port);
	InetAddress(const char *pIp, unsigned short port);
	
	struct sockaddr_in *getSockAddrPtr();
	std::string getIp() const;
	unsigned short getPort() const;		

private:
	struct sockaddr_in addr_;
};





}//end of namespace





#endif
