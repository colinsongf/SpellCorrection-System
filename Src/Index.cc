#include "../Inc/Index.h"
#include "../Inc/Dict.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <string.h>



namespace tanfy
{
extern size_t getWordLen(const char word);

extern void delSameWord(char *sendbuf, const char *recvbuf, int count);

typedef std::map<std::string, std::vector<int> > IndexMap;

void createIndex(Dict &dict, IndexMap &indexMap)
{
	//0. 从dict中提取出vector
	std::vector<std::pair<std::string, int> > &dictVec = dict.getVector();
	//1. 遍历dictVector，创建index
	for(int idx=0; idx < dictVec.size(); ++idx)
	{
		const char *wordtmp= dictVec[idx].first.c_str(); //从dict中提取单词
		char word[256] = {0};
		delSameWord(word, wordtmp, strlen(wordtmp));

		//std::cout << "wordtmp = " << wordtmp << std::endl;//test
		//std::cout << "wordfinal = " << word << std::endl; //test

		for(int wordidx = 0; wordidx < strlen(word); ) //遍历word中字母，加入indexmap
		{
			int wordlen = tanfy::getWordLen(word[wordidx]);
			IndexMap::iterator itm = indexMap.find(std::string(word, wordidx, wordlen));
			//std::cout << "std::string(&word[])" << std::string(word, wordidx, wordlen) << std::endl;//test
			if(itm != indexMap.end()) //在indexMap中找到该字
			{
				(*itm).second.push_back(idx);
			}
			else //在indexMap中未找到该字母
			{
				std::pair<IndexMap::iterator, bool> ret = indexMap.insert(std::make_pair(std::string(word, wordidx, wordlen), std::vector<int>(1, idx)));
				if(!ret.second)
				{
					std::cout << "Insert to indexMap error" << std::endl;
					exit(EXIT_FAILURE);
				}
			}
			wordidx += wordlen;				
		}
	}
}



//==================================
//class Index
//==================================

Index *Index::pInstance_ = NULL;
Index::AutoRelease Index::autoRelease_;

Index *Index::getInstance(Dict &dict)
{
	if(NULL == pInstance_)
	{
		pInstance_ = new Index(dict);
	}
	return pInstance_;
}


//private
Index::Index(Dict &dict)
{
	tanfy::createIndex(dict, indexMap_);
//	exit(EXIT_FAILURE);//test
}


IndexMap &Index::getIndexMap()
{
	return indexMap_;
}


bool Index::getPosinVector(std::string word, std::vector<int> &posVec)
{
	IndexMap::iterator itm = indexMap_.find(word);
	if(itm != indexMap_.end())
	{
		posVec = itm->second;
		return true;
	}
	return false;
}


void Index::displayMap()
{
	IndexMap::iterator itm = indexMap_.begin();
	for(; itm != indexMap_.end(); ++itm)
	{
		std::cout << itm->first << "=";
		for(int idx = 0; idx < itm->second.size(); ++idx)
		{
			std::cout << itm->second[idx] << " ";
		}
		std::cout << std::endl;
	}
}









}//end of namespace
