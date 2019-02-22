#pragma once


//转码基类
class CTranscoding
{
public:
	//lua的字符串转换成程序识别字符串
	static std::string		LuaString_Procedure(const char* ch);
	static void			LuaString_Procedure(TCHAR* pOutCh, LONG maxlen, const char* ch);	//解决SEH问题

	//程序字符串转换成lua识别字符串
	static std::string	Procedure_LuaString(TCHAR* ch);
	static void			Procedure_LuaString(char* pOutCh, LONG maxlen, TCHAR* ch);			//解决SEH问题

	//转换输入的unicode字符串为LUA代码层识别字符串
	static std::string	TcharToChar(const TCHAR * tchar);

	//LUA代码层的字符串转换成tchar字符串
	static std::string		CharToTchar(const char * ch);
};
