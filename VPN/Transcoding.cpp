#include "stdafx.h"
#include "Transcoding.h"


// UTF8编码转为多字节编码  
bool UTF8_MB(std::string& pmb, const char* pu8)
{
	// convert an UTF8 string to widechar   
	LONG nLen = MultiByteToWideChar(CP_UTF8, 0, pu8, (int)(strlen(pu8)), NULL, 0);

	WCHAR* lpszW = NULL;
	try
	{
		lpszW = new WCHAR[nLen + 1];
		lpszW[nLen]=0;
	}
	catch (std::bad_alloc&)
	{
		return false;
	}

	LONG nRtn = MultiByteToWideChar(CP_UTF8, 0, pu8, (int)(strlen(pu8)), lpszW, nLen);

	if (nRtn != nLen)
	{
		delete[] lpszW;
		return false;
	}

	// convert an widechar string to Multibyte   
	LONG MBLen = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, NULL, 0, NULL, NULL);
	if (MBLen <= 0)
	{
		delete[] lpszW;
		return false;
	}
	
	char* pOut = new char[MBLen + 1];
	pOut[MBLen]=0;
	nRtn = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, pOut, MBLen, NULL, NULL);
	delete[] lpszW;

	if (nRtn != MBLen)
	{
		delete[]pOut;
		pmb.clear();
		return false;
	}
	pmb = pOut;
	delete[]pOut;
	return true;
}

// 多字节编码转为UTF8编码  
bool MB_UTF8(std::string& pu8, const char* pmb)
{
	// convert an MBCS string to widechar   
	LONG nLen = MultiByteToWideChar(CP_ACP, 0, pmb, (int)(strlen(pmb)), NULL, 0);

	WCHAR* lpszW = NULL;
	try
	{
		lpszW = new WCHAR[nLen + 1];
		lpszW[nLen]=0;
	}
	catch (std::bad_alloc&)
	{
		return false;
	}

	LONG nRtn = MultiByteToWideChar(CP_ACP, 0, pmb, (int)(strlen(pmb)), lpszW, nLen);
	if (nRtn != nLen)
	{
		delete[] lpszW;
		return false;
	}
	// convert an widechar string to utf8  
	LONG utf8Len = WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, NULL, 0, NULL, NULL);
	if (utf8Len <= 0)
	{
		delete[] lpszW;
		return false;
	}

	char* pOut = new char[utf8Len + 1];	
	pOut[utf8Len]=0;
	nRtn = WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, pOut, utf8Len, NULL, NULL);

	delete[] lpszW;
	if (nRtn != utf8Len)
	{
		delete[]pOut;
		pu8.clear();
		return false;
	}
	pu8 = pOut;
	delete []pOut;
	return true;
}

// 多字节编码转为Unicode编码  
bool MB_Unicode(std::wstring& pun, const char* pmb)
{
	// convert an MBCS string to widechar   
	LONG uLen = MultiByteToWideChar(CP_ACP, 0, pmb, (int)(strlen(pmb)), NULL, 0);

	if (uLen <= 0)
	{
		return false;
	}

	wchar_t* pUnicode;
	pUnicode = new wchar_t[uLen + 1];	
	pUnicode[uLen] = 0;
	LONG nRtn = MultiByteToWideChar(CP_ACP, 0, pmb, (int)(strlen(pmb)), pUnicode, uLen);

	if (nRtn != uLen)
	{
		delete[]pUnicode;
		pun.clear();
		return false;
	}

	pun = pUnicode;
	delete[]pUnicode;
	return true;
}

//utf-8转unicode
bool UTF8_Unicode16(std::wstring& pmb, const char* utf)
{
	if (!utf || !strlen(utf))
	{
		return false;
	}
	int dwUnicodeLen = MultiByteToWideChar(CP_UTF8, 0, utf, -1, NULL, 0);

	wchar_t* pUnicode;
	pUnicode = new wchar_t[dwUnicodeLen + 1];
	pUnicode[dwUnicodeLen]=0;	
	int nRet = MultiByteToWideChar(CP_UTF8, 0, utf, -1, pUnicode, dwUnicodeLen);
	if (nRet != dwUnicodeLen)
	{
		delete[]pUnicode;
		pmb.clear();
		return false;
	}	
	
	pmb = pUnicode;
	delete[]pUnicode;
	return true;
}

//unicode转utf-8
bool Unicode16_UTF8(std::string &pmb, const wchar_t* unicode)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);	
	char* pChar;
	pChar = new char[len + 1];
	pChar[len]=0;
	int nRet = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, pChar, len, NULL, NULL);
	if (nRet != len)
	{
		delete[]pChar;
		pmb.clear();
		return false;
	}
	
	pmb = pChar;
	delete[]pChar;
	return true;
}

////lua的字符串转换成程序识别字符串
std::string	CTranscoding::LuaString_Procedure(const char* ch)
{
#ifdef LUAFORMAT_UTF8
	#ifdef _UNICODE
		std::wstring str;
		UTF8_Unicode16(str, ch);
		return str;
	#else
		std::string str;
		UTF8_MB(str, ch);
		return str;	
	#endif
#else
	#ifdef _UNICODE
		std::wstring str;
		MB_Unicode(str, ch);
		return str;
	#else
	return ch;
	#endif
#endif
}

void CTranscoding::LuaString_Procedure(TCHAR* pOutCh, LONG maxlen, const char* ch)
{
	std::string str = LuaString_Procedure(ch);
	lstrcpyn(pOutCh, str.c_str(), maxlen);
}

//程序字符串转换成lua识别字符串
std::string	CTranscoding::Procedure_LuaString(TCHAR* ch)
{
#ifdef LUAFORMAT_UTF8
	#ifdef _UNICODE
		std::string str;
		Unicode16_UTF8(str, ch);
		return str;
	#else
		std::string str;
		MB_UTF8(str, ch);
		return str;		
	#endif
#else
	#ifdef _UNICODE
		std::string str;
		Unicode16_UTF8(str, ch);
		return str;
	#else
		return ch;
	#endif
#endif
}

void CTranscoding::Procedure_LuaString(char* pOutCh, LONG maxlen, TCHAR* ch)
{
	std::string str = Procedure_LuaString(ch);
	lstrcpynA(pOutCh, str.c_str(), maxlen);
}



std::string CTranscoding::TcharToChar(const TCHAR * tchar)
{
#ifdef _UNICODE
	LONG nLen = (LONG)(wcslen(tchar) + 1);
	if (nLen == 1)
		return std::string();

	char	*pTemp = new char[nLen * 2];
	wcstombs(pTemp, tchar, nLen * 2);
	std::string str = pTemp;
	delete[]pTemp;
	return str;
#else
	return tchar;
#endif
}


std::string CTranscoding::CharToTchar(const char * ch)
{
#ifdef _UNICODE
	LONG nLen = (LONG)(strlen(ch) + 1);
	if (nLen == 0)
		return std::wstring();

	wchar_t	*pTemp = new wchar_t[nLen];
	::setlocale(LC_CTYPE, "chs");
	mbstowcs(pTemp, ch, nLen);	
	std::wstring str = pTemp;
	delete[]pTemp;
	return str;
#else
	return ch;
#endif
}

