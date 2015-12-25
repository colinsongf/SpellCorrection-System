#ifndef __CACHE_H__
#define __CACHE_H__

#include <map>
#include <string>
#include "MutexLock.h"

namespace tanfy
{

class Conf;

class Cache
{
public:
	typedef std::map<std::string, std::string> ResultMap;

	Cache():mutex_(){}
	Cache(Conf &conf);
	~Cache();

	void update(Cache &rcache);//将rcache中map内容加进去
	void setMap(const Cache &rcache);//使用rcache的map替换对象的map
	ResultMap &getMap();
	void addDatatoMap(std::string word, std::string result); //向cachemap中添加数据
	void displayMap();//向终端输出map

	void readfromFile();
	void writetoFile();
	
private:
	ResultMap cacheMap_;
	const char *cachepath_;	
	MutexLock mutex_;
};


}//end of namespace



#endif
