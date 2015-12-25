#ifndef __CONF_H__
#define __CONF_H__

#include <map>
#include <string>


namespace tanfy
{

class Conf
{
public:
	typedef std::map<std::string, std::string> ConfMap;	
	
	Conf(const char *confpath);
	
	std::string searchMap(const std::string &key);
	const char *getIp();
	unsigned short getPort();
	const char *getDictEnPath();
	const char *getDictChPath();
	const char *getCachePath();

	const char *getSourcePath(int idx);

private:
	ConfMap confMap_;	
};

}//end of namespace





#endif
