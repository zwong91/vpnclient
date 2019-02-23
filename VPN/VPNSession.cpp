// VPNSession.cpp: implementation of the CVPNSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "VPNDlg.h"
#include "VPNSession.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVPNSession::CVPNSession()
{
	m_network="0.0.0.0";
	m_netmask="0.0.0.0";
	m_authType=0;
	m_ipsecMode=0;
	m_ipsecEncryption=2;
	m_ipsecAuth=1;
	m_ipsecPFS=true;
	m_ipsecRekeyData=50000;
	m_ipsecRekeyTime=3600;
	m_blockNonIPSec=0;
	m_allowLocalTraffic=0;
}

CVPNSession::~CVPNSession()
{

}

CString CVPNSession::GetFirstVPNIP()
{
	CString firstIP;
	BYTE ip1,ip2,ip3,ip4;
	BYTE nm1,nm2,nm3,nm4;
	
	splitIp(m_network,ip1,ip2,ip3,ip4);
	splitIp(m_netmask,nm1,nm2,nm3,nm4);

	if(nm4==255)
	{
		firstIP=m_network;
	}
	else
	{
		joinIp(ip1,ip2,ip3,ip4+1,firstIP);
	}

	return firstIP;
}

void CVPNSession::splitIp(const CString &ip, BYTE &ip1, BYTE &ip2, BYTE &ip3, BYTE &ip4)
{
	CString temp;
	int pos1=0;
	int pos2=0;

	pos1=ip.Find(".",0);
	temp=ip.Left(pos1);
	ip1=atoi(temp);

	pos1++;
	pos2=ip.Find(".",pos1);
	temp=ip.Mid(pos1,pos2-pos1);
	ip2=atoi(temp);
	pos1=pos2+1;

	pos2=ip.Find(".",pos1);
	temp=ip.Mid(pos1,pos2-pos1);
	ip3=atoi(temp);
	pos1=pos2+1;

	temp=ip.Right(ip.GetLength()-pos1);
	ip4=atoi(temp);
}

void CVPNSession::joinIp(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, CString& ip)
{
	ip.Format("%i.%i.%i.%i",ip1,ip2,ip3,ip4);
}
