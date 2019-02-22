
#include "StdAfx.h"
#include "resource.h"
#include "RasDial.h"
#include "vpnDlg.h"
#include "MySocket.h"

LPCTSTR ConnStatus;
HANDLE g_TerminalEvent = NULL;
int gItem = -1;//当前正在连接的项
BOOL g_bIsAppExit = FALSE;//程序要退出了！
BOOL gIsRasSucess = FALSE;

//output the debug infor into log file & DbgPrint
void OutputString( char *lpFmt, ... )
{
    char buff[1024];
    va_list    arglist;
    va_start( arglist, lpFmt );
    _vsnprintf( buff, sizeof buff, lpFmt, arglist );
    va_end( arglist );
	ConnStatus= buff;
	SetDlgItemText(AfxGetApp()->m_pMainWnd->m_hWnd,IDC_STATUS,buff);
}
LPCTSTR GetConnStatus()
{
	return ConnStatus;
}

BOOL CreateRasEntry(const char *pszEntryName, const char *pszServerName, const char *pszUserName,
					const char *pszPassWord)
{
	RASENTRY rasEntry;
	DWORD rasEntrySize, dwResult;
	
	rasEntrySize = sizeof(rasEntry);
	memset(&rasEntry, 0, sizeof(rasEntry));
	rasEntry.dwType = RASET_Vpn;
	rasEntry.dwRedialCount = 30000;
	rasEntry.dwfNetProtocols=RASNP_Ip;
	rasEntry.dwRedialPause = 1;
	//rasEntry.dwFramingProtocol=RASFP_Ppp;
	rasEntry.dwEncryptionType=ET_Optional; //可选加密

	rasEntry.dwSize = sizeof(rasEntry);
	rasEntry.dwfOptions = RASEO_RequireMsEncryptedPw | RASEO_RequireCHAP | RASEO_RemoteDefaultGateway  | RASEO_ModemLights | RASEO_ShowDialingProgress;
	rasEntry.dwfOptions2 = RASEO2_ReconnectIfDropped; //断线重播
	lstrcpy(rasEntry.szLocalPhoneNumber, pszServerName);
	lstrcpy(rasEntry.szDeviceType, RASDT_Vpn);
	lstrcpy(rasEntry.szDeviceName, TEXT("RAS VPN Line 0"));
	//rasEntry.dwFramingProtocol = RASFP_Ppp;
	//rasEntry.dwfNetProtocols = RASNP_Ip;
	//rasEntry.dwVpnStrategy = VS_PptpOnly;        //设置pptp连接属性

	dwResult = RasSetEntryProperties(NULL, pszEntryName, &rasEntry, sizeof(rasEntry), NULL, 0);
	if (dwResult != 0)
	{
		OutputString("RasSetEntryProperties %s拨号失败!(ERROR：%d)\n", pszEntryName, dwResult);
		return FALSE;
	}
	RASDIALPARAMS rdParams;
	
	ZeroMemory(&rdParams, sizeof(RASDIALPARAMS));
	rdParams.dwSize = sizeof(RASDIALPARAMS);
	lstrcpy(rdParams.szEntryName, pszEntryName);
	lstrcpy(rdParams.szUserName, pszUserName);
	lstrcpy(rdParams.szPassword, pszPassWord);
	DWORD dwRet = RasSetEntryDialParams(NULL, &rdParams, FALSE);

	if(dwRet == 0)
		return TRUE;
	else 
		return FALSE;
}

// Callback function RasDialFunc()

void WINAPI RasDialFunc(UINT unMsg, RASCONNSTATE rasconnstate, DWORD dwError)
{
	char szRasString[256]; // Buffer for storing the error string

	if (dwError)  // Error occurred
	{
		RasGetErrorString((UINT)dwError, szRasString, 256);
		OutputString("ERROR:%d - %s\n",dwError, szRasString);
		SetEvent(g_TerminalEvent);
		return;
	}

	// Map each of the states of RasDial() and display on the screen
	// the next state that RasDial() is entering
	switch (rasconnstate)
	{
		// Running States
		case RASCS_OpenPort:
			OutputString ("打开端口...\n");
			break;
		case RASCS_PortOpened:
			OutputString ("端口打开成功.\n");
        	break;
		case RASCS_ConnectDevice: 
			OutputString ("连接设备...\n");
			break;
		case RASCS_DeviceConnected: 
			OutputString ("设备连接成功.\n");
			break;
		case RASCS_AllDevicesConnected:
			OutputString ("All devices connected.\n");
			break;
		case RASCS_Authenticate: 
			OutputString ("验证账户...\n");
			break;
		case RASCS_AuthNotify:
			OutputString ("Authentication notify.\n");
			break;
		case RASCS_AuthRetry: 
			OutputString ("再次验证账户...\n");
			break;
		case RASCS_AuthCallback:
			OutputString ("Authentication callback...\n");
			break;
		case RASCS_AuthChangePassword: 
			OutputString ("请修改密码...\n");
			break;
		case RASCS_AuthProject: 
			OutputString ("Projection phase started...\n");
			break;
		case RASCS_AuthLinkSpeed: 
			OutputString ("Negotiating speed...\n");
			break;
		case RASCS_AuthAck: 
			OutputString ("Authentication acknowledge...\n");
			break;
		case RASCS_ReAuthenticate: 
			OutputString ("Retrying Authentication...\n");
			break;
		case RASCS_Authenticated: 
			OutputString ("Authentication complete.\n");
			break;
		case RASCS_PrepareForCallback: 
			OutputString ("Preparing for callback...\n");
			break;
		case RASCS_WaitForModemReset: 
			OutputString ("Waiting for modem reset...\n");
			break;
		case RASCS_WaitForCallback:
			OutputString ("Waiting for callback...\n");
			break;
		case RASCS_Projected:  
			OutputString ("Projection completed.\n");
			break;
	#if (WINVER >= 0x400) 
		case RASCS_StartAuthentication:
			OutputString ("Starting authentication...\n");
            break;
		case RASCS_CallbackComplete: 
			OutputString ("Callback complete.\n");
			break;
		case RASCS_LogonNetwork:
			OutputString ("Logon to the network.\n");
			break;
	#endif 
		case RASCS_SubEntryConnected:
			OutputString ("Subentry connected.\n");
			break;
		case RASCS_SubEntryDisconnected:
			OutputString ("Subentry disconnected.\n");
			break;

		// The RAS Paused States will not occur because
		// we did not use the RASDIALEXTENSIONS structure
		// to set the RDEOPT_PausedState option flag.

		// The Paused States are:

		// RASCS_RetryAuthentication:
		// RASCS_CallbackSetByCaller:
		// RASCS_PasswordExpired:

		// Terminal States
		case RASCS_Connected: 
			OutputString ("连接成功\n");

			gIsRasSucess = TRUE;
			SetEvent(g_TerminalEvent);
			break;
		case RASCS_Disconnected: 
			OutputString ("Disconnecting...\n");
			SetEvent(g_TerminalEvent);
			gIsRasSucess = FALSE;
			break;
		default:
			OutputString ("Unknown Status = %d\n", rasconnstate);
			break;
	}
}


//BOOL ConenectVPN(VPNPARAMS VPNParams)
BOOL ConenectVPN(LPVOID lparam)
{
	RASDIALPARAMS	RasDialParams;
	RASCONNSTATUS	RasConnStatus;
	HRASCONN		hRasConn;
	DWORD			Ret;
	DWORD			tcLast;

	VPNPARAMS VPNParams;
	memcpy(&VPNParams, lparam, sizeof(VPNPARAMS));
	
	// Create the event that indicates a terminal state
	//if ((g_TerminalEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
	//{
	//	OutputString("CreateEvent failed with error %d\n", GetLastError());
	//	return FALSE;
	//}
	
	RasDialParams.dwSize = sizeof(RASDIALPARAMS);
	lstrcpy(RasDialParams.szEntryName, VPNParams.szDescription);
	lstrcpy(RasDialParams.szPhoneNumber, VPNParams.szServer);
	lstrcpy(RasDialParams.szUserName, VPNParams.szUserName);
	lstrcpy(RasDialParams.szPassword, VPNParams.szPassword);
	lstrcpy(RasDialParams.szDomain, VPNParams.szDomain);
	
	AddRouteItem(VPNParams.szServer);

	while (TRUE)
	{
		BOOL bIsDisconnect = FALSE; //是否手动断开了.
		gIsRasSucess = FALSE;//假定拔号不成功！

		ResetEvent(g_TerminalEvent);
		//RasDialParams.szPhoneNumber
		CreateRasEntry(RasDialParams.szEntryName, RasDialParams.szPhoneNumber,
			RasDialParams.szUserName, RasDialParams.szPassword);

		// Dial out asynchronously using RasDial()
		OutputString("Dialing... %s\n", RasDialParams.szPhoneNumber);
		hRasConn = NULL;
		if (Ret = RasDial(NULL, NULL, &RasDialParams, 0, &RasDialFunc, &hRasConn))
		{
			OutputString("RasDial %s error %d\n", RasDialParams.szPhoneNumber, Ret);
			RasDeleteEntry(NULL, RasDialParams.szEntryName);
			return FALSE;
		}
		// Wait for RasDial to complete or enter a paused state
		HANDLE handles[2];
		handles[0] = g_TerminalEvent;
		handles[1] = VPNParams.hTerminalEvent;

		Ret = WaitForMultipleObjects(2, &handles[0], FALSE, INFINITE);
		if (Ret == WAIT_OBJECT_0 + 0)
		{
			if (gIsRasSucess)	//拔号成功
			{
				while (TRUE)
				{
					Ret = WaitForSingleObject(VPNParams.hTerminalEvent, 4000);
					if (Ret != WAIT_TIMEOUT)
					{
						bIsDisconnect = TRUE;// 手动已经断开
						break;
					}
					else
					{
						////检测vpn是否存活，ping虚拟地址 网络延时
						//if (!MyPing(gVPNGetwayItem.dwForwardNextHop))  //(SOCK_RAW,IPPROTO_ICMP)
						//{
						//	break;
						//}
						// 						CheckSocket.Socket(SOCK_STREAM, IPPROTO_TCP);
						// 						if(!CheckSocket.Connect(gVPNGetwayItem.dwForwardNextHop,1723))
						// 							break;
						// 						
						// 						CheckSocket.CloseSocket();
					}
				}
			}
			else				//拔号失败
			{
				OutputString("RasDial Timed out...\n");
			}
		}
		else if (Ret == WAIT_OBJECT_0 + 1)									//手动已断开
		{
			bIsDisconnect = TRUE;
		}
		else
		{
			bIsDisconnect = TRUE;
		}

		OutputString("Calling RasHangUp...\n");
		if (Ret = RasHangUp(hRasConn))
		{
			OutputString("RasHangUp error %d\n", Ret);
		}

		RasConnStatus.dwSize = sizeof(RASCONNSTATUS);

		tcLast = GetTickCount() + 10000;
		while ((RasGetConnectStatus(hRasConn, &RasConnStatus)
			!= ERROR_INVALID_HANDLE) && (tcLast > GetTickCount()))
		{
			Sleep(50);
		}


		OutputString("Connection to %s terminated.\n", RasDialParams.szPhoneNumber);
		RasDeleteEntry(NULL, RasDialParams.szEntryName);
		OutputString("Delete EntryName %s OK.\n", RasDialParams.szEntryName);
		OutputString("已断开");

		Sleep(1000);

		if (bIsDisconnect)
			break;
	}

	DelRouteItem(VPNParams.szServer);

	CloseHandle(g_TerminalEvent);
	g_TerminalEvent = NULL;

	return TRUE;
}


BOOL AddRouteItem(char* lpszDestIP)
{
	BOOL bRet = FALSE;

	u_long ip = MySocket::ginet_addr(lpszDestIP);

	if (0 != ip)
	{
		MIB_IPFORWARDROW MIBItem = gDefaultGetwayItem;
		MIBItem.dwForwardDest = ip;
		MIBItem.dwForwardMask = -1;

		if (NO_ERROR == CreateIpForwardEntry(&MIBItem))
		{
			bRet = TRUE;
		}
	}//

	if (!bRet)
	{
		MyShowDbg("添加路由 %s 失败,有可能已经存在!", lpszDestIP);
	}
	return bRet;
}

BOOL DelRouteItem(char* lpszIP)
{
	BOOL bRet = FALSE;

	u_long ip = MySocket::ginet_addr(lpszIP);

	if (0 != ip)
	{
		MIB_IPFORWARDROW MIBItem = gDefaultGetwayItem;
		MIBItem.dwForwardDest = ip;
		MIBItem.dwForwardMask = -1;

		if (NO_ERROR == DeleteIpForwardEntry(&MIBItem))
		{
			bRet = TRUE;
		}
	}//

	if (!bRet)
	{
		MyShowDbg("删除路由 %s 失败,有可能已经不存在!", lpszIP);
	}

	return bRet;
}

//获取原始网关
BOOL GetDefaultGetway()
{
	PMIB_IPFORWARDTABLE lpRouteTable;
	DWORD size = 0;
	BOOL dwRet = FALSE;
	GetIpForwardTable(NULL, &size, FALSE);

	while (TRUE)
	{
		if (0 != size)
		{
			lpRouteTable = (PMIB_IPFORWARDTABLE)new char[size * 2];

			if (NO_ERROR == GetIpForwardTable(lpRouteTable, &size, TRUE))
				dwRet = TRUE;
			else
			{
				dwRet = FALSE;
				break;
			}
		}

		//寻找默认网关
		for (DWORD i = 0; i < lpRouteTable->dwNumEntries; i++)
		{
			PMIB_IPFORWARDROW lpCurItem = &(lpRouteTable->table[i]);

			if (0 == lpCurItem->dwForwardMask && 0 == lpCurItem->dwForwardDest)
			{
				gDefaultGetwayItem = *lpCurItem;
				break;
			}
		}

		break;
	}

	if (!dwRet)
		MyShowDbg("备份原始路由失败!");
	else
		MyShowDbg("备份原始路由完成!");

	delete lpRouteTable;

	return dwRet;
}

//获取vpn网关
BOOL GetVPNGetway()
{
	PMIB_IPFORWARDTABLE lpRouteTable;
	DWORD size = 0;
	BOOL dwRet = FALSE;
	GetIpForwardTable(NULL, &size, FALSE);

	while (TRUE)
	{
		if (0 != size)
		{
			lpRouteTable = (PMIB_IPFORWARDTABLE)new char[size * 2];

			if (NO_ERROR == GetIpForwardTable(lpRouteTable, &size, TRUE))
				dwRet = TRUE;
			else
			{
				dwRet = FALSE;
				break;
			}
		}

		//寻找默认网关
		for (DWORD i = 0; i < lpRouteTable->dwNumEntries; i++)
		{
			PMIB_IPFORWARDROW lpCurItem = &(lpRouteTable->table[i]);

			if (0 == lpCurItem->dwForwardMask && 0 == lpCurItem->dwForwardDest)
			{
				gVPNGetwayItem = *lpCurItem;
				break;
			}
		}

		break;
	}

	if (!dwRet)
		MyShowDbg("备份VPN路由失败!");
	else
		MyShowDbg("备份VPN路由完成!");

	delete lpRouteTable;

	return dwRet;
}


// 还原路由
BOOL RepairDefaultGetway()
{
	BOOL bRet = FALSE;

	DeleteIpForwardEntry(&gVPNGetwayItem);
	bRet = CreateIpForwardEntry(&gDefaultGetwayItem);

	if (!bRet)
		MyShowDbg("还原始路由失败!");
	else
		MyShowDbg("还原始路由完成!");

	return bRet;
}

// 添加一条路由
// BOOL RepairDefaultGetway()
// {
// 	BOOL bRet = FALSE;
// 	
// 	CreateIpForwardEntry()
// 	DeleteIpForwardEntry(&gVPNGetwayItem);
// 	bRet = CreateIpForwardEntry(&gDefaultGetwayItem);
// 	
// 	if (!bRet)
// 		MyShowDbg("还原始路由失败!");
// 	else
// 		MyShowDbg("还原始路由完成!");
// 	
// 	return bRet;
// }


BOOL MyPing(u_long DestHost)
{
	//ping的目标
	sockaddr_in DestAddr = { 0 };
	DestAddr.sin_family = AF_INET;
	DestAddr.sin_addr.S_un.S_addr = (u_long)DestHost;//MySocket::ginet_addr(lpszDestHost);
	DestAddr.sin_port = MySocket::ghtons(0); //不使用端口

	MySocket hsk;
	if (!hsk.Socket(SOCK_RAW, IPPROTO_ICMP)) //icmp原始套接字
		return FALSE;

	//构造ICMP包 当发送缓冲区用
	PROTOCOL_HEAD::ICMPHEADER IcmpHead = { 0 };
	IcmpHead.i_type = 8;
	IcmpHead.i_code = 0;
	IcmpHead.i_cksum = 0;	//必须先赋0,最后再求校验和
	IcmpHead.i_id = 0;
	IcmpHead.i_seq = 0;
	IcmpHead.timestamp = 0;

	//必须icmp头构好后,再计算校验和,否则出错.
	IcmpHead.i_cksum = PROTOCOL_HEAD::CheckSum((USHORT*)&IcmpHead, sizeof(IcmpHead));

	fd_set fdRead = { 0 };

	//接收缓冲区
	char RecvBuf[MAXBYTE] = { 0 };
	PROTOCOL_HEAD::LPIPHEADER lpIPHead = NULL;
	PROTOCOL_HEAD::LPICMPHEADER lpICMPHead = NULL;
	sockaddr_in from = { 0 };

	lpIPHead = (PROTOCOL_HEAD::LPIPHEADER)RecvBuf;
	lpICMPHead = (PROTOCOL_HEAD::LPICMPHEADER)(lpIPHead + 1);


	//发送请求数据
	int nRet =
		hsk.SendTo((PCHAR)&IcmpHead, sizeof(IcmpHead), &DestAddr);

	//等待 返回数据,3秒超时
	FD_ZERO(&fdRead);
	fdRead.fd_count = 1;
	fdRead.fd_array[0] = hsk.GetSafeHandle();
	nRet = MySocket::gSelect(&fdRead, NULL, NULL, 3000);
	if (0 == nRet)
	{
		MyShowDbg("time out!!!\r\n");
		return FALSE;
	}

	memset(RecvBuf, 0, sizeof(RecvBuf));
	nRet = hsk.RecvFrom(RecvBuf, sizeof(RecvBuf), &from);

	//如果应答请求回显
	if (0 == lpICMPHead->i_code &&
		0 == lpICMPHead->i_type)
	{
		MyShowDbg("收到字节: %u \t目标IP: %s \tTTL: %d \n",
			nRet,
			MySocket::ginet_ntoa(lpIPHead->destIP),
			lpIPHead->ttl);

		return TRUE;
	}

	return FALSE;
}

//内部缓冲3048,不要超过此数 
void MyShowDbg(char* lpszFormat, ...)
{
	char bufFormat[3048];
	va_list arglist;
	int nlen = 0;
	va_start(arglist, lpszFormat);

	RtlZeroMemory(bufFormat, sizeof(bufFormat));

	nlen = vsprintf(bufFormat, lpszFormat, arglist);
	if ((nlen + 2) >= sizeof(bufFormat))
	{
		OutputDebugString("MyShowDbg 缓冲区溢出\r\n!");
		_asm int 3
	}
	strcpy(bufFormat + nlen, "\r\n");
	OutputDebugString(bufFormat);

}