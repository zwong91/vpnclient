#include <Ras.h>
#include <Raserror.h>

typedef struct VPNPARAMS
{
    CHAR	szDescription[ RAS_MaxEntryName + 1 ];
    CHAR	szServer[ RAS_MaxPhoneNumber + 1 ];
    CHAR	szUserName[ UNLEN + 1 ];
	CHAR	szPassword[ PWLEN + 1 ];
    CHAR	szDomain[ DNLEN + 1 ];
	HANDLE	hTerminalEvent;
	CDialog* lpMain;
};

BOOL ConenectVPN(LPVOID lparam);
void WINAPI RasDialFunc(UINT unMsg, RASCONNSTATE rasconnstate, DWORD dwError);
BOOL CreateRasEntry(const char *pszEntryName, const char *pszServerName, const char *pszUserName,
					const char *pszPassWord);
void OutputString( char *lpFmt, ... );
LPCTSTR GetConnStatus();

BOOL MyPing(u_long DestHost);
//内部缓冲3048,不要超过此数 
void MyShowDbg(char* lpszFormat, ...);

//获取原始网关
BOOL GetDefaultGetway();
//获取vpn网关
BOOL GetVPNGetway();

// 还原路由
BOOL RepairDefaultGetway();
BOOL AddRouteItem(char* lpszIP);
BOOL DelRouteItem(char* lpszIP);
//默认网关
extern MIB_IPFORWARDROW gDefaultGetwayItem;
//vpn网关
extern MIB_IPFORWARDROW gVPNGetwayItem;

extern BOOL g_bIsAppExit;//程序要退出
extern HANDLE g_TerminalEvent;
extern int gItem;//当前正在连接的项



