#ifndef __WORDTASK_H__
#define __WORDTASK_H__

#include "Task.h"
#include <string>
#include <queue>


namespace tanfy
{

class ThreadPool;

class TcpConnection;

typedef struct word_inf
{
	std::string word;
	int edit_dis;
	int freq;
}WordInf;

const int maxwordlen = 100;

class WordTask:public Task
{
public:
	WordTask(ThreadPool &thrpool, TcpConnection *tcpConn, std::string recvword, int sendNum);
	void process(); //线程池中处理任务

private:
	int calEditDis(std::string sourceword);


private:
	int sendNum_;
	std::priority_queue<WordInf> sendQue_;
};

}//end of namespace

#endif
