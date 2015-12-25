#include "../Inc/WordTask.h"
#include "../Inc/TcpConnection.h"
#include "../Inc/ThreadPool.h"
#include "../Inc/Thread.h"
#include "../Inc/Dict.h"
#include "../Inc/Cache.h"
#include "../Inc/Index.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace tanfy
{

bool operator <(const WordInf &lhs, const WordInf &rhs)
{
	if(lhs.edit_dis > rhs.edit_dis)
		return true;	
	else if(lhs.edit_dis < rhs.edit_dis)
		return false;
	else if(lhs.freq != rhs.freq)
	{
		return (lhs.freq < rhs.freq) ? true:false;
	}
	else
	{
		return (lhs.word < rhs.word) ? true:false;
	}	
}

//获取单个中文字的逻辑长度
extern size_t getWordLen(const char word)
{
	size_t bytelen = 0;
	
	if(word & (1<<7))
	{
		if(((word & 0xF0) == 0xD0) || ((word & 0xF0) == 0xC0))//2个字节
			bytelen = 2;
		if((word & 0xF0) == 0xE0)//3个字节
			bytelen = 3;
		if((word & 0xF8) == 0xF0)//4个字节
			bytelen = 4;
		if((word & 0xFC) == 0xF8)//5个字节
			bytelen = 5;
		if((word & 0xFE) == 0xFC)//6个字节
			bytelen = 6;	
	}	
	else//1个字节
	{
		bytelen = 1;	
	}
	return bytelen;
}


int strLength(std::string str)
{
	int num = 0;
	for(int idx = 0; idx < str.size(); )
	{
		size_t strbyte = tanfy::getWordLen(str[idx]);
		num ++;
		idx += strbyte;
	}
	return num;
}

extern void delSameWord(char *sendbuf, const char *recvbuf, int count)
{
//	std::cout << "delSameWord()" << std::endl;//test
	char tmpbuf[98] = {0};
	int tmpnum = 0;
	bool flag;
	for(int idx = 0; idx < count;)
	{
		flag = false;
		int tmpidx;
		int tmpchecklen = strlen(tmpbuf);
		int recvlen = tanfy::getWordLen(recvbuf[idx]);
		//查找到相等的字符就跳出子循环
		for(tmpidx=0; tmpidx < tmpchecklen;)
		{
			flag = true;
			//判断recvbuf[idx]和tmpbuf[tmpidx]的长度
			int tmplen = tanfy::getWordLen(tmpbuf[tmpidx]);
			//长度相等则比较两个字符，查看是否重复
			if(recvlen == tmplen)
			{
				for(int lenidx = 0; lenidx < tmplen; ++lenidx)
				{
					if(recvbuf[idx+lenidx] != tmpbuf[tmpidx+lenidx])
					{
						flag = false;//此字符不相等
						tmpidx += tmplen;
						break;	
					}
				}
			}
			else
			{
				flag = false;
				tmpidx += tmplen;
			}
			if(flag)
				break;
		}
		if(!flag)
		{
			strncat(tmpbuf, recvbuf+idx, recvlen);
		}
		idx += recvlen;		
	}

	strcpy(sendbuf, tmpbuf);	
//	std::cout << "delsame完成" << std::endl;//test
}



//======================================
//class WordTask
//======================================
WordTask::WordTask(ThreadPool &thrpool, TcpConnection *tcpConn, std::string recvword, int sendNum)
	:tanfy::Task(thrpool, tcpConn, recvword),
	 sendNum_(sendNum)
{
}

void WordTask::process()
{
	//=================================
	//	在所在线程中的Cache中搜索
	//=================================
	std::map<std::string, std::string> &resultMap = pth_->getCache().getMap();

	std::map<std::string, std::string>::iterator itm = resultMap.find(recvword_);
	if(itm != resultMap.end())//在cache中找到该结果
	{
		sendword_ = itm->second + '\n';
		sendToClient();
		return;	
	}

	//====================================
	//通过index，从词典中查询
	//====================================
	Index &myindex = thrpool_.getIndex();	
	std::vector<std::pair<std::string, int> > &dictVec = thrpool_.getDict().getVector();
	
	//0. 根据recvword_,提取出index中的vector<int>位置序列
	const char *tmpbuf = recvword_.c_str();
	//删除相同字母
	char recvbuf[256] = {0};
	tanfy::delSameWord(recvbuf, tmpbuf, strlen(tmpbuf));

	std::set<int> checkRepeatSet;
	for(int idx = 0; idx < strlen(recvbuf); )//遍历recvword_中的每一个字母
	{
		int recvlen = tanfy::getWordLen(recvbuf[idx]);
		std::vector<int> posVec;
		bool findflag = myindex.getPosinVector(std::string(recvbuf, idx, recvlen), posVec);
		if(!findflag)
		{
			sendword_ = "未搜索到相近词汇";
			sendword_ += '\n';
			sendToClient();
			return;
		}

		// 根据vector<int>中的位置信息，遍历词典
		std::vector<int>::iterator itv = posVec.begin();
		for(; itv != posVec.end(); ++itv)
		{
			std::pair<std::set<int>::iterator, bool> ret = checkRepeatSet.insert(*itv);
			if(!ret.second)//该位置的单词已检测过
			{
				continue;
			}
		
			//edit-distance计算	
			int edit_dis = calEditDis(dictVec[*itv].first);			

			//加入优先级队列
			if(edit_dis < 10) //当最小编辑距离在10之内才加入队列中
			{
				WordInf wordtoque;
				wordtoque.word = dictVec[*itv].first;
				wordtoque.edit_dis = edit_dis;
				wordtoque.freq = dictVec[*itv].second;
				sendQue_.push(wordtoque);				
			}
		}
		idx += recvlen;	
	}

#if 0
	//==================================
	//	直接从词典中查询
	//==================================
	std::vector<std::pair<std::string, int> > &dictVec = thrpool_.getDict().getVector();
	std::vector<std::pair<std::string, int> >::iterator ite = dictVec.begin();
	
	//0. 遍历vector中数据，获取每个数据的edit_distance，并提取出来放入优先级队列中
	int edit_dis;
	for(; ite != dictVec.end(); ++ite)
	{
		//0.1 edit-distance计算	
		edit_dis = calEditDis(ite->first);			
		//0.2 加入优先级队列
		WordInf wordtoque;
		wordtoque.word = ite->first;
		wordtoque.edit_dis = edit_dis;
		wordtoque.freq = ite->second;
		sendQue_.push(wordtoque);				
	}
#endif
	//1. 提取优先级队列中的前sendNum个,传给客户端
	std::string sendbuf;
	int idx;
	int finalSendNum = (sendQue_.size() < sendNum_) ? sendQue_.size():sendNum_;
	for(idx = 0; idx < finalSendNum; ++idx)
	{
		sendbuf  = sendbuf + sendQue_.top().word + "   "; 
		sendQue_.pop();
	}
	//2. 向线程的Cache中添加数据
	if(finalSendNum > 0)
		pth_->getCache().addDatatoMap(recvword_, sendbuf);	
	else if(0 == finalSendNum)
		sendbuf = "未搜索到相近词汇";

	sendbuf += '\n';
	sendword_ = sendbuf;
	sendToClient();
}


int WordTask::calEditDis(std::string sourceword)
{
	int target_len = recvword_.size();
	int source_len = sourceword.size();
	int dis[maxwordlen] [maxwordlen] = {0};
	int i, j;
	int tar_idx, sou_idx;
	int pend1, pend2, pend3;

	int tarstrlen = tanfy::strLength(recvword_);
	int soustrlen = tanfy::strLength(sourceword);
	for(i = 0; i <= tarstrlen; ++i)
	{
		for(j = 0; j <= soustrlen; ++j)
		{
			if(i == 0 || j == 0)
				dis[i][j] = i*1 + j*1;
		}
	}

	for(tar_idx = 0, i = 1; tar_idx < target_len; ++i)
	{
		size_t target_byte = tanfy::getWordLen(recvword_[tar_idx]);
		for(sou_idx = 0, j = 1; sou_idx < source_len; ++j)
		{
			size_t source_byte = tanfy::getWordLen(sourceword[sou_idx]); 

			std::string subtarget = recvword_.substr(tar_idx, target_byte);
			std::string subsource = sourceword.substr(sou_idx, source_byte);
			pend1 = dis[i-1] [j] + 1;
			pend2 = dis[i-1] [j-1] + (subtarget == subsource ? 0:2);
			pend3 = dis[i][j-1] + 1;			
			dis[i][j] = pend1 < pend2 ?((pend1 < pend3)?pend1:pend3):((pend2<pend3) ? pend2:pend3);
			sou_idx += source_byte;
		}
		tar_idx += target_byte;
	}
	return dis[tarstrlen][soustrlen];		
}

}//end of namespace
