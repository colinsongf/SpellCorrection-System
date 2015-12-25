#include "../Inc/Conf.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <string>
#include <map>


namespace tanfy
{

extern FILE *openFileRead(const char *confpath)
{
	FILE *fp = fopen(confpath, "r");
	if(NULL == fp)
	{
		perror("fopen error");
		exit(EXIT_FAILURE);
	}	
	return fp;
}

void extractDataToMap(const char *buf, tanfy::Conf::ConfMap &confMap)
{
	char key[27] = {0};
	char word[1024] = {0};
	//0. 提取数据
	int idx;
	int keyidx = 0;
	int wordidx = 0;
	bool ifkey = true;
		
	for(idx = 0; idx < strlen(buf); ++idx)
	{
		if(ifkey) //key
		{
			if('=' == buf[idx])
			{
				ifkey = false;
				key[keyidx] = '\0';
				continue;			
			}
			key[keyidx++] = buf[idx];	
		}	
		else //word
		{
			word[wordidx++] = buf[idx];				
		}
	}
	word[wordidx-1] = '\0';//可能有问题：不知道getline是否会读取\n
	
//	std::cout <<"key = " << key << std::endl; //test
//	std::cout << "word = " << word << std::endl; //test

	//1. 将key-word放入map中
	std::pair<tanfy::Conf::ConfMap::iterator, bool> ret = confMap.insert(std::make_pair(std::string(key), std::string(word)));
	if(!ret.second)
	{
		std::cout << "confMap insert error" << std::endl;;
		exit(EXIT_FAILURE);
	}	
}


void readConf(const char *confpath, tanfy::Conf::ConfMap &confMap)
{
	//0. 打开配置文件
	FILE *fp = tanfy::openFileRead(confpath);
	
	//1. 从配置文件中读取信息至map中
	char *linebuf = NULL;
	size_t len = 0;
	
	//1.1 读取配置信息
	while(-1 != ::getline(&linebuf, &len, fp))
	{
	//1.2 将配置信息分解，并填入map中
		tanfy::extractDataToMap(linebuf, confMap);		
	}
	//bug:line free
}

//========================================
//class Conf
//========================================
Conf::Conf(const char *confpath)
{
	tanfy::readConf(confpath, confMap_);
}

std::string Conf::searchMap(const std::string &key)
{
	ConfMap::iterator ite = confMap_.find(key);
	if(ite != confMap_.end())
		return (ite->second);
	return std::string();
}

const char *Conf::getIp()
{
	std::string ip = searchMap(std::string("ip"));
	return ip.c_str();		
}

unsigned short Conf::getPort()
{
	unsigned short uport;
	std::string port = searchMap(std::string("port"));
	
	std::stringstream ss_stream;
	ss_stream << port;
	ss_stream >> uport;
	return uport;
}

const char *Conf::getDictEnPath()
{
	std::string word = searchMap(std::string("dict_en"));
	return word.c_str();
}

const char *Conf::getDictChPath()
{
	std::string word = searchMap(std::string("dict_ch"));
	return word.c_str();
}


const char *Conf::getSourcePath(int idx)
{
	std::string key, word;
	std::stringstream ss_stream;
	ss_stream << idx;
	ss_stream >> key;
	word = searchMap(key);
	return word.c_str(); 			
}

const char *Conf::getCachePath()
{
	std::string word = searchMap(std::string("cache"));
	return word.c_str();
}














}//end of namespace
