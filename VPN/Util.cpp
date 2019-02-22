#include "StdAfx.h"
#include "Util.h"


Util::Util(void)
{
}


Util::~Util(void)
{
}

//�ַ����ָ��
std::vector<std::string> Util::split(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str+=pattern;//��չ�ַ����Է������
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

