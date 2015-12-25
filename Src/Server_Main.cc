#include "../Inc/TcpServer.h"
#include "../Inc/ThreadPool.h"
#include "../Inc/Task.h"
#include "../Inc/WordTask.h"
#include "../Inc/Conf.h"
#include "../Inc/Dict.h"
#include "../Inc/Index.h"
#include "../Inc/Thread.h"
#include "../Inc/TimerThread.h"
#include <iostream>
#include <string>

//将大写改成小写，去掉空格(搜索引擎项目合并时去空格可以省略，因为纠错前已分词完成)
std::string dealWord(std::string word);

void myConnection(tanfy::TcpConnection *pConn);
void myTask(tanfy::TcpConnection *pConn);
void myClose(tanfy::TcpConnection *pConn);


int main()
{
//========================================
//准备
//========================================
	//0. 读取词典内容到内存中
	std::cout << "//===========================" << std::endl;
	std::cout << "//0.0 服务器正在初始化..." << std::endl;

	tanfy::Conf myconf("/home/fiona/PROJECT@WANGDAO/c++/SpellCorrection/Conf/conf.txt");
	tanfy::Dict *pmydict = tanfy::Dict::getInstance(myconf);
	tanfy::Index *pmyindex = tanfy::Index::getInstance(*pmydict);

	//1. 获取配置信息：服务器ip地址/port
	const char *ip = myconf.getIp();
	unsigned short port = myconf.getPort();

//=========================================
//开工
//=========================================
	//0. 线程池/服务器初始化
	tanfy::ThreadPool thrpool(10, 10, *pmydict, myconf, *pmyindex);
	tanfy::TcpServer myserver(thrpool,  port); 
	tanfy::Thread *mytimer = new tanfy::TimerThread (std::bind(&tanfy::ThreadPool::updateCache, &thrpool), 180, 60);

	myserver.setConnectionCb(myConnection);
	myserver.setMessageCb(myTask);
	myserver.setCloseCb(myClose);	

	std::cout << "//0.0 服务器已成功启动" << std::endl;	
	std::cout << "//===========================" << std::endl;

	//1. 启动服务器/线程池/定时器
	thrpool.start(); // 必须先启动线程池：保证客户端任务来时已经有线程池在运行
	mytimer->start();//定时器启动,必须在服务器之前启动，否则服务器loop,根本轮不到定时器执行
	myserver.start(); //开始监听客户端的链接 以及 发送的信息	
}



std::string dealWord(std::string word)
{
	std::string str;
	for(int idx = 0; idx < word.size(); ++idx)
	{
		if(word[idx] <= 'Z' && word[idx] >= 'A')
			word[idx] += 32;

		if(word[idx] != ' ')
			str += word[idx];
	}	
	return str;
}


void myConnection(tanfy::TcpConnection *pConn)
{
	std::string str = pConn->toString();	
	std::cout << str << " 已连接" << std::endl;
	pConn->sendmsg("欢迎使用文本纠错系统");
}

void myTask(tanfy::TcpConnection *pConn)
{
	std::string wordmsg = pConn->recvmsg();
	std::string dealmsg = dealWord(wordmsg);
	std::cout << "待处理数据: " << dealmsg << std::endl;
	//1. 将单词包装成Task
	tanfy::Task *pMytask = new tanfy::WordTask(pConn->getThrpool(), pConn, dealmsg, 5);
	//2. 将Task丢入线程池中
	pMytask->addToThreadpool();
}

void myClose(tanfy::TcpConnection *pConn)
{
	std::string str = pConn->toString();
	std::cout << str << " 已退出" << std::endl;
}

