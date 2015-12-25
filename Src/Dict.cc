#include "../Inc/Dict.h"
#include "../Inc/Conf.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>


namespace tanfy
{

extern FILE *openFileRead(const char *confpath);

void createVector(const char *dictpath, std::vector<std::pair<std::string, int> > &vecDict)
{
	std::ifstream dict(dictpath);
	
	bool flag = true; // word
	bool oneflag = false; //提取完一个置true
	std::string str, word;
	int frequency;
	
	while(dict >> str)
	{
		if(flag) //word
		{
			word = str;
			flag = false;
		}
		else
		{
			std::stringstream sstream;	
			sstream << str;
			sstream >> frequency;
			flag = true;
			oneflag = true;
		}
		if(oneflag)
		{
			vecDict.push_back(std::make_pair(word, frequency));
			oneflag = false;
		}
	}	
	dict.close();	
}


//======================================
//class Dict
//======================================

Dict* Dict::pInstance_ = NULL;
Dict::AutoRelease Dict::autoRelease_;

Dict* Dict::getInstance(Conf &conf)
{
	if(NULL == pInstance_)
	{
		pInstance_ = new Dict(conf);
	}
	return pInstance_;
}

//private
Dict::Dict(Conf &conf)
	:conf_(conf)	 
{
	const char *en_dictpath = conf_.getDictEnPath();
	const char *ch_dictpath = conf_.getDictChPath();

	tanfy::createVector(en_dictpath, vecDict_);
	tanfy::createVector(ch_dictpath, vecDict_);
}

Dict::DictVector &Dict::getVector()
{
	return vecDict_;
}


}//end of namespace
