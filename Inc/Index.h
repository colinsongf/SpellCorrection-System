#ifndef __INDEX_H__
#define __INDEX_H__

#include <map>
#include <string>
#include <vector>



namespace tanfy
{

class Dict;

class Index
{
public:
	typedef std::map<std::string, std::vector<int> > IndexMap;

	static Index *getInstance(Dict &dict);	

	IndexMap &getIndexMap();
	bool getPosinVector(std::string word, std::vector<int> &posVec);
	void displayMap();

private:
	Index(Dict &dict);
	
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
	IndexMap indexMap_;
	static Index *pInstance_;
	static AutoRelease autoRelease_;
};


}//end of namespace




#endif
