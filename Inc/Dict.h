#ifndef __DICT_H__
#define __DICT_H__

#include <vector>
#include <map>
#include <string>
#include "../Inc/Noncopyable.h"

namespace tanfy
{

class Conf;

class Dict: Noncopyable
{
public:
	typedef std::vector<std::pair<std::string, int> > DictVector;

	static Dict* getInstance(Conf &conf);

	DictVector &getVector();

private:
	Dict(Conf &conf);

private:
	class AutoRelease
	{
	public:
		~AutoRelease()
		{
			if(pInstance_)
			{
				delete pInstance_;
				pInstance_ = NULL;
			}
		}
	};

private:
	Conf &conf_;
	DictVector vecDict_;
	static Dict *pInstance_;
	static AutoRelease autoRelease_;
};

}//end of namespace





#endif
