#include "../Lib/cppjieba/src/Jieba.hpp"
#include "../Inc/Conf.h"
#include "../Inc/PreDict.h"
#include <iostream>


int main()
{
	cppjieba::Jieba mycut("../Lib/cppjieba/dict/jieba.dict.utf8",
						  "../Lib/cppjieba/dict/hmm_model.utf8",
						  "../Lib/cppjieba/dict/user.dict.utf8");

	char *dict_en = "/home/fiona/PROJECT@WANGDAO/c++/SpellCorrection/Data/dict_en.dat";
	char *sourceconf_en = "/home/fiona/PROJECT@WANGDAO/c++/SpellCorrection/Conf/dict_en_conf.txt";
	char *dict_ch = "/home/fiona/PROJECT@WANGDAO/c++/SpellCorrection/Data/dict_ch.dat";
	char *sourceconf_ch = "/home/fiona/PROJECT@WANGDAO/c++/SpellCorrection/Conf/dict_ch_conf.txt";

	tanfy::PreDict mydict(mycut, 3, dict_en, sourceconf_en, 23, dict_ch, sourceconf_ch);
	mydict.createDictEn();
	mydict.createDictCh();
}
