
// vpnDlg.h : 头文件
//
#if !defined(AFX_VPNCLIENTDLG_H__E3DF5BD6_0E0A_4409_A50D_B1025AECCF43__INCLUDED_)
#define AFX_VPNCLIENTDLG_H__E3DF5BD6_0E0A_4409_A50D_B1025AECCF43__INCLUDED_

#include "TrayIcon.h"
#include "afxwin.h"
#include "Util.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class VpnInfo
{
public:
	int  id;
	char ip[50];
	char location[256];
	char user[50];
	char pass[50];
	char remark[256];
};

// CvpnDlg 对话框
class CvpnDlg : public CDialogEx
{
// 构造
public:
	CvpnDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_VPN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
	CTrayIcon m_TrayIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 图片地址
	CStatic m_ctrlpic;
	void ReadConfig(CString filepath);
	void SaveConfig();
	// 初始化数据
	void InitData(void);
	CString GetPosition(char* lpIPAddr);
	// 添加记录
	int Add(LPSTR lpHostName, LPSTR lpUser, LPSTR lpPass, LPSTR lpRemark);
	// 删除记录
	int Del(int rowid);
	//配置数据
	bool VpnWebConfig();

	afx_msg void OnBnClickedMfclink1();
	afx_msg void OnBnClickedButton1();
	CComboBox m_ip;
	CString m_username;
	CString m_password;
	TCHAR	m_szConfigFile[MAX_PATH];
	afx_msg void OnBnClickedButton2();
	// 记住密码
	CButton m_remember;
	CString configFile;
protected:
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
public:
	// 设置当前状态
	void SetStatus(CString StatusStr);
	afx_msg void OnCbnSelchangeCombo();
};

#endif // !defined(AFX_VPNCLIENTDLG_H__E3DF5BD6_0E0A_4409_A50D_B1025AECCF43__INCLUDED_)
