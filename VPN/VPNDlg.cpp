
// vpnDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "vpn.h"
#include "vpnDlg.h"
#include "RasDial.h"
#include "afxdialogex.h"
#include "MySocket.h"
#include "sqlite3.h"
#include "CppSQLite3.h"
#include "IpFinder.h"
#pragma comment(lib,"rasapi32.lib")
#pragma comment(lib,"sqlite3.lib")
#include <fstream>
#include <iostream>
using namespace std;

#define WM_ICON_NOTIFY WM_USER+10

#if((_WIN32_WINNT >= 0x0400) || (WINVER >= 0x0500))
#define WM_MOUSEHOVER                   0x02A1
#define WM_MOUSELEAVE                   0x02A3
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define  MAX_CONFIG_VPN_NUM 200
HANDLE	g_hDisconnect[100]; // 100个事件对象
HANDLE g_hconnectThread[MAX_CONFIG_VPN_NUM];

#pragma comment(lib, "Iphlpapi.lib")
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//默认网关
MIB_IPFORWARDROW gDefaultGetwayItem = { 0 };
//vpn网关
MIB_IPFORWARDROW gVPNGetwayItem = { 0 };
//例外IP
extern CDialog* lpMainDlg;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CvpnDlg 对话框

CvpnDlg::CvpnDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CvpnDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CvpnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//	DDX_Control(pDX, IDCLOSE, m_ctrlpic);
	//  DDX_Control(pDX, IDC_CHECK1, m_remember);
	//  DDX_Control(pDX, IDC_BUTTON1, m_btnLogin);
	DDX_Control(pDX, IDC_LIST2, m_list);
	DDX_Text(pDX, IDC_EDIT1, m_ip);
	DDX_Text(pDX, IDC_EDIT2, m_username);
	DDX_Text(pDX, IDC_EDIT3, m_password);
	//  DDX_Check(pDX, IDC_CHECK1, m_remember);
	DDX_Control(pDX, IDC_CHECK1, m_remember);

}

BEGIN_MESSAGE_MAP(CvpnDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CvpnDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CvpnDlg::OnBnClickedButton2)
	ON_MESSAGE(WM_ICON_NOTIFY, &CvpnDlg::OnTrayNotification)
	ON_CBN_SELCHANGE(IDC_COMBO, &CvpnDlg::OnCbnSelchangeCombo)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, &CvpnDlg::OnNMClickList2)
END_MESSAGE_MAP()


// CvpnDlg 消息处理程序

void ctow(wchar_t* wchar, const char *str)
{
	int wcslen = ::MultiByteToWideChar(CP_ACP, NULL, str, strlen(str), NULL, 0);
	::MultiByteToWideChar(CP_ACP, NULL, str, strlen(str), wchar, wcslen);
	wchar[wcslen] = 0;
}

CListCtrl *staticlist = NULL;
CMap<int, int, VpnInfo*, VpnInfo*> infos;

BOOL CvpnDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	//ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	//ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			//pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//加载界面皮肤
	SkinH_Attach();


	//获取窗口风格;
	LONG lStyle = GetWindowLong(this->m_list.m_hWnd, GWL_STYLE);
	//设置窗口风格;
	SetWindowLong(this->m_list.m_hWnd, GWL_STYLE, lStyle | LVS_REPORT);
	//网格线;整行选中;
	this->m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	this->m_list.InsertColumn(0, _T("编号"), 0, 50, -1);
	this->m_list.InsertColumn(1, _T("IP地址"), 0, 130, -1);
	this->m_list.InsertColumn(2, _T("地理位置"), 0, 130, -1);
	this->m_list.InsertColumn(3, _T("延迟"), 0, 50, -1);
	this->m_list.InsertColumn(4, _T("备注"), 0, 150, -1);

	staticlist = &this->m_list;

	m_ip = _T("");
	m_username = _T("");
	m_password = _T("");

	//填充账户
	InitData();

	::GetCurrentDirectory(MAX_PATH - 1, m_szConfigFile);
	_tcscat(m_szConfigFile, TEXT("\\Parameter.ini"));

	MySocket::gWSAStartup();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CvpnDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	/*if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
	}*/
	
	if (nID == SC_MINIMIZE)
	{
		m_TrayIcon.Create(this,WM_ICON_NOTIFY,"VPNClient仍在运行,  双击此图标显示主界面 ...", m_hIcon, IDR_MAINFRAME, TRUE); //构造
		m_TrayIcon.SetTooltipText("Running ....");
		ShowWindow(SW_HIDE);
	}
	else if (nID==SC_CLOSE)
	{
		if (MessageBox("确定退出吗？","退出",MB_YESNO)==IDYES)
		{
			if (NULL != g_TerminalEvent)
			{
				AfxMessageBox("请断开活动的VPN后，再关闭程序!");
				return;
			}

			g_bIsAppExit = TRUE;

			int iItemCount = this->m_list.GetItemCount();
			VpnInfo *pInfo = NULL;
			for (int i = 0; i < iItemCount; i++)
			{			
				pInfo = (VpnInfo *)(this->m_list.GetItemData(i));
				if (pInfo != NULL)
				{
					delete pInfo;
				}
			}

			for (int i = 3; i > 0; i--)
			{
				char szText[200];
				wsprintf(szText, "正在清理资源...  %d 秒退出程序.", i);
				SetWindowText(szText);
				Sleep(1000);
			}

			//结束程序
			if (NULL != g_hDisconnect[0])
			{
				SetEvent(g_hDisconnect[0]);
				CloseHandle(g_hDisconnect[0]);
			}

			MySocket::gWSACleanup();
			CDialog::OnClose();
			//exit(-1);
		}
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CvpnDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CvpnDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CvpnDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(lParam) ==WM_LBUTTONDOWN)
	{
		ShowWindow(SW_SHOW);
		return TRUE;
	}
	else if (LOWORD(lParam)==WM_MOUSEHOVER)
	{
		MessageBox("aaa");
		m_TrayIcon.SetTooltipText(GetConnStatus());
		return TRUE;
	}
	return m_TrayIcon.OnTrayNotification(wParam, lParam);
}

void CvpnDlg::OnBnClickedMfclink1()
{
	// TODO: 在此添加控件通知处理程序代码
	//注册
}


void CvpnDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	int	nItem = m_list.GetSelectionMark();
	if (nItem == -1)
	{
		CString statusstr = "没有找到任何可用的VPN服务器！";
		OutputString(statusstr.GetBuffer(0));
		UpdateData(FALSE);
		return;
	}

	if (NULL != g_TerminalEvent)
	{
		AfxMessageBox("已经存在一个活动连接!");
		return;
	}
	else
	{
		if ((g_TerminalEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		{
			OutputString("CreateEvent failed with error %d\n", GetLastError());
			return;
		}

	}

	CString statusstr="正在连接中！";
	OutputString(statusstr.GetBuffer(0));

	OnSysCommand(SC_MINIMIZE,NULL); //最小化到托盘
	//登陆
	int isremember=m_remember.GetCheck(); //0:不记密码 1:记住密码
	if (isremember==1)
	{
		//写入config.ini
		//SaveConfig();
	}
	try
	{
		g_hDisconnect[0] = CreateEvent(NULL, FALSE, FALSE, NULL);

		ResetEvent(g_hDisconnect[0]); // 重置事件对象
		VPNPARAMS VPNParams;
		VPNParams.hTerminalEvent = g_hDisconnect[0];
		CString vpnname="myvpn";
		UpdateData(TRUE);
		lstrcpy(VPNParams.szServer, m_ip.GetBuffer(0));
		lstrcpy(VPNParams.szUserName, m_username.GetBuffer(0));
		lstrcpy(VPNParams.szPassword, m_password.GetBuffer(m_password.GetLength()));
		lstrcpy(VPNParams.szDescription, vpnname.GetBuffer(0));
		lstrcpy(VPNParams.szDomain, "");
		VPNParams.lpMain = this;
		UpdateData(FALSE);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConenectVPN, (LPVOID)&VPNParams, 0, NULL);
		Sleep(500); // 等待参数传递完毕 !!!!!
	}
	catch(...)
	{

	}
}


void CvpnDlg::OnBnClickedButton2()
{
	//断开
	SetEvent(g_hDisconnect[0]);
	CloseHandle(g_hDisconnect[0]);
	m_TrayIcon.SetTooltipText("断开连接!");
}

void CvpnDlg::ReadConfig(CString filepath)
{
	//char chHost[40], chUserName[40], chPassWord[40], chDescription[40];
	try
	{
		ifstream file;
		char line[256];
		file.open(filepath);
		if(!file)
		{
			return;
		}
		while(file.getline(line,sizeof(line)))
		{
			CString str;
			str.Format("%s",line);
			//ip开始的位置
			size_t ip= str.Find("|");
			m_ip = str.Left(ip);
			CString str1=str.Right(str.GetLength()-ip-1);  //截取剩下的字符串
			//username开始的位置
			size_t uname=str1.Find("|");
			m_username=str1.Left(uname);
			CString str2=str1.Right(str1.GetLength()-uname-1);
			//password开始的位置
			size_t pass=str2.Find("|");
			m_password=str2.Left(pass);
		}
	}
	catch(...)
	{

	}
}

void CvpnDlg::SaveConfig()
{

	try{
		CString	filepath = "config.ini";

		CStdioFile	file(filepath, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		UpdateData(TRUE);
		CString strip;
		((CComboBox*)GetDlgItem(IDC_COMBO))->GetWindowText(strip);
		file.WriteString(strip + "|");
		file.WriteString(m_username + "|");
		file.WriteString(m_password+"|");
		file.Close();
	}
	catch(...)
	{
	
	}
}

// 初始化数据
void CvpnDlg::InitData(void)
{
	//CString ip = "103.230.241.24";
	//CString username = "testvpn";
	//CString password = "123456";
	//CString remark = "测试";
	//int result = 0;
	//result = this->Add(ip.GetBuffer(), username.GetBuffer(), password.GetBuffer(), remark.GetBuffer());
	//if (result > 0)
	//{
	//	OutputString("添加成功！\n");
	//}
	//else
	//{
	//	OutputString("添加失败！\n");
	//}

	//result = this->Del(2);
	//if (result > 0)
	//{
	//	OutputString("删除成功！\n");
	//}
	//else
	//{
	//	OutputString("删除失败！\n");
	//}

	this->m_list.DeleteAllItems();
	CppSQLite3DB db;
	db.open("VPN.db");
	CppSQLite3Query query = db.execQuery("select rowid,* from vpninfo where 1=1 order by rowid");
	int i = 0;
	while (!query.eof())
	{
		this->m_list.InsertItem(i, query.getStringField("rowid"));
		this->m_list.SetItemText(i, 1, query.getStringField("ip"));
		this->m_list.SetItemText(i, 2, GetPosition((char *)query.getStringField("ip")));
		this->m_list.SetItemText(i, 4, (char *)query.getStringField("remark"));
		VpnInfo *pVpnInfo = new VpnInfo;
		ZeroMemory(pVpnInfo, sizeof(VpnInfo));
		pVpnInfo->id = atoi(query.getStringField("rowid"));
		sprintf(pVpnInfo->ip, "%s", query.getStringField("ip"));
		sprintf(pVpnInfo->user, "%s", query.getStringField("user"));
		sprintf(pVpnInfo->pass, "%s", query.getStringField("pass"));
		this->m_list.SetItemData(i, (DWORD)pVpnInfo);
		query.nextRow();
		//默认值
		if (i == 0)
		{
			//ip
			m_ip = pVpnInfo->ip;
			//user
			m_username = pVpnInfo->user;
			//password
			m_password = pVpnInfo->pass;

			SetDlgItemText(IDC_EDIT1, m_ip);
			SetDlgItemText(IDC_EDIT2, m_username);
			SetDlgItemText(IDC_EDIT3, m_password);
		}
		i++;
	}
	query.finalize();
	db.close();
}


CString CvpnDlg::GetPosition(char* lpIPAddr)
{
	CIpFinder m_ipfind;
	if (!m_ipfind.Open("qqwry.dat"))
	{
		return CString(_T("qqwry.dat不存在"));
	}
	std::string country, location;
	m_ipfind.GetAddressByIp(lpIPAddr, country, location);
	m_ipfind.Close();
	CString result;
	result.Format(_T("%s %s"), country.c_str(), location.c_str());
	return result;
}

// 添加记录
int CvpnDlg::Add(LPSTR lpHostName, LPSTR lpUser, LPSTR lpPass, LPSTR lpRemark)
{
	char *sql = "insert into vpninfo (ip,user,pass,remark) values ('%s','%s','%s','%s')";
	char *lpSql = new char[4096];
	ZeroMemory(lpSql, 4096);
	sprintf(lpSql, sql, lpHostName, lpUser, lpPass, lpRemark);
	int result = 0;
	CppSQLite3DB db;
	db.open("VPN.db");
	result = db.execDML(lpSql);
	delete[]lpSql;
	db.close();
	return result;
}

// 删除记录
int CvpnDlg::Del(int rowid)
{
	char *lpSql = new char[4096];
	ZeroMemory(lpSql, 4096);
	sprintf(lpSql, "delete from vpninfo where rowid=%d", rowid);
	int result = 0;
	CppSQLite3DB db;
	db.open("VPN.db");
	result = db.execDML(lpSql);
	delete[]lpSql;
	db.close();
	return result;
}


#define WEBCONFIG_KEY	TEXT("http")
static int writer(char *data, size_t size, size_t nmemb, string *writeData)
{
	if (NULL == writeData)
		return 0;
	int len = (int)(size * nmemb);
	writeData->append(data, len);

	return len;
}

static BOOL GetReturnBase_Post(string strURL, const char* content, string& strtreturn)
{
	CURL *curl = curl_easy_init();

	if (NULL == curl)
		return 0;

	char errorBuffer[CURL_ERROR_SIZE];

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
	curl_easy_setopt(curl, CURLOPT_HEADER, false);
	curl_easy_setopt(curl, CURLOPT_URL, strURL.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, true);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 10000);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strtreturn);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, &content);

	if (CURLE_OK == curl_easy_perform(curl))
	{
		curl_easy_cleanup(curl);
		return TRUE;
	}

	curl_easy_cleanup(curl);
	return FALSE;
}

static BOOL GetReturnBase_Get(string strURL, const char* content, string& strreturn)
{
	CURL *curl = curl_easy_init();

	if (NULL == curl)
		return 0;

	char errorBuffer[CURL_ERROR_SIZE];

	strURL += "?";
	strURL += content;
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_URL, strURL.c_str());
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
	curl_easy_setopt(curl, CURLOPT_HEADER, false);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 10000);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strreturn);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, &content);

	if (CURLE_OK == curl_easy_perform(curl))
	{
		curl_easy_cleanup(curl);
		return TRUE;
	}

	curl_easy_cleanup(curl);
	return FALSE;
}

//网页配置数据
bool CvpnDlg::VpnWebConfig()
{
	TCHAR	szTBuffer1[MAX_PATH] = { 0 };
	TCHAR	szTBuffer2[MAX_PATH] = { 0 };
	TCHAR	szTBuffer3[MAX_PATH] = { 0 };
	GetPrivateProfileString(WEBCONFIG_KEY, TEXT("vpn"), TEXT("/VPN/riskmanage"), szTBuffer1, sizeof(szTBuffer1), m_szConfigFile);
	GetPrivateProfileString(WEBCONFIG_KEY, TEXT("weburl"), TEXT("/VPN/riskmanage"), szTBuffer2, sizeof(szTBuffer2), m_szConfigFile);
	GetPrivateProfileString(WEBCONFIG_KEY, TEXT("webkey"), TEXT("/VPN/riskmanage"), szTBuffer3, sizeof(szTBuffer3), m_szConfigFile);

	std::string strReturn;
	std::string strHttp = CTranscoding::TcharToChar(szTBuffer2);
	strHttp += CTranscoding::TcharToChar(szTBuffer1);

	//std::string strmd5 = "cyvpn2019" + strMachineID + CTranscoding::TcharToChar(szTBuffer3);

	char szMD5[128] = { 0 };
	//GetMD5Hash(szMD5, (unsigned char*)strmd5.c_str(), strmd5.length());

	char szcontent[1024];
	//_snprintf_s(szcontent, sizeof(szcontent) - 1, "deviceToken=%s&machineid=%s&sign=%s", strDeviceToken.c_str(), strMachineID.c_str(), szMD5);

	GetReturnBase_Get(strHttp.c_str(), szcontent, strReturn);

	std::string httphead = CTranscoding::LuaString_Procedure(strHttp.c_str());
	std::string httpcontent = CTranscoding::LuaString_Procedure(szcontent);
	std::string httpreturn = CTranscoding::LuaString_Procedure(strReturn.c_str());

	//TraceRequest(httphead, httpcontent, httpreturn);

	Json::Reader json;
	Json::Value root;
	json.parse(strReturn, root);

	if (root.isMember("info") && root.isMember("status"))
	{
		std::string strinfo;

		if (root["info"].isString())
			strinfo = root["info"].asString();

		std::string info = CTranscoding::LuaString_Procedure(strinfo.c_str());

		if (root["status"].isNumeric() && root["status"].asInt() != 1)
		{
			//g_pCServiceUnits->TraceWebLog(TEXT("发送aliapi失败: 设备号:%s,机器码:%s, 返回描述:%s"),
			//	pRecv->szDeviceToken, pRecv->szMachineID, info.c_str());

			return false;
		}
		else
		{
			//g_pCServiceUnits->TraceWebLog(TEXT("发送aliapi成功: 设备号:%s,机器码:%s, 返回描述:%s"),
			//	pRecv->szDeviceToken, pRecv->szMachineID, info.c_str());

			return true;

		}
	}

	return false;
}

void CvpnDlg::OnCbnSelchangeCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	int iPos;
	CString  strTemp;
	iPos = ((CComboBox*)GetDlgItem(IDC_COMBO))->GetCurSel();//当前选中的行。
	((CComboBox*)GetDlgItem(IDC_COMBO))->SetCurSel(iPos);//设置第n行内容为显示的内容。
	((CComboBox*)GetDlgItem(IDC_COMBO))->GetWindowText(strTemp);

}


void CvpnDlg::OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (this->m_list.GetSelectedCount() < 1)
	{
		return;
	}
	POSITION pos = this->m_list.GetFirstSelectedItemPosition();
	VpnInfo *pInfo = NULL;
	while (pos)
	{
		int nIdx = -1;
		nIdx = this->m_list.GetNextSelectedItem(pos);
		if (nIdx >= 0 && nIdx < this->m_list.GetItemCount())
		{
			pInfo = (VpnInfo *)(this->m_list.GetItemData(nIdx));
			if (pInfo != NULL)
			{

				//this->m_hrascon = NULL;
				//int result = ConenectVPN("YVPN连接", pInfo->user, pInfo->pass, pInfo->ip, &this->m_hrascon);
				//pInfo = NULL;
				//if (result != 0)
				//{
				//	char *temp = new char[1024];
				//	ZeroMemory(temp, 1024);
				//	sprintf(temp, "连接失败！错误代码：%d ...", result);
				//	ShowInfo(temp);
				//	delete temp;
				//}

				{
					//ip
					m_ip = pInfo->ip;
					//user
					m_username = pInfo->user;
					//password
					m_password = pInfo->pass;

					SetDlgItemText(IDC_EDIT1, m_ip);
					SetDlgItemText(IDC_EDIT2, m_username);
					SetDlgItemText(IDC_EDIT3, m_password);
				}

			}
		}
	}

	*pResult = 0;
}

void CvpnDlg::OnMenuAdd()
{
	// TODO: 在此添加命令处理程序代码
	//AddItem dlg;
	//if (dlg.DoModal() == 2)
	//{
	//	return;
	//}
	//int result = 0;
	//result = this->Add(dlg.m_host.GetBuffer(), dlg.m_user.GetBuffer(), dlg.m_pass.GetBuffer(), dlg.m_remark.GetBuffer());
	//if (result > 0)
	//{
	//	ShowInfo("添加成功！");
	//	InitData();
	//	return;
	//}
	//ShowInfo("添加失败！");
}




void CvpnDlg::OnMenuDel()
{
	// TODO: 在此添加命令处理程序代码
	////this->Del(15);
	//if (this->m_list.GetSelectedCount() < 1)
	//{
	//	return;
	//}
	//if (MessageBox("是否真的执行此操作？", "提示", MB_YESNO) == IDNO)
	//{
	//	return;
	//}
	//POSITION pos = this->m_list.GetFirstSelectedItemPosition();
	//Info *pInfo = NULL;
	//while (pos)
	//{
	//	int nIdx = -1;
	//	nIdx = this->m_list.GetNextSelectedItem(pos);
	//	if (nIdx >= 0 && nIdx < this->m_list.GetItemCount())
	//	{
	//		pInfo = (Info *)(this->m_list.GetItemData(nIdx));
	//		if (pInfo != NULL)
	//		{
	//			int result = 0;
	//			result = this->Del(pInfo->id);
	//			if (result > 0)
	//			{
	//				ShowInfo("删除成功！");
	//				InitData();
	//			}
	//			else
	//			{
	//				ShowInfo("删除失败！");
	//			}
	//		}
	//	}
	//}
}

