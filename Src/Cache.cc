#include "../Inc/Cache.h"
#include "../Inc/Conf.h"
#include "../Inc/MutexLock.h"
#include <map>
#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>


namespace tanfy
{


void createMap(const char *cachepath, std::map<std::string, std::string> &cacheMap)
{
	//0. 打开文件
	FILE *fp = fopen(cachepath, "a+");
	if(NULL == fp)
	{
		perror("file open error");
		exit(EXIT_FAILURE);
	}
	
	//1. 按行提取文件（word和result之间用=隔开）
	char *linebuf = NULL;
	size_t len = 0;
	bool flag = false;//word
	int wordidx, resultidx;
	while(-1 != ::getline(&linebuf, &len, fp))
	{
		char word[98] = {0};
		char result[908] = {0};
		//1.1 提取
		wordidx = 0;
		resultidx = 0;
		for(int idx = 0; idx < strlen(linebuf); ++idx)
		{
			if(!flag)//word
			{
				if(linebuf[idx] == '=')
				{	
					word[wordidx] = '\0';
				//	std::cout << "word = " << word << std::endl;//test
					flag = true;
				}
				else
					word[wordidx++] = linebuf[idx];
			}
			else//result
			{
				result[resultidx++] = linebuf[idx];
			}
		}
		result[resultidx-1] = '\0';//getline读取\n,需作处理
		flag = false; //word
	//	std::cout << "result = " << result << std::endl; //test
		//1.2 加入map中
		std::pair<std::map<std::string, std::string>::iterator, bool > ret = cacheMap.insert(std::make_pair(word, result));
		if(!ret.second)
		{
			std::cout << "insert map error";
			exit(EXIT_FAILURE);
		}
	}
	
	fclose(fp);
}


//===========================================
//class Cache
//===========================================
Cache::Cache(Conf &conf)
	:cachepath_(conf.getCachePath()),
	 mutex_()
{
	tanfy::createMap(cachepath_, cacheMap_);
#if 0
	//test
	ResultMap::iterator ite = cacheMap_.begin();
	for(; ite != cacheMap_.end(); ++ite)
	{
		std::cout << ite->first <<"=" << ite->second << std::endl;
	}
	exit(EXIT_FAILURE);
#endif
}


Cache::~Cache()
{
	delete []cachepath_;
}


void Cache::update(Cache &rcache)
{
	mutex_.lock();
	ResultMap::iterator ite = rcache.cacheMap_.begin();
	for(; ite != rcache.cacheMap_.end(); ++ite)
	{
		cacheMap_.insert(*ite);//失败与否无关	
	}
	mutex_.unlock();
}

void Cache::setMap(const Cache &rcache)
{
	mutex_.lock();
	cacheMap_ = rcache.cacheMap_;	
	mutex_.unlock();
}


Cache::ResultMap &Cache::getMap()
{
	return cacheMap_;
}


void Cache::addDatatoMap(std::string word, std::string result)
{
	mutex_.lock();
	std::pair<ResultMap::iterator, bool> ret = cacheMap_.insert(std::make_pair(word, result));
	if(!ret.second)
	{
		std::cout << "insert to cachemap error" << std::endl;
		exit(EXIT_FAILURE);
	}
	mutex_.unlock();	
}


void Cache::displayMap()
{
	ResultMap::iterator ite = cacheMap_.begin();
	for(; ite != cacheMap_.end(); ++ite)
	{
		std::cout << ite->first <<"=" << ite->second << std::endl;
	}
}



void Cache::readfromFile()
{
	mutex_.lock();
	tanfy::createMap(cachepath_, cacheMap_);	
	mutex_.unlock();
}


void Cache::writetoFile()
{
	mutex_.lock();
	std::ofstream cache(cachepath_, std::ios::out);
	if(!cache.good())
	{
		std::cout << "write cache to file error" << std::endl;
		exit(EXIT_FAILURE);
	}
	ResultMap::iterator ite = cacheMap_.begin();
	while(ite != cacheMap_.end())
	{
		cache << ite->first << '=' << ite->second << '\n';
		ite ++;
	}
	cache.close();	
	mutex_.unlock();
}


}//end of namespace
