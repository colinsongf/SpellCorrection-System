#include "../Inc/PreDict.h"
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_set>


namespace tanfy
{

extern FILE *openFileRead(const char *confpath);

void LoadStopWordDict(const std::string &filePath, std::unordered_set<std::string> &stopWords)
{
	std::ifstream ifs(filePath.c_str());
	if(!ifs.is_open())
	{
		perror("stopwordsdict open error");
		exit(EXIT_FAILURE);
	}
	std::string linestr;
	while(getline(ifs, linestr))
	{
		stopWords.insert(linestr);
	}
	assert(stopWords.size());
}



//class PreDict
PreDict::PreDict(cppjieba::Jieba &mycut, int source_num_en, const char *dict_en, const char *sourceconf_path_en, int source_num_ch, const char *dict_ch, const char *sourceconf_path_ch)
	:mycut_(mycut),
	 source_num_en_(source_num_en),
	 dict_en_(new char[strlen(dict_en)+1]),
	 source_conf_en_(sourceconf_path_en),
	 source_num_ch_(source_num_ch),
	 dict_ch_(new char[strlen(dict_ch)+1]),
	 source_conf_ch_(sourceconf_path_ch)
{
	strcpy(dict_en_, dict_en);
	strcpy(dict_ch_, dict_ch);
	
	std::string stopPath = "../Lib/cppjieba/dict/stop_words.utf8";
	LoadStopWordDict(stopPath, stopWords_);
	//std::cout << "PreDict()" << std::endl;//test
}

PreDict::~PreDict()
{
	delete []dict_en_;
	delete []dict_ch_;
}

void PreDict::createDictEn()
{
	//0. 获取源文件路径
	char *source[10] = {0};	
	getSourcePath(source, true);

	//1. 循环：获取源文件内容；按行提取单词；放入map中
	WordFre_Map wordfreMap;
	int idx;
	for(idx = 0; idx < source_num_en_; ++idx)
	{
		extractWordtoMap(source[idx], wordfreMap, true);
	}

	//2. 将map中的内容读入dict_en文件
	readMaptoDict(wordfreMap, true);

	std::cout << "英文词典建立完成" << std::endl;
}


void PreDict::createDictCh()
{
	//0. 获取源文件路径
	char *source[100] = {0};
	getSourcePath(source, false);
	
	//1. 循环：分词，并放入map中	
	WordFre_Map wordfreMap;
	int idx;
	for(idx = 0; idx < source_num_ch_; ++idx)
	{
		extractWordtoMap(source[idx], wordfreMap, false);		
	} 		
	
	//2. 将map内容写入dict_ch中
	readMaptoDict(wordfreMap, false);

	std::cout << "中文词典建立完成" << std::endl;
}


void PreDict::getSourcePath(char *source[], bool enflag) 
{
	int idx;
	if(enflag)
	{
		for(idx = 0; idx < source_num_en_; ++idx)
		{
			source[idx] = const_cast<char *>(source_conf_en_.getSourcePath(idx));						
			//std::cout << idx << ":"<< source[idx] << std::endl;//test
		}
	}
	else
	{
		for(idx = 0; idx < source_num_ch_; ++idx)
		{
			source[idx] = const_cast<char *>(source_conf_ch_.getSourcePath(idx));						
			//std::cout << idx << ":"<< source[idx] << std::endl;//test
		}	
	}
}


void PreDict::extractWordtoMap(const char *source, WordFre_Map &wordfreMap, bool enflag)
{
	if(enflag)
	{
		FILE *fp = tanfy::openFileRead(source);
		//1. 按行提取并处理数据
		char *linebuf = NULL;
		size_t len = 0;
		while(-1 != ::getline(&linebuf, &len, fp))
		{
			//1.1 将非字母全部置为空格，同时将大写改为小写
			dealWord(linebuf, strlen(linebuf));
			//1.2 提取单词，并加入map			
			addWordtoMap(linebuf, strlen(linebuf), wordfreMap);		
		}
	}
	else
	{
		addChtoMap(source, wordfreMap);	
	}	
	//bug line free
}

//======================================
//提取中文所用函数
//======================================
void PreDict::addChtoMap(const char *source, WordFre_Map &wfmap)
{
	//1.提取文件内容
	std::string content;
#if 0
	std::string tmp;
	
	std::ifstream ifs(source);
	if(!ifs.is_open())
	{
		perror("ch source open error");
		exit(EXIT_FAILURE);
	}

		
	while(ifs >> tmp)
	{
		content += tmp;	
	}
#endif
#if 1
	FILE *fp = tanfy::openFileRead(source); 
	char *linebuf = NULL;
	size_t len = 0;
	while(-1 != ::getline(&linebuf, &len, fp))
	{
		linebuf[strlen(linebuf)-1] = '\0';
		content += linebuf;
	}
	//std::cout << "content" << std::endl << content << std::endl;//test
#endif

	//2. 分词
	std::vector<std::string> words;
	mycut_.Cut(content, words);
	
	for(std::vector<std::string>::iterator ite = words.begin(); ite != words.end();)
	{
		if(stopWords_.end()!=stopWords_.find(*ite))
		{
			words.erase(ite);
		}
		else
			ite++;
	}
#if 0
	//test	
	for(std::vector<std::string>::iterator itv = words.begin(); itv != words.end(); ++itv)
	{
		std::cout << *itv << '\t';
	}
	exit(EXIT_FAILURE);
#endif

	//3. 加入map中
	std::vector<std::string>::iterator itv = words.begin();
	for(; itv != words.end(); ++itv)
	{
		WordFre_Map::iterator ite = wfmap.begin();
		ite = wfmap.find(*itv);
		if(ite != wfmap.end()) //map中已存在该单词	
		{
			ite->second ++;
		}
		else //map中不存在该单词
		{
			std::pair<WordFre_Map::iterator, bool> ret = wfmap.insert(std::make_pair(*itv, 1));
			if(!ret.second)
			{
				std::cout << "add chword to map error" << std::endl;
				exit(EXIT_FAILURE);
			}
		}			
	}

}

//=======================================
//提取英文所用函数
void PreDict::dealWord(char *buf, int count)
{
	int index;
	for(index = 0; index < count; ++index)
	{
		 if(buf[index] != ' ' && !((buf[index] >='a'&&buf[index] <= 'z')||(buf[index]>='A'&& buf[index] <= 'Z')))
         {
             buf[index] = ' ';
         }
         if(buf[index]>='A' && buf[index]<='Z')
         {
             buf[index] += 32;   
         }		
	}
}

void PreDict::addWordtoMap(char *buf, int count, WordFre_Map &wfmap)
{
	char word[98] = {0};
	bool flag = false; //记录是否正在提取单词
	int word_ind = 0;
	int index;
	for(index = 0; index < count; ++index)
	{
		if(buf[index] >= 'a' && buf[index] <= 'z')//此句待改进
		{
			flag = true; //正在提取
			word[word_ind++] = buf[index];
		}
		if(flag && !(buf[index] >= 'a' && buf[index] <= 'z')) // 单个单词提取完毕
		{
			flag = false;
			word[word_ind] = '\0';
			word_ind = 0;
		
			// 将提取出的单词加入map中
			WordFre_Map::iterator ite = wfmap.begin();
			ite = wfmap.find(word);
			if(ite != wfmap.end()) //map中已存在该单词	
			{
				ite->second ++;
			}
			else //map中不存在该单词
			{
				std::pair<WordFre_Map::iterator, bool> ret = wfmap.insert(std::make_pair(word, 1));
				if(!ret.second)
				{
					std::cout << "add word to map error" << std::endl;
					exit(EXIT_FAILURE);
				}
			}		
		 }
	}// end of for	
}

//提取英文所用函数
//========================================


void PreDict::readMaptoDict(WordFre_Map &wfmap, bool enflag)
{
	char *dictpath = NULL;
	if(enflag)
		dictpath = dict_en_;
	else
		dictpath = dict_ch_;
		
	std::ofstream dict(dictpath, std::ios::out);
	
	if(!dict.good())
	{
		std::cout << "create dict error" << std::endl;
		exit(EXIT_FAILURE);	
	}		
	WordFre_Map::iterator ite = wfmap.begin();
	while(ite != wfmap.end())
	{
		dict << ite->first << '\t' << ite->second << '\n';
		ite ++;
	}
	dict.close();
}


}//end of namespace
