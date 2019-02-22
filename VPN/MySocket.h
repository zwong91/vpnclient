// MySocket.h: interface for the MySocket class.
//ss1
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSOCKET_H__AAFE97E6_1447_49BF_B051_FA8C05640E35__INCLUDED_)
#define AFX_MYSOCKET_H__AAFE97E6_1447_49BF_B051_FA8C05640E35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Winsock2.h.>
#pragma comment(lib, "Ws2_32.lib")
#define  OUT
#define  IN

class MySocket  
{
public:
	MySocket();
	MySocket(MySocket&);
	MySocket(SOCKET);

	~MySocket();

	MySocket& operator=(SOCKET hsocket);
	MySocket& operator=(MySocket&);

	BOOL operator==(SOCKET hsocket);
	BOOL operator==(MySocket& );
	
/*	operator UINT()*/;
public:
	static BOOL gWSAStartup();
	static BOOL gWSACleanup();
	//显示winsocket最后错误原因
	static  VOID gWSAShowLasterr();
	//显示最后错误原因
	static  VOID gShowLasterr();
	
	//ushot 网络-->主机
	static u_short gntohs(u_short netshort);
	//ulong 网络-->主机
	static u_long gntohl( u_long netlong);

	//ushot 主机--> 网络
	static u_short ghtons(u_short hostshort);
	//ulong 主机--> 网络
	static u_long ghtonl( u_long hostlong );

	//字符串IP转成网络字节IP , 返回 0 失败！ 
	static unsigned long ginet_addr(const char* lpszIPaddr);

	//网络IP--＞字符串IP　返回NULL 失败
	static char* ginet_ntoa(in_addr IP);
	//网络IP--＞字符串IP　返回NULL 失败
	static char* ginet_ntoa(u_long uIP);

	//返回: 0超时退出,SOCKET_ERROR错误 ,否则返回发生网络事件的所有套接字数量的总和
	//功能:可等待64个socket信号事件(readfds可读?, writefds 可写? ,nMillisecond: 超时时间 毫秒单位 )
	/*
	FD_ZERO(fd_set   *fdset)：清空fdsetf,集合在使用前总是空的。       
	FD_SET(int   fd,   fd_set   *fdset): 添加套接字到集合       
	FD_CLR(int   fd,   fd_set   *fdset)：从集合中删除提定套接字      
	FD_ISSET(int   fd,   fdset   *fdset)：检查s是不是集合的成员,如果是返回TRUE   
	*/
	static int gSelect(fd_set FAR *readfds, fd_set FAR *writefds, fd_set FAR *exceptfds, int nMillisecond);

public:
	//纪录是否已经调用了WSAStartup 
	static BOOL m_gIsInitialSocket;
	
	SOCKET m_hsk;
	//引用计数
	PINT  m_lpnRefCount;

public:
	
	SOCKET GetSafeHandle();
	// 释放
	void Release();
	//创建一个套接字 参数:套接字类型,协义
	BOOL Socket(int type, int protocol);
	//绑下    参数:指向IP字符串，　端口;
	BOOL Bind(char* lpszIp,u_short nPort);
	//连接     (lpDestIp: 目标IP，　nDestPort：目标端口)
	BOOL MySocket::Connect(char* lpDestIp,u_short nDestPort);

	//监听　(backlog:等待队列最多请求,队列满了，后来请求，直接拒绝)
	BOOL Listen(int backlog);
	
	//INVALID_SOCKET失败:  接受连接(addr:地址相关信息)
	SOCKET MySocket::Accept(OUT sockaddr_in *lpAddr);

	//已复制到系统缓冲区字节数,失败SOCKET_ERROR: 发送 (buf: 要发送的用户缓冲区, len:buf大小, 何种模式)
	int Send(char *buf, int len, int flags);
	//接收到字节数,失败SOCKET_ERROR: 接收 (buf: 接收的用户缓冲区, len:buf大小, 何种模式)
	int Recv(char* buf, int len, int flags);

	//关闭套接字,清空引用计数缓冲区，socket 置invalid_socket
	BOOL CloseSocket();

	//udp -------------------------------------------------------------------------
	//在从系统接收缓冲区中一次性取一个完整UDP包,所以不会出现粘包现像,len过小会出错.
	//接收到字节数,失败SOCKET_ERROR: UDP接收 (buf: 接收的用户缓冲区, len:buf大小, from:客户端的地址信息)
	int RecvFrom(IN char* buf, IN int len, OUT struct sockaddr_in *from);
	
	//已发送的字节数,失败SOCKET_ERROR: UDP发送 (buf: 接收的用户缓冲区, len:Length of the data in buf. , from:客户端的地址信息)
	//已发送字节数可能会小于指定的长度
	int SendTo(IN char* buf, IN int len, IN struct sockaddr_in *to);

	//已发送的字节数,失败SOCKET_ERROR: UDP发送 (buf: 接收的用户缓冲区, len:Length of the data in buf. , lpDestIp: 目标IP nDestPort：目标端口)
	//已发送字节数可能会小于指定的长度
	int SendTo(IN char* buf, IN int len, IN char* lpDestIp, IN u_short nDestPort);

	//设置套接字类型函数

	//The WSAAsyncSelect function requests Windows message-based notification of network events for a socket.
	//SOCKET_ERROR失败 (梆定的窗口, 哪个消息通知, 什么网络事件触发通知)
	int WSAAsyncSelect(HWND hWnd, u_int wMsg, long lEvent = FD_ACCEPT|
												FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE);	

	//连接     (lpDestIp: 目标IP，　nDestPort：目标端口)
BOOL Connect(u_long uDestIp,u_short nDestPort);

};


#pragma pack (push)
#pragma pack (1)


struct PROTOCOL_HEAD
{

	// 定义IP首部 
	typedef struct ip_hdr 
	{ 
		unsigned char h_verlen;				// 4位首部长度,4位IP版本号 
		unsigned char tos;					// 8位服务类型TOS 
		unsigned short total_len;			// 16位总长度（字节） 
		unsigned short ident;				// 16位标识 
		unsigned short frag_and_flags;		// 3位标志位 
		unsigned char ttl;					// 8位生存时间 TTL 
		unsigned char proto;				// 8位协议 (TCP, UDP 或其他) 
		unsigned short checksum;			// 16位IP首部校验和 
		unsigned int sourceIP;				// 32位源IP地址 
		unsigned int destIP;				// 32位目的IP地址 
	}IPHEADER,*LPIPHEADER; 
	

	// 定义ICMP首部
	typedef struct icmp_hdr
	{
		unsigned char  i_type;           // 类型
		unsigned char  i_code;           // 代码
		unsigned short i_cksum;          // 校验码
		//---------以下用户使用字段-------------------------
		unsigned short i_id;             // 非标准的ICMP首部  
		unsigned short i_seq;
		unsigned long  timestamp;
	}ICMPHEADER,*LPICMPHEADER;
	
	// 定义UDP首部
	// The UDP packet is lick this. Took from RFC768.
	//                  0      7 8     15 16    23 24    31  
	//                 +--------+--------+--------+--------+ 
	//                 |     Source      |   Destination   | 
	//                 |      Port       |      Port       | 
	//                 +--------+--------+--------+--------+ 
	//                 |                 |                 | 
	//                 |     Length      |    Checksum     | 
	//                 +--------+--------+--------+--------+ 
	//                 |                                     
	//                 |          data octets ...            
	//                 +---------------- ...       
	
	typedef struct udp_hdr  // 8 Bytes
	{
		unsigned short uh_sport;         
		unsigned short uh_dport;
		unsigned short uh_len;
		unsigned short uh_sum;
	} UDPHEADER,*LPUDPHEADER;

	/*UDP/TCP报头中的校验和的计算比较复杂的，要用到 UDP/TCP伪首部：先要填充伪首部各个字段，
	然后再将UDP/TCP报头以后（包括报头）的数据附加到伪首部的后面，
	(如PSDHEADER + UDPHEADER) 
	再对伪首部使用上述校验和计算，所得到的值才是UDP/TCP报头部分的校验和,伪首部可以用如下的结构体表示：*/

	// 定义TCP/UDP伪首部 
	typedef struct tsd_hdr{

		ULONG sourceip;    //源IP地址
		ULONG destip;      //目的IP地址
		BYTE mbz;           //置空(0)
		BYTE ptcl;          // 协议类型 UDP的协议类型为17，TCP为6 
		USHORT plen;        //TCP/UDP数据包的长度(即从TCP/UDP报头算起到数据包结束的长度 单位:字节)

	}PSDHEADER,*LPPSDHEADER; 



	/*
	一、ipv4、tcp、udp的校验和  
		IP/ICMP/IGMP/TCP/UDP等协议的校验和算法都是相同的，采用的都是将数据流视为16位整数流进行重复叠加计算。
		为了计算检验和，首先把检验和字段置为0。然后，对有效数据范围内中每个16位进行二进制反码求和，结果存在检验和字段中，
		
		如果数据长度为奇数则补一字节0。当收到数据后，同样对有效数据范围中每个16位数进行二进制反码的求和。
		由于接收方在计算过程中包含了发送方存在首部中的检验和，因此，如果首部在传输过程中没有发生任何差错，
		那么接收方计算的结果应该为全0或全1(具体看实现了,本质一样) 。如果结果不是全0或全1，那么表示数据错误。

		以上算法原理是浮云,以下重要!!!
		对于IPv4层中的校验和只包括IPv4头部分，不包括上层协议头和应用层数据，校验和是必须计算的。
		对于ICMP/IGMP校验和计算范围为从ICMP/IGMP开始到数据结束，不包括IP头部分，校验和是必须计算的。
		对于TCP及UDP层的检验和计算有点特殊，所计算的数据范围除了包括TCP/UDP头开始到数据结束外，还要包括一个IP伪头部分，
		所谓伪头，只有12字节数据，包括源地址(4字节)、目的地址(4字节)、协议(2字节，第一字节补0)和TCP/UDP包长(2字节)。
		TCP的校验和是必须的，而UDP的校验和是可选的，如果UDP中校验和字段为0，表示不进行校验和计算，
		因此对于UDP协议数据的修改后想偷懒的话直接将校验和设为0 就可以了。
		参考:http://hi.baidu.com/ffsdaihuiping/blog/item/7a65a6c35dc84345b219a839.html
	*/
	static USHORT CheckSum(USHORT *buffer, int size);	

};



typedef struct _DATAPACK
{
	enum{
		//上次操作被服务端拒绝 客户端操作完成		服务端操作完成
		SC_OPERATE_REJECT_,	 CS_OPERATE_FINISH_, SC_OPERATE_FINISH_,

		//登录包
		CS_LOGIN_, SC_LOGIN_WELCOME_, CS_LOGOUT_,

		//获取用户信息		//发送用户信息
		CS_USER_INFO_GET_, SC_USER_INFO_POST_,

		//下载文件
		CS_FILE_GET_, SC_FILE_DATA_POST_, CS_FILE_DATA_GET_,

		//包的最大长度					包的数据域最大长度 = 包的最大长度 - 头部长度
		PCAK_TOTAL_MAX_SIZE_ = 0X2000, PACK_DATA_SIZE_ = PCAK_TOTAL_MAX_SIZE_ - 5*4};

	//---------------包的格式-------------
		//包的头部						
	struct _HEAD{								
		UINT PackLen;		//本包的总长度,当校验和吧!哈哈
		UINT PackType;		//本包的类型	
		UINT CurPack;		//包序: 当前是第几个包
		UINT TotalPacks;    //包序: 总共有多少包
		UINT DataLen;		//数据长度
	}HEAD;
		//包的数据域
	union _DATA
	{
		//占地方
		char DataBuffer[PACK_DATA_SIZE_];

		//操作状态
		struct _SC_OPERATE_REJECT{
			char RejectInfo[MAXBYTE];			//服务器拒绝客户端返回的信息,可以打印看看
		};
		struct _CS_OPERATE_FINISH{};			//客户端告诉服务器一个操作完成 [cs]
		struct _SC_OPERATE_FINISH{};			//服务器告诉客户端一个操作完成 [sc 以下雷同]
		
		//登录包
		struct _CS_LONGIN{				
			char UserName[MAXBYTE]; 
		};
		struct _SC_LOGIN_WELCOME{};
		struct _CS_LOGOUT{};

		//用户信息包
		struct _CS_USER_INFO_ONLINE_GET{
			enum{USERINFO_ADDR_			   = 0x0001,		//获取指定用户ip地址信息
				 USERINFO_ONLINE_USER_NAME_= 0x0002,		//获取在线所有用户
				 USERINFO_SHARE_FILE_	   = 0x0004,		//获取指定用户的共享文件

				 USERINFO_ALL_ = USERINFO_ADDR_ |			//获取所有信息
								USERINFO_ONLINE_USER_NAME_ |
								USERINFO_SHARE_FILE_};					
			DWORD Flag;										//获取标志	
			char UserName[MAXBYTE];							//仅当USERINFO_ADDR_ 或 USERINFO_SHARE_FILE_ 有效
		};	
											
		struct _SC_USER_INFO_ONLINE_POST{	
			union{

				struct _USER_NAMES{							//仅当请求时指定 USERINFO_NAME_ 有效
					UINT nUserNums;							//本包 含用户个数		注意:nUserNums* MAXBYTE <= PACK_DATA_SIZE_
					char Name[1][MAXBYTE];					//用户名		
				};
				sockaddr_in Addr;							//仅当请求时指定 USERINFO_ADDR_ 有效

				struct _SHARE_FILES{						//仅当请求时指定 USERINFO_SHARE_FILE_ 有效
					UINT nShareFileNums;					//本包 含用户共享文件个数  注意:nShareFileNums* MAXBYTE <= PACK_DATA_SIZE_
					char FileName[1][MAXBYTE];				//每个文件名固定长度:MAXBYTE
				};
			};			
		};

		//文件包
		struct _CS_FILE_GET_{					//想要从哪个用户获取什么文件 
			char UserName[MAXBYTE];				// 用户	"SERVER"表示从服务器上获取
			char FileName[MAXBYTE];				// 文件名
		};
		struct _CS_FILE_DATA_GET{
			UINT GetNextPack;					//客户端想要获取的下一个包序数据,此前包序表示已经确认收到!
		};
		struct _SC_FILE_DATA_POST{				//服务器向客户端发 文件数据 包
			char FileData[PACK_DATA_SIZE_];		//文件数据内容
		};
	}DATA;
	//------------------------------------




}DATAPACK,*LPDATAPACK;
#pragma pack (pop)























#endif // !defined(AFX_MYSOCKET_H__AAFE97E6_1447_49BF_B051_FA8C05640E35__INCLUDED_)

/*
网络字节顺序与主机字节顺序的转化问题  

	ntohs =net to host short int 16位

	htons=host to net short int 16位

	ntohl =net to host long int 32位

	htonl=host to net   long int   32位
	
	//字符串IP转成网络字节IP
	 sockaddr_in.sinv_addr.s_addr =   unsigned long 
									 inet_addr("132.241.5.10");

	//将网络字节顺 转面 字符串IP
	printf("%s",inet_ntoa(ina.sin_addr));

*/


// 	typedef void (_DATAPACK::* LPDOFUN)();
// 	LPDOFUN lpFuns[4];
// 	
// 	_DATAPACK()
// 	{
// 		lpFuns[CS_CMD_] = DoCmd;
// 	}
// 	
// 	void Do()
// 	{
// 		(this->*lpFuns[HEAD.PackType])();
// 	}
// 	
// 	void DoCmd()
// 	{
// 		AfxMessageBox("我是Cmd包");
// 	}

//发包  sock对像			 数据长度        包类型         要发送的数据               目标地址信息
// 	int SendTo(IN MySocket& obj, IN UINT nDataLen, IN DWORD dPackType, IN CHAR* lpDataBuf, sockaddr_in* lpTo)
// 	{	
// 		if (nDataLen > DATABUFF_MAX)
// 		{
// 			AfxMessageBox("发的内容过多");
// 			return 0;
// 		}
// 		m_nDataLen = nDataLen;
// 		m_PackType = dPackType;
// 		memcpy(m_DataBuf, lpDataBuf, m_nDataLen);
// 		
// 		m_PackLen = 4 + 4 + 4 + m_nDataLen; //包长度 数据长度  类型 数据区    
// 		
// 		int nRet = obj.SendTo((char *)&m_PackLen, m_PackLen,lpTo);
// 		if (0 == nRet || SOCKET_ERROR == nRet)
// 			return nRet;
// 
// 		return m_PackLen;
// 	}
