//
//// stdafx.h : 标准系统包含文件的包含文件，
//// 或是经常使用但不常更改的
//// 特定于项目的包含文件
//
//#pragma once
//
//#ifndef _SECURE_ATL
//#define _SECURE_ATL 1
//#endif
//
//#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
//#endif
//
//#include "targetver.h"
//
//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的
//
//// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
//#define _AFX_ALL_WARNINGS
//
//#include <afxwin.h>         // MFC 核心组件和标准组件
//#include <afxext.h>         // MFC 扩展
//
//
//#include <afxdisp.h>        // MFC 自动化类
//
//
//
//#ifndef _AFX_NO_OLE_SUPPORT
//#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
//#endif
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
//#endif // _AFX_NO_AFXCMN_SUPPORT
//
//#include "SkinH.h"
//#pragma comment(lib,"SkinH.lib")
//
//#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#if !defined(AFX_STDAFX_H__DB0917CA_805B_4295_AE82_843E03BFC367__INCLUDED_)
#define AFX_STDAFX_H__DB0917CA_805B_4295_AE82_843E03BFC367__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifndef _WIN32_IE // 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0500 //为 IE 5.0 及更新版本改变为适当的值。
#define WINVER 0x0501
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <ras.h>
#pragma comment(lib, "rasapi32.lib")

///////////////界面皮肤///////////////
#include "SkinH.h"
#pragma comment(lib,"SkinH.lib")
////////////////END//////////////////

//////////////////////////////////////////////////////////////////////////////////
//链接代码

#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#ifndef _DEBUG
#pragma comment (lib,"../Lib/LibCurl_X64.lib")
#pragma comment (lib,"../Lib/LibJson_X64.lib")
#else
#pragma comment (lib,"../Lib/LibCurl_X64D.lib")
#pragma comment (lib,"../Lib/LibJson_X64D.lib")
#endif  // _DEBUG
#else
#ifndef _DEBUG
#pragma comment (lib,"../Lib/LibCurl.lib")
#pragma comment (lib,"../Lib/LibJson.lib")
#else
#pragma comment (lib,"../Lib/LibCurlD.lib")
#pragma comment (lib,"../Lib/LibJsonD.lib")
#endif
#endif

#define LUAFORMAT_UTF8
#include "MD5.h"
#include "json.h"
#include "curl.h"
#include "Transcoding.h"

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DB0917CA_805B_4295_AE82_843E03BFC367__INCLUDED_)

