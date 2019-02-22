// MySocket.cpp: implementation of the MySocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MySocket.h"
#include <afxtempl.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL MySocket::m_gIsInitialSocket = FALSE;

MySocket::MySocket()
{
	m_hsk = INVALID_SOCKET;	
	m_lpnRefCount = NULL;
}

MySocket::MySocket(SOCKET hsocket)
{
		m_hsk = hsocket;
		m_lpnRefCount = new INT(1);
}

MySocket::MySocket(MySocket& obj)
{
	m_hsk = obj.m_hsk;
	m_lpnRefCount = obj.m_lpnRefCount;

	//增加引用计数
	int nCOunt = *m_lpnRefCount;
	*m_lpnRefCount = ++nCOunt;
}

MySocket::~MySocket()
{
	Release();
}

BOOL MySocket::operator==(SOCKET hsocket)
{
		if (m_hsk == hsocket)
			return TRUE;
		else
			return FALSE;
}

BOOL MySocket::operator==(MySocket& obj)
{
		if (m_hsk == obj.m_hsk)
			return TRUE;
		else
			return FALSE;
}



MySocket& MySocket::operator=(MySocket& obj)
{
	Release();

	m_hsk = obj.m_hsk;
	m_lpnRefCount = obj.m_lpnRefCount;

	int nCount = *m_lpnRefCount;
	*m_lpnRefCount = ++nCount;

	return *this;
}

BOOL MySocket::gWSAStartup()
{
	//已经初始化直接返回
	if (m_gIsInitialSocket)
		return TRUE;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	BOOL nRet = FALSE;
	wVersionRequested = MAKEWORD( 2, 2 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		AfxMessageBox("Winsocket 初始化 失败,无法加载WinSock.dll!!!");

		//初始失败!
		m_gIsInitialSocket = FALSE;
		return FALSE;
	}
	
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 ) {
		AfxMessageBox("Winsocket 初始化 失败,winsocket 版本不对!!!");           
		WSACleanup();
		//初始失败!
		m_gIsInitialSocket = FALSE;
		return FALSE;
	}
	
	/* The WinSock DLL is acceptable. Proceed. */
	//初始成功!
	m_gIsInitialSocket = TRUE;
	return TRUE;
}

BOOL MySocket::gWSACleanup()
{
	m_gIsInitialSocket = FALSE;
	if(WSACleanup() == SOCKET_ERROR)
	{
		gWSAShowLasterr();
		return FALSE;
	}	
	return TRUE;
}

VOID MySocket::gWSAShowLasterr()
{
	DWORD ErrID = WSAGetLastError();
	if (m_gIsInitialSocket)
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			ErrID,
			0, // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
			);

		//弹的频太高,先关了这几项报错!
// 		if (ErrID != WSAEINTR &&
// 			  ErrID != WSAECONNRESET )
// 			MessageBox( NULL, (LPCTSTR)lpMsgBuf, "结果:", MB_OK | MB_ICONINFORMATION );

		// Free the buffer.
		LocalFree( lpMsgBuf );
		return;
	}
	else
	{
		AfxMessageBox("winsokcet 没有初始化, 不能拿到错误码!");
		return;
	}

	/*
				 * All Windows Sockets error constants are biased by WSABASEERR from

			#define WSABASEERR              10000

			  * Windows Sockets definitions of regular Microsoft C error constants	
				#define WSAEINTR                (WSABASEERR+4)
				#define WSAEBADF                (WSABASEERR+9)
				#define WSAEACCES               (WSABASEERR+13)
				#define WSAEFAULT               (WSABASEERR+14)
				#define WSAEINVAL               (WSABASEERR+22)
				#define WSAEMFILE               (WSABASEERR+24)

	
				 * Windows Sockets definitions of regular Berkeley error constants
	
				#define WSAEWOULDBLOCK          (WSABASEERR+35)
				#define WSAEINPROGRESS          (WSABASEERR+36)
				#define WSAEALREADY             (WSABASEERR+37)
				#define WSAENOTSOCK             (WSABASEERR+38)
				#define WSAEDESTADDRREQ         (WSABASEERR+39)
				#define WSAEMSGSIZE             (WSABASEERR+40)
				#define WSAEPROTOTYPE           (WSABASEERR+41)
				#define WSAENOPROTOOPT          (WSABASEERR+42)
				#define WSAEPROTONOSUPPORT      (WSABASEERR+43)
				#define WSAESOCKTNOSUPPORT      (WSABASEERR+44)
				#define WSAEOPNOTSUPP           (WSABASEERR+45)
				#define WSAEPFNOSUPPORT         (WSABASEERR+46)
				#define WSAEAFNOSUPPORT         (WSABASEERR+47)
				#define WSAEADDRINUSE           (WSABASEERR+48)
				#define WSAEADDRNOTAVAIL        (WSABASEERR+49)
				#define WSAENETDOWN             (WSABASEERR+50)
				#define WSAENETUNREACH          (WSABASEERR+51)
				#define WSAENETRESET            (WSABASEERR+52)
				#define WSAECONNABORTED         (WSABASEERR+53)
				#define WSAECONNRESET           (WSABASEERR+54)
				#define WSAENOBUFS              (WSABASEERR+55)
				#define WSAEISCONN              (WSABASEERR+56)
				#define WSAENOTCONN             (WSABASEERR+57)
				#define WSAESHUTDOWN            (WSABASEERR+58)
				#define WSAETOOMANYREFS         (WSABASEERR+59)
				#define WSAETIMEDOUT            (WSABASEERR+60)
				#define WSAECONNREFUSED         (WSABASEERR+61)
				#define WSAELOOP                (WSABASEERR+62)
				#define WSAENAMETOOLONG         (WSABASEERR+63)
				#define WSAEHOSTDOWN            (WSABASEERR+64)
				#define WSAEHOSTUNREACH         (WSABASEERR+65)
				#define WSAENOTEMPTY            (WSABASEERR+66)
				#define WSAEPROCLIM             (WSABASEERR+67)
				#define WSAEUSERS               (WSABASEERR+68)
				#define WSAEDQUOT               (WSABASEERR+69)
				#define WSAESTALE               (WSABASEERR+70)
				#define WSAEREMOTE              (WSABASEERR+71)

				#define WSAEDISCON              (WSABASEERR+101)


				 * Extended Windows Sockets error constant definitions

				#define WSASYSNOTREADY          (WSABASEERR+91)
				#define WSAVERNOTSUPPORTED      (WSABASEERR+92)
				#define WSANOTINITIALISED       (WSABASEERR+93)

	
				 * Error return codes from gethostbyname() and gethostbyaddr()
				 * (when using the resolver). Note that these errors are
				 * retrieved via WSAGetLastError() and must therefore follow
				 * the rules for avoiding clashes with error numbers from
				 * specific implementations or language run-time systems.
				 * For this reason the codes are based at WSABASEERR+1001.
				 * Note also that [WSA]NO_ADDRESS is defined only for
				 * compatibility purposes.


				#define h_errno         WSAGetLastError()

				 Authoritative Answer: Host not found 
				#define WSAHOST_NOT_FOUND       (WSABASEERR+1001)
				#define HOST_NOT_FOUND          WSAHOST_NOT_FOUND

				 Non-Authoritative: Host not found, or SERVERFAIL 
				#define WSATRY_AGAIN            (WSABASEERR+1002)
				#define TRY_AGAIN               WSATRY_AGAIN

				/* Non recoverable errors, FORMERR, REFUSED, NOTIMP 
				#define WSANO_RECOVERY          (WSABASEERR+1003)
				#define NO_RECOVERY             WSANO_RECOVERY

				/* Valid name, no data record of requested type 
				#define WSANO_DATA              (WSABASEERR+1004)
				#define NO_DATA                 WSANO_DATA

				/* no address, look for MX record 
				#define WSANO_ADDRESS           WSANO_DATA
				#define NO_ADDRESS              WSANO_ADDRESS

				/*
				 * Windows Sockets errors redefined as regular Berkeley error constants.
				 * These are commented out in Windows NT to avoid conflicts with errno.h.
				 * Use the WSA constants instead.
				 
				#if 0
				#define EWOULDBLOCK             WSAEWOULDBLOCK
				#define EINPROGRESS             WSAEINPROGRESS
				#define EALREADY                WSAEALREADY
				#define ENOTSOCK                WSAENOTSOCK
				#define EDESTADDRREQ            WSAEDESTADDRREQ
				#define EMSGSIZE                WSAEMSGSIZE
				#define EPROTOTYPE              WSAEPROTOTYPE
				#define ENOPROTOOPT             WSAENOPROTOOPT
				#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
				#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
				#define EOPNOTSUPP              WSAEOPNOTSUPP
				#define EPFNOSUPPORT            WSAEPFNOSUPPORT
				#define EAFNOSUPPORT            WSAEAFNOSUPPORT
				#define EADDRINUSE              WSAEADDRINUSE
				#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
				#define ENETDOWN                WSAENETDOWN
				#define ENETUNREACH             WSAENETUNREACH
				#define ENETRESET               WSAENETRESET
				#define ECONNABORTED            WSAECONNABORTED
				#define ECONNRESET              WSAECONNRESET
				#define ENOBUFS                 WSAENOBUFS
				#define EISCONN                 WSAEISCONN
				#define ENOTCONN                WSAENOTCONN
				#define ESHUTDOWN               WSAESHUTDOWN
				#define ETOOMANYREFS            WSAETOOMANYREFS
				#define ETIMEDOUT               WSAETIMEDOUT
				#define ECONNREFUSED            WSAECONNREFUSED
				#define ELOOP                   WSAELOOP
				#define ENAMETOOLONG            WSAENAMETOOLONG
				#define EHOSTDOWN               WSAEHOSTDOWN
				#define EHOSTUNREACH            WSAEHOSTUNREACH
				#define ENOTEMPTY               WSAENOTEMPTY
				#define EPROCLIM                WSAEPROCLIM
				#define EUSERS                  WSAEUSERS
				#define EDQUOT                  WSAEDQUOT
				#define ESTALE                  WSAESTALE
				#define EREMOTE                 WSAEREMOTE
				#endif
					
	
	*/

}

VOID MySocket::gShowLasterr()
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		WSAGetLastError(),
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, "结果:", MB_OK | MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );
	return;
}

//创建一个套接字 参数:套接字类型,协义
BOOL MySocket::Socket(int type, int protocol)
{
	//一个对像只管理一个套接字,如果套接存在,直接返回
	if (INVALID_SOCKET != m_hsk)
	{
	//	AfxMessageBox("MySocket 对像已经绑定了一个套接字!　直接返回，C++对像内socket保持不变!");
		return TRUE; 
	}

	m_hsk = ::socket(AF_INET, type, protocol);
	if (INVALID_SOCKET == m_hsk)
	{
		MySocket::gWSAShowLasterr();
		return FALSE;
	}
	
	if (NULL == m_lpnRefCount)
		m_lpnRefCount = new int(1);
	else
	{
		AfxMessageBox("MySocket::Socket 引用计数缓冲区存在，与设计不符，请改正!");
	}
	return TRUE;
}


MySocket& MySocket::operator=(SOCKET hsocket)
{
	Release();
	m_hsk = hsocket;
	m_lpnRefCount = new INT(1);
	return *this;
}

BOOL MySocket::Bind(char* lpszIp,u_short nPort)
{
	sockaddr_in ski = {0};
	ski.sin_family = AF_INET;
	ski.sin_port   = ::htons(nPort);

	ski.sin_addr.s_addr = ::inet_addr(lpszIp);

	int nRet = ::bind(m_hsk, (sockaddr*)&ski, sizeof(ski));

	if (SOCKET_ERROR == nRet)
	{
		MySocket::gWSAShowLasterr();
		return FALSE;
	}

	return TRUE;
}



u_short MySocket::gntohs(u_short netshort)
{
	return ::ntohs(netshort);
}

u_short MySocket::ghtons(u_short hostshort)
{
	return ::htons(hostshort);
}

u_long MySocket::gntohl( u_long netlong)
{
	return ::ntohl(netlong);
}

u_long MySocket::ghtonl( u_long hostlong )
{
	return ::htonl(hostlong);
}

//字符串IP转成网络字节IP , 返回 0 失败！ 
unsigned long MySocket::ginet_addr(const char* lpszIPaddr)
{
	unsigned long uRet = 0;

	uRet = ::inet_addr(lpszIPaddr);
	if (0 == uRet || INADDR_NONE == uRet)
	{
		AfxMessageBox("inet_addr：IP地址转换失败!");
		return 0;
	}

	return uRet;
}


char* MySocket::ginet_ntoa(in_addr IP)
{
	char* lpszIP = NULL;
	lpszIP = ::inet_ntoa(IP);
	if (NULL == lpszIP)
	{
		AfxMessageBox("ginet_ntoa：IP地址转换失败!");
	}

	return lpszIP;
}

char* MySocket::ginet_ntoa(u_long uIP)
{
	char* lpszIP = NULL;
	in_addr IP;
	IP.s_addr = uIP;
	lpszIP = ::inet_ntoa(IP);

	if (NULL == lpszIP)
	{
		AfxMessageBox("ginet_ntoa：IP地址转换失败!");
	}
	
	return lpszIP;
}

//　　　监听　(backlog:等待队列最多请求,队列满了，后来请求，直接拒绝)
BOOL MySocket::Listen(int backlog)
{
	if (SOCKET_ERROR == ::listen(m_hsk, backlog))
	{
		MySocket::gWSAShowLasterr();
		return FALSE;
	}

	return TRUE;
}

//INVALID_SOCKET失败:  接受连接(addr:地址相关信息)
SOCKET MySocket::Accept(OUT sockaddr_in *lpAddr)
{
		SOCKET hNewsk = INVALID_SOCKET;		
		
		int  AddrSize = sizeof(sockaddr);
		hNewsk = ::accept(m_hsk, (sockaddr*)lpAddr, &AddrSize);
		
		if (INVALID_SOCKET == hNewsk)
			gWSAShowLasterr();

		return hNewsk;
}

//已复制到系统缓冲区字节数,失败SOCKET_ERROR: 发送 (buf: 要发送的用户缓冲区, len:buf大小, 何种模式)
int MySocket::Send(char *buf, int len, int flags)
{
		int nRet = 0;
		
		nRet = ::send(m_hsk, buf, len, flags);
		
		if (SOCKET_ERROR  == nRet)
		{
			gWSAShowLasterr();
		}
		return nRet;
}

//接收到字节数,失败SOCKET_ERROR: 发送 (buf: 接收的用户缓冲区, len:buf大小, 何种模式)
int MySocket::Recv(char* buf, int len, int flags)
{
		int nRet = 0;
		nRet = ::recv(m_hsk, buf, len, flags);
		if (SOCKET_ERROR == nRet || nRet == 0)
		{
			gWSAShowLasterr();
		}
		
		return nRet;
}


//关闭套接字,清空引用计数缓冲区，socket 置invalid_socket
BOOL MySocket::CloseSocket()
{
		if (INVALID_SOCKET == m_hsk)
			return TRUE;

		int nRet = ::shutdown(m_hsk, SD_BOTH);
// 		if (SOCKET_ERROR == nRet)
// 		{
// 			gWSAShowLasterr();
// 		}
		
		nRet = ::closesocket(m_hsk);
// 		if (SOCKET_ERROR == nRet)
// 		{
// 			gWSAShowLasterr();
// 			//return FALSE;
// 		}
		
		m_hsk = INVALID_SOCKET;
		if (m_lpnRefCount != NULL)
			delete m_lpnRefCount;
		m_lpnRefCount = NULL;

		return TRUE;
}

//连接     (lpDestIp: 目标IP，　nDestPort：目标端口)
BOOL MySocket::Connect(char* lpDestIp,u_short nDestPort)
{
	sockaddr_in ski = {0};
	ski.sin_family = AF_INET;
	ski.sin_port   = ::htons(nDestPort);
	ski.sin_addr.s_addr = ::inet_addr(lpDestIp);
		
	if ( SOCKET_ERROR == ::connect(m_hsk, (sockaddr*)&ski, sizeof(sockaddr)) )
	{
		gWSAShowLasterr();
		return FALSE;
	}
	
	return TRUE;
}

//连接     (lpDestIp: 目标IP，　nDestPort：目标端口)
BOOL MySocket::Connect(u_long uDestIp,u_short nDestPort)
{
	sockaddr_in ski = {0};
	ski.sin_family = AF_INET;
	ski.sin_port   = ::htons(nDestPort);
	ski.sin_addr.s_addr = uDestIp;
	
	if ( SOCKET_ERROR == ::connect(m_hsk, (sockaddr*)&ski, sizeof(sockaddr)) )
	{
		gWSAShowLasterr();
		return FALSE;
	}
	
	return TRUE;
}


void MySocket::Release()
{
	if (m_hsk == INVALID_SOCKET)
	{
		return ;
	}
	
	//减少引用计数
	if (m_lpnRefCount)
	{
		int nRefCount = (*m_lpnRefCount) - 1;
		*m_lpnRefCount = nRefCount;

		if (nRefCount <= 0)
			this->CloseSocket();
	}
	else
	{
		m_hsk = INVALID_SOCKET;
		return;
	}

}

SOCKET MySocket::GetSafeHandle()
{
	return m_hsk;
}


//在从系统接收缓冲区中一次性取一个完整UDP包,所以不会出现粘包现像,len过小会出错.
//接收到字节数,失败SOCKET_ERROR: UDP接收 (buf: 接收的用户缓冲区, len:buf大小, from:客户端的地址信息)
int MySocket::RecvFrom(IN char* buf, IN int len, OUT struct sockaddr_in *from)
{
	int nAddrSize = sizeof(sockaddr);
	int nRet = 
		::recvfrom(m_hsk, buf, len, 0, (sockaddr*)from, &nAddrSize);
	
	if (0 == nRet || SOCKET_ERROR == nRet)
	{
		gWSAShowLasterr();
	}
	
	return nRet;
}

//已发送的字节数,失败SOCKET_ERROR: UDP发送 (buf: 接收的用户缓冲区, len:Length of the data in buf. , from:客户端的地址信息)
//已发送字节数可能会小于指定的长度
int MySocket::SendTo(IN char* buf, IN int len, IN sockaddr_in *to)
{
	int nRet = 
		::sendto(m_hsk, buf, len, 0, (sockaddr*)to, sizeof(sockaddr));
	
	if (SOCKET_ERROR == nRet)
	{
		gWSAShowLasterr();
	}
	
	return nRet;
}

//已发送的字节数,失败SOCKET_ERROR: UDP发送 (buf: 接收的用户缓冲区, len:Length of the data in buf. , lpDestIp: 目标IP nDestPort：目标端口)
//已发送字节数可能会小于指定的长度
int MySocket::SendTo(IN char* buf, IN int len, IN char* lpDestIp, IN u_short nDestPort)
{
	
	sockaddr_in to = {0};
	to.sin_family = AF_INET;
	to.sin_port = ::htons(nDestPort);
	to.sin_addr.s_addr = ::inet_addr(lpDestIp);

	int nRet = 
		::sendto(m_hsk, buf, len, 0, (sockaddr*)&to, sizeof(sockaddr));
	
	if (SOCKET_ERROR == nRet)
	{
		gWSAShowLasterr();
	}
	
	return nRet;
}
//设置套接字类型函数

//The WSAAsyncSelect function requests Windows message-based notification of network events for a socket.
//SOCKET_ERROR失败 (梆定的窗口, 哪个消息通知, 什么网络事件触发通知)
int MySocket::WSAAsyncSelect(HWND hWnd, u_int wMsg, long lEvent)
{
	int nRet = 0;
	
	nRet = ::WSAAsyncSelect(m_hsk, hWnd, wMsg, lEvent);
	if (SOCKET_ERROR == nRet)
	{
		gWSAShowLasterr();
	}
	
	return nRet;
}
//返回: 0超时退出,SOCKET_ERROR错误 ,否则返回发生网络事件的所有套接字数量的总和
//功能:可等待64个socket信号事件(readfds可读?, writefds 可写? ,nMillisecond: 超时时间 毫秒单位 )
/*
FD_ZERO(fd_set   *fdset)：清空fdsetf,集合在使用前总是空的。       
FD_SET(int   fd,   fd_set   *fdset): 添加套接字到集合       
FD_CLR(int   fd,   fd_set   *fdset)：从集合中删除提定套接字      
FD_ISSET(int   fd,   fdset   *fdset)：检查s是不是集合的成员,如果是返回TRUE   
*/
int MySocket::gSelect(fd_set FAR *readfds, fd_set FAR *writefds, fd_set FAR *exceptfds, int nMillisecond)
{
	timeval timeout;
	timeout.tv_sec = nMillisecond / 1000;
	timeout.tv_usec = (nMillisecond % 1000)*1000 ;

	int nRet = 0;
	int nfds = 0;	//忽略,仅是为了与Berkeley套接字兼容
						
	nRet = ::select(nfds, readfds, writefds, exceptfds, &timeout);
	if (SOCKET_ERROR == nRet )
	{
	 gWSAShowLasterr();
	 return nRet;
	}

	return nRet;
}

//CheckSum:计算校验和的子函数 
USHORT PROTOCOL_HEAD::CheckSum(USHORT *buffer, int size) 
{ 
	unsigned long cksum=0; 
	while(size >1) 
	{ 
		cksum+=*buffer++; 
		size -=sizeof(USHORT); 
	} 
	if(size ) 
	{ 
		cksum += *(UCHAR*)buffer; 
	} 
	
	cksum = (cksum >> 16) + (cksum & 0xffff); 
	cksum += (cksum >>16); 
	return (USHORT)(~cksum); 
}