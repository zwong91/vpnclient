#include <iostream>
#include <string.h>
#include <vector>
#pragma once
class Util
{
public:
	Util(void);
	~Util(void);
	 std::vector<std::string> split(std::string str,std::string pattern);
};

