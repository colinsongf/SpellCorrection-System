#ifndef __PREDICT_H__
#define __PREDICT_H__

#include "../Lib/cppjieba/src/Jieba.hpp"
#include "Conf.h"
#include <map>
#include <string>
#include <unordered_set>


namespace tanfy
{

class PreDict
{
public:
	typedef std::map<std::string, int> WordFre_Map;
	
	PreDict(cppjieba::Jieba &mycut, int source_num_en, const char *dict_en, const char *sourceconf_path_en, int source_num_ch, const char *dict_ch, const char *sourceconf_path_ch);
	~PreDict();
	
	void createDictEn();//建立英文词典
	void createDictCh();//建立中文词典
	
private:
	void getSourcePath(char *source[], bool enflag);//提取英文true, 中文false
	void extractWordtoMap(const char *source, WordFre_Map &wordfreMap, bool enflag);
	void readMaptoDict(WordFre_Map &wfmap, bool enflag); 

	void dealWord(char *buf, int count);
	void addWordtoMap(char *buf, int count, WordFre_Map &wfmap);

	void addChtoMap(const char *source, WordFre_Map &wfmap);

private:
	cppjieba::Jieba &mycut_;
	int source_num_en_;
	int source_num_ch_;
	char *dict_en_;
	char *dict_ch_;
	Conf source_conf_en_; 
	Conf source_conf_ch_;	
	std::unordered_set<std::string> stopWords_;
};



}//end of namespace





#endif 
