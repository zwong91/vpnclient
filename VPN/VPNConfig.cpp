// VPNConfig.cpp: implementation of the CVPNConfig class.
//
//////////////////////////////////////////////////////////////////////

/*******************************************************************************
 *                                                                             *
 *  This file is part of VPNDialer.                                            *
 *                                                                             *
 *  VPNDialer is free software; you can redistribute it and/or modify          *
 *  it under the terms of the GNU General Public License as published by       *
 *  the Free Software Foundation; either version 2 of the License, or          *
 *  (at your option) any later version.                                        *
 *                                                                             *
 *  VPNDialer is distributed in the hope that it will be useful,               *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 *  GNU General Public License for more details.                               *
 *                                                                             *
 *  You should have received a copy of the GNU General Public License          *
 *  along with VPNDialer; if not, write to the Free Software                   *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  *
 *                                                                             *
 *  Copyright (c) 2003 Evidian GmbH, written by Thomas Kriener                 *
 *                                                                             *
 *******************************************************************************/

#include "stdafx.h"
#include "VPNDialer.h"
#include "VPNConfig.h"
#include "VPNSession.h"
#include "SelectInterfaceDialog.h"

#include <conio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define bzero(a) memset(a,0,sizeof(a)) //easier -- shortcut

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVPNConfig::CVPNConfig()
{
	HKEY    hKey;
	DWORD   valueSize;
	DWORD   valueType;
	LONG    regRetVal;
	char*   value;
	int     intValue;
	int     i=0;
	CString key;

	do
	{
		CVPNSession actSession;
		key.Format("SOFTWARE\\Evidian\\VPNDialer\\Session%i",i);
		// get Registry-Key
		regRetVal=RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		                       key,
						       0,
						       KEY_QUERY_VALUE,
						       &hKey);
		if(regRetVal==ERROR_SUCCESS)
		{
			// Name of Connection
			valueType=REG_SZ;
			if(RegQueryValueEx(hKey,
			                   "Name",
							   NULL,
							   &valueType,
							   NULL,
							   &valueSize)==ERROR_SUCCESS)
			{
				value=new char[valueSize];
				if(RegQueryValueEx(hKey,
				                   "Name",
								   NULL,
								   &valueType,
								   (LPBYTE)value,
								   &valueSize)==ERROR_SUCCESS)
				{
					actSession.m_name=value;
				}
				delete value;
			}
			// Gateway
			valueType=REG_SZ;
			if(RegQueryValueEx(hKey,
			                   "Gateway",
							   NULL,
							   &valueType,
							   NULL,
							   &valueSize)==ERROR_SUCCESS)
			{
				value=new char[valueSize];
				if(RegQueryValueEx(hKey,
				                   "Gateway",
								   NULL,
								   &valueType,
								   (LPBYTE)value,
								   &valueSize)==ERROR_SUCCESS)
				{
					actSession.m_gateway=value;
				}
				delete value;
			}
			// Network
			valueType=REG_SZ;
			if(RegQueryValueEx(hKey,
			                   "Network",
							   NULL,
							   &valueType,
							   NULL,
							   &valueSize)==ERROR_SUCCESS)
			{
				value=new char[valueSize];
				if(RegQueryValueEx(hKey,
				                   "Network",
								   NULL,
								   &valueType,
								   (LPBYTE)value,
								   &valueSize)==ERROR_SUCCESS)
				{
					actSession.m_network=value;
				}
				delete value;
			}
			// Netmask
			valueType=REG_SZ;
			if(RegQueryValueEx(hKey,
			                   "Netmask",
							   NULL,
							   &valueType,
							   NULL,
							   &valueSize)==ERROR_SUCCESS)
			{
				value=new char[valueSize];
				if(RegQueryValueEx(hKey,
				                   "Netmask",
								   NULL,
								   &valueType,
								   (LPBYTE)value,
								   &valueSize)==ERROR_SUCCESS)
				{
					actSession.m_netmask=value;
				}
				delete value;
			}
			// AuthType
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			if(RegQueryValueEx(hKey,
			                   "AuthType",
							   NULL,
							   &valueType,
							   (LPBYTE)&intValue,
							   &valueSize)==ERROR_SUCCESS)
			{
				actSession.m_authType=intValue;
			}
			// AuthValue
			valueType=REG_MULTI_SZ;
			if(RegQueryValueEx(hKey,
			                   "AuthValue",
							   NULL,
							   &valueType,
							   NULL,
							   &valueSize)==ERROR_SUCCESS)
			{
				value=new char[valueSize];
				if(RegQueryValueEx(hKey,
				                   "AuthValue",
								   NULL,
								   &valueType,
								   (LPBYTE)value,
								   &valueSize)==ERROR_SUCCESS)
				{
					for(DWORD j=0; j<valueSize; j++)
					{
						if((value[j]=='\0') && (j!=(valueSize-1)))
							value[j]='\n';
					}
					actSession.m_authValue=value;
				}
				delete value;
			}
			// IpsecMode
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			if(RegQueryValueEx(hKey,
			                   "IpsecMode",
							   NULL,
							   &valueType,
							   (LPBYTE)&intValue,
							   &valueSize)==ERROR_SUCCESS)
			{
				actSession.m_ipsecMode;
			}
			// IpsecEncryption
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			if(RegQueryValueEx(hKey,
			                   "IpsecEncryption",
							   NULL,
							   &valueType,
							   (LPBYTE)&intValue,
							   &valueSize)==ERROR_SUCCESS)
			{
				actSession.m_ipsecEncryption=intValue;
			}
			// IpsecAuth
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			if(RegQueryValueEx(hKey,
			                   "IpsecAuth",
							   NULL,
							   &valueType,
							   (LPBYTE)&intValue,
							   &valueSize)==ERROR_SUCCESS)
			{
				actSession.m_ipsecAuth=intValue;
			}
			// IpsecPFS
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			if(RegQueryValueEx(hKey,
			                   "IpsecPFS",
							   NULL,
							   &valueType,
							   (LPBYTE)&intValue,
							   &valueSize)==ERROR_SUCCESS)
			{
				actSession.m_ipsecPFS=intValue;
			}
			// IpsecRekeyData
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			if(RegQueryValueEx(hKey,
			                   "IpsecRekeyData",
							   NULL,
							   &valueType,
							   (LPBYTE)&intValue,
							   &valueSize)==ERROR_SUCCESS)
			{
				actSession.m_ipsecRekeyData=intValue;
			}
			// IpsecRekeyTime
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			if(RegQueryValueEx(hKey,
			                   "IpsecRekeyTime",
							   NULL,
							   &valueType,
							   (LPBYTE)&intValue,
							   &valueSize)==ERROR_SUCCESS)
			{
				actSession.m_ipsecRekeyTime=intValue;
			}
			// RAS
			valueType=REG_SZ;
			if(RegQueryValueEx(hKey,
			                   "RAS",
							   NULL,
							   &valueType,
							   NULL,
							   &valueSize)==ERROR_SUCCESS)
			{
				value=new char[valueSize];
				if(RegQueryValueEx(hKey,
				                   "RAS",
								   NULL,
								   &valueType,
								   (LPBYTE)value,
								   &valueSize)==ERROR_SUCCESS)
				{
					actSession.m_ras=value;
				}
				delete value;
			}
			// L2TP
			valueType=REG_SZ;
			if(RegQueryValueEx(hKey,
			                   "L2TP",
							   NULL,
							   &valueType,
							   NULL,
							   &valueSize)==ERROR_SUCCESS)
			{
				value=new char[valueSize];
				if(RegQueryValueEx(hKey,
				                   "L2TP",
								   NULL,
								   &valueType,
								   (LPBYTE)value,
								   &valueSize)==ERROR_SUCCESS)
				{
					actSession.m_l2tp=value;
				}
				delete value;
			}
			// BlockNonIPSec
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			if(RegQueryValueEx(hKey,
			                   "BlockNonIPSec",
							   NULL,
							   &valueType,
							   (LPBYTE)&intValue,
							   &valueSize)==ERROR_SUCCESS)
			{
				actSession.m_blockNonIPSec=intValue;
			}
			// AllowLocalTraffic
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			if(RegQueryValueEx(hKey,
			                   "AllowLocalTraffic",
							   NULL,
							   &valueType,
							   (LPBYTE)&intValue,
							   &valueSize)==ERROR_SUCCESS)
			{
				actSession.m_allowLocalTraffic=intValue;
			}


			// Add Session to Array
			if(actSession.m_name!="")
				m_vpnSessions.Add(actSession);

			// Close Session-Key
			RegCloseKey(hKey);
		}

		// Try next Session
		i++;
	} while (regRetVal==ERROR_SUCCESS);


}

CVPNConfig::~CVPNConfig()
{

}

int CVPNConfig::SaveConfig()
{
	HKEY    hKey;
	DWORD   valueSize;
	DWORD   valueType;
	CString key;
	LONG    regRetVal;
	char*   value;
	int     intValue;
	int     i;

	for(i=0;i<m_vpnSessions.GetSize();i++)
	{
		CVPNSession actSession=m_vpnSessions[i];
		key.Format("SOFTWARE\\Evidian\\VPNDialer\\Session%i",i);
		// open Key with write-rights
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		                  key,
						  0,
						  "CLASS",
						  REG_OPTION_NON_VOLATILE,
						  KEY_WRITE,
						  NULL,
						  &hKey,
						  NULL)==ERROR_SUCCESS)
		{
			// Name
			valueSize=actSession.m_name.GetLength()+1;
			valueType=REG_SZ;
			if(RegSetValueEx(hKey,
			                 "Name",
							 NULL,
							 valueType,
							 (LPBYTE)(LPCSTR)actSession.m_name,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}

			// Gateway
			valueSize=actSession.m_gateway.GetLength()+1;
			valueType=REG_SZ;
			if(RegSetValueEx(hKey,
			                 "Gateway",
							 NULL,
							 valueType,
							 (LPBYTE)(LPCSTR)actSession.m_gateway,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// Network
			valueSize=actSession.m_network.GetLength()+1;
			valueType=REG_SZ;
			if(RegSetValueEx(hKey,
			                 "Network",
							 NULL,
							 valueType,
							 (LPBYTE)(LPCSTR)actSession.m_network,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// Netmask
			valueSize=actSession.m_netmask.GetLength()+1;
			valueType=REG_SZ;
			if(RegSetValueEx(hKey,
			                 "Netmask",
							 NULL,
							 valueType,
							 (LPBYTE)(LPCSTR)actSession.m_netmask,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// AuthType
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			intValue=actSession.m_authType;
			if(RegSetValueEx(hKey,
			                 "AuthType",
							 NULL,
							 valueType,
							 (LPBYTE)&intValue,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// AuthValue
			valueSize=actSession.m_authValue.GetLength()+1;
			valueType=REG_MULTI_SZ;
			value=new char[valueSize];
			strcpy(value,actSession.m_authValue);

			for(DWORD j=0; j<valueSize; j++)
			{
				if(value[j]=='\n')
					value[j]='\0';
			}

			if(RegSetValueEx(hKey,
			                 "AuthValue",
							 NULL,
							 valueType,
							 (LPBYTE)value,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// IpsecMode
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			intValue=actSession.m_ipsecMode;
			if(RegSetValueEx(hKey,
			                 "IpsecMode",
							 NULL,
							 valueType,
							 (LPBYTE)&intValue,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// IpsecEncryption
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			intValue=actSession.m_ipsecEncryption;
			if(RegSetValueEx(hKey,
			                 "IpsecEncryption",
							 NULL,
							 valueType,
							 (LPBYTE)&intValue,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// IpsecAuth
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			intValue=actSession.m_ipsecAuth;
			if(RegSetValueEx(hKey,
			                 "IpsecAuth",
							 NULL,
							 valueType,
							 (LPBYTE)&intValue,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// IpsecPFS
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			intValue=actSession.m_ipsecPFS;
			if(RegSetValueEx(hKey,
			                 "IpsecPFS",
							 NULL,
							 valueType,
							 (LPBYTE)&intValue,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// IpsecRekeyData
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			intValue=actSession.m_ipsecRekeyData;
			if(RegSetValueEx(hKey,
			                 "IpsecRekeyData",
							 NULL,
							 valueType,
							 (LPBYTE)&intValue,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// IpsecRekeyTime
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			intValue=actSession.m_ipsecRekeyTime;
			if(RegSetValueEx(hKey,
			                 "IpsecRekeyTime",
							 NULL,
							 valueType,
							 (LPBYTE)&intValue,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// RAS
			valueSize=actSession.m_ras.GetLength()+1;
			valueType=REG_SZ;
			if(RegSetValueEx(hKey,
			                 "RAS",
							 NULL,
							 valueType,
							 (LPBYTE)(LPCSTR)actSession.m_ras,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// L2TP
			valueSize=actSession.m_l2tp.GetLength()+1;
			valueType=REG_SZ;
			if(RegSetValueEx(hKey,
			                 "L2TP",
							 NULL,
							 valueType,
							 (LPBYTE)(LPCSTR)actSession.m_l2tp,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// BlockNonIPSec
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			intValue=actSession.m_blockNonIPSec;
			if(RegSetValueEx(hKey,
			                 "BlockNonIPSec",
							 NULL,
							 valueType,
							 (LPBYTE)&intValue,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}
			// AllowLocalTraffic
			valueType=REG_DWORD;
			valueSize=sizeof(intValue);
			intValue=actSession.m_allowLocalTraffic;
			if(RegSetValueEx(hKey,
			                 "AllowLocalTraffic",
							 NULL,
							 valueType,
							 (LPBYTE)&intValue,
							 valueSize)!=ERROR_SUCCESS)
			{
				return 1;
			}

			RegCloseKey(hKey);
		}
	}

	do
	{
		key.Format("SOFTWARE\\Evidian\\VPNDialer\\Session%i",i);
		// get Last Date from Registry
		regRetVal=RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		                       key,
						       0,
						       KEY_QUERY_VALUE,
						       &hKey);
		RegCloseKey(hKey);

		if(regRetVal==ERROR_SUCCESS)
		{
			RegDeleteKey(HKEY_LOCAL_MACHINE, key);
		}

		// Try next Session
		i++;
	} while (regRetVal==ERROR_SUCCESS);

	return 0;

}

CString CVPNConfig::GetIPSecTool()
{

	switch(GetOSVersion())
	{
	case OS_WIN2K:
			return "ipsecpol";
			break;
	case OS_WINXP:
			return "ipseccmd";
			break;
	default:
		return "";
	} 
	return "";
}

int CVPNConfig::ActivateIPSec(int actConfig, const CString& myIP, CString& log)
{
	CString ipsectool=GetIPSecTool();
	CString tmp, tmp2, tmp3;
	CString tunnel, myNet, myTunIP, partnerNet, authMode, reKey;
	CArray<CString,CString> myIPs, myNetmasks;
	int     res;
	int     pos1,pos2;

	/*
	 * First remove old Session
	 */
	RemoveIPSec();


	/*
	 * Create Tunnels
	 */
	// if myIP is empty just use a lan-one
	if(myIP=="")
	{
		GetLanIPs(myIPs,myNetmasks);
		if(myIPs.GetSize()>1)
		{
			// Select Interface to use for Tunnel
			CSelectInterfaceDialog dlg;
			dlg.m_interfaces=&myIPs;
			dlg.DoModal();
			myNet.Format("%s/255.255.255.255",myIPs[dlg.m_selectedInterface]);
			myTunIP=myIPs[dlg.m_selectedInterface];
		}
		else
		{
			myNet.Format("%s/255.255.255.255",myIPs[0]);
			myTunIP=myIPs[0];
		}
	}
	else
	{
		myNet.Format("%s/255.255.255.255",myIP);
		myTunIP=myIP;
	}
	if(m_vpnSessions[actConfig].m_network=="0.0.0.0")
		partnerNet="*";
	else
		partnerNet.Format("%s/%s",m_vpnSessions[actConfig].m_network,m_vpnSessions[actConfig].m_netmask);

	switch(m_vpnSessions[actConfig].m_ipsecAuth)
	{
	case VPN_IPSECAUTH_NONE:
		authMode = "none";
		break;
	case VPN_IPSECAUTH_MD5:
		authMode = "MD5";
		break;
	case VPN_IPSECAUTH_SHA:
		authMode = "SHA";
		break;
	default:
		AfxMessageBox("Wrong Auth-Method for IPSEC",MB_OK|MB_ICONERROR);
	}

	reKey.Format("%iS/%iK",m_vpnSessions[actConfig].m_ipsecRekeyTime,m_vpnSessions[actConfig].m_ipsecRekeyData);
	
	tmp2=m_vpnSessions[actConfig].m_name;
	tmp2.Replace(" ","-");
	tunnel.Format("%s -w REG -p VPNDialer -r Host-%s -t %s ",ipsectool,tmp2,m_vpnSessions[actConfig].m_gateway);
	tmp.Format("-f %s=%s -n ESP[%s,3DES]%s",myNet,partnerNet,authMode,reKey);
	tunnel+=tmp;

	if(m_vpnSessions[actConfig].m_ipsecPFS)
		tunnel+="PFS";
	
	if(m_vpnSessions[actConfig].m_authType==VPN_AUTH_CERTIFICATE)
	{
		tmp=" -a";

		pos1=0;
		do
		{
			pos2=m_vpnSessions[actConfig].m_authValue.Find("\n",pos1);
			tmp2=m_vpnSessions[actConfig].m_authValue.Mid(pos1,pos2-pos1);
			if(tmp2.GetLength()==0)
			{
				tmp2=m_vpnSessions[actConfig].m_authValue.Right(m_vpnSessions[actConfig].m_authValue.GetLength()-pos1);
			}
			if(tmp2.GetLength()>0)
			{
				tmp3.Format(" CERT:\"%s\"",tmp2);
				tmp+=tmp3;
			}
			pos1=pos2+1;
		}
		while(pos2>0);
	}
	else
		tmp.Format(" -a PRESHARE:\"%s\"",m_vpnSessions[actConfig].m_authValue);
	
	tunnel+=tmp;

	if(m_vpnSessions[actConfig].m_ras!="")
		tunnel+=" -dialup";
	else
		tunnel+=" -lan";

	if(m_vpnSessions[actConfig].m_ipsecPFS)
		tunnel+=" -1p";


#ifdef _DEBUG
	tmp.Format("Command 1: %s\r\n",tunnel);
	log+=tmp;
	AfxMessageBox(tmp);
#endif

	res = CVPNConfig::ExecuteCmd(tunnel,tmp);
	if (res!=0)
	{
		tmp.Format("Fehler bei Command: %s\r\n",tunnel);
		log+=tmp;
		AfxMessageBox(tmp,MB_OK|MB_ICONERROR);
		return -1;
	}

	tmp2=m_vpnSessions[actConfig].m_name;
	tmp2.Replace(" ","-");
	tunnel.Format("%s -w REG -p VPNDialer -r %s-Host -t %s ",ipsectool,tmp2,myTunIP);
	tmp.Format("-f %s=%s -n ESP[%s,3DES]%s",partnerNet,myNet,authMode,reKey);
	tunnel+=tmp;
	
	if(m_vpnSessions[actConfig].m_ipsecPFS)
		tunnel+="PFS";
	
	if(m_vpnSessions[actConfig].m_authType==VPN_AUTH_CERTIFICATE)
	{
		tmp=" -a";

		pos1=0;
		do
		{
			pos2=m_vpnSessions[actConfig].m_authValue.Find("\n",pos1);
			tmp2=m_vpnSessions[actConfig].m_authValue.Mid(pos1,pos2-pos1);
			if(tmp2.GetLength()==0)
			{
				tmp2=m_vpnSessions[actConfig].m_authValue.Right(m_vpnSessions[actConfig].m_authValue.GetLength()-pos1);
			}
			if(tmp2.GetLength()>0)
			{
				tmp3.Format(" CERT:\"%s\"",tmp2);
				tmp+=tmp3;
			}
			pos1=pos2+1;
		}
		while(pos2>0);
	}
	else
		tmp.Format(" -a PRESHARE:\"%s\"",m_vpnSessions[actConfig].m_authValue);
	
	tunnel+=tmp;

	if(m_vpnSessions[actConfig].m_ras!="")
		tunnel+=" -dialup";
	else
		tunnel+=" -lan";

	if(m_vpnSessions[actConfig].m_ipsecPFS)
		tunnel+=" -1p";

#ifdef _DEBUG
	tmp.Format("Command 2: %s\r\n",tunnel);
	log+=tmp;
	AfxMessageBox(tmp);
#endif

	res = CVPNConfig::ExecuteCmd(tunnel,tmp);
	if (res!=0)
	{
		tmp.Format("Fehler bei Command: %s\r\n",tunnel);
		log+=tmp;
		AfxMessageBox(tmp,MB_OK|MB_ICONERROR);
		return -1;
	}

	/*
	 * Block Traffic
	 */
	if(m_vpnSessions[actConfig].m_blockNonIPSec)
	{
		// Allow local Traffic
		if(m_vpnSessions[actConfig].m_allowLocalTraffic)
		{
			for(int i=0;i<myIPs.GetSize();i++)
			{
				tmp.Format("%s -w REG -p VPNDialer -r \"Allow local Traffic for %s\" -f %s+%s/%s -n PASS ",
				           ipsectool,myIPs[i],myIPs[i],GetNetwork(myIPs[i],myNetmasks[i]),myNetmasks[i]);
#ifdef _DEBUG
				tmp2.Format("Command : %s\r\n",tmp);
				log+=tmp2;
				AfxMessageBox(tmp2);
#endif
				res = CVPNConfig::ExecuteCmd(tmp,tmp2);
				if (res!=0)
				{
					tmp3.Format("Fehler bei Command: %s\r\n%s\r\n",tmp,tmp2);
					log+=tmp3;
					AfxMessageBox(tmp3,MB_OK|MB_ICONERROR);
					return -1;
				}
			}
		}
		// BLOCK the rest
		tmp.Format("%s -w REG -p VPNDialer -r \"Block non-IPSec\" -f *+0 -n BLOCK ",ipsectool);
#ifdef _DEBUG
		tmp2.Format("Command : %s\r\n",tmp);
		log+=tmp2;
		AfxMessageBox(tmp2);
#endif
		res = CVPNConfig::ExecuteCmd(tmp,tmp2);
		if (res!=0)
		{
			tmp3.Format("Fehler bei Command: %s\r\n%s\r\n",tmp,tmp2);
			log+=tmp3;
			AfxMessageBox(tmp3,MB_OK|MB_ICONERROR);
			return -1;
		}
	}

	/*
	 * Activate the policy
	 */
	log+="Activating policy...\r\n";
#ifdef _DEBUG
	AfxMessageBox("\tActivating policy...\n");
#endif
	tmp.Format("%s -w REG -p %s -x",ipsectool,"VPNDialer");

#ifdef _DEBUG
	tmp2.Format("Command 3: %s\r\n",tmp);
	log+=tmp2;
	AfxMessageBox(tmp2);
#endif

	res=CVPNConfig::ExecuteCmd(tmp,tmp2);
	if (res!=0)
	{
		tmp2.Format("Fehler bei Command: %s\r\n",tmp);
		log+=tmp2;
		AfxMessageBox(tmp2,MB_OK|MB_ICONERROR);
		return -1;
	}

	log+="IPSec active\r\n";
#ifdef _DEBUG
	AfxMessageBox("IPSec active");
#endif
	

	return(0);
}

int CVPNConfig::RemoveIPSec()
{
	CString ipsectool=GetIPSecTool();
	CString tmp,tmp1;

#ifdef _DEBUG
	AfxMessageBox("\tDeactivating old policy...\n");
#endif
	tmp.Format("%s -w REG -p %s -y",ipsectool,"VPNDialer");
	CVPNConfig::ExecuteCmd(tmp,tmp1);

#ifdef _DEBUG
	AfxMessageBox("\tRemoving old policy...\n");
#endif
	tmp.Format("%s -w REG -p %s -o",ipsectool,"VPNDialer");
	CVPNConfig::ExecuteCmd(tmp,tmp1);

	return 0;
}

int CVPNConfig::GetLanIPs(CArray<CString,CString>& IPs, CArray<CString,CString>& netmasks)
{
	CString tmp;

	IPs.RemoveAll();
	netmasks.RemoveAll();

	SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    if (sd == SOCKET_ERROR)
	{
        tmp.Format("Failed to get a socket. Error %i",WSAGetLastError());
		AfxMessageBox(tmp,MB_OK|MB_ICONERROR);
		return -1;
    }
    
	INTERFACE_INFO InterfaceList[20];
    unsigned long nBytesReturned;
    if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList, sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)
	{
        tmp.Format("Failed calling WSAIoctl: error %i", WSAGetLastError());
		AfxMessageBox(tmp,MB_OK|MB_ICONERROR);
		return -1;
	}
	
	int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);
#ifdef _DEBUG
	tmp.Format("There are %i interfaces.",nNumInterfaces);
	AfxMessageBox(tmp);
#endif
	
	for (int i = 0; i < nNumInterfaces; ++i)
	{
        u_long nFlags = InterfaceList[i].iiFlags;
        if (nFlags & IFF_UP)
		{
			// not point-to-point or loopback
			if(!((nFlags & IFF_POINTTOPOINT)||(nFlags & IFF_LOOPBACK)))
			{
				sockaddr_in *pAddress;
				pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
				tmp=inet_ntoa(pAddress->sin_addr);
				IPs.Add(tmp);
				
				pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
				tmp=inet_ntoa(pAddress->sin_addr);
				netmasks.Add(tmp);
			}
		}
    }

	return 0;
}

int CVPNConfig::ExecuteCmd(const CString &cmd, CString& output, BOOL wait)
{
	char *szCmd;
	CString temp;
	STARTUPINFO si;
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;		//security information for pipes
    PROCESS_INFORMATION pi;
	DWORD exitCode;
	HANDLE newstdin,newstdout,read_stdout,write_stdin;  //pipe handles
	char buf[1024];             //i/o buffer

	// Clear Ouptut
	output="";

	// Initialize Security for pipes
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, true, NULL, false);
    sa.lpSecurityDescriptor = &sd;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = true;         //allow inheritable handles

	// Create Pipes
	if (!CreatePipe(&newstdin,&write_stdin,&sa,0))   //create stdin pipe
	{
		AfxMessageBox("Error on CreatePipe stdin",MB_OK|MB_ICONERROR);
		getch();
		return -1;
	}
	
	if (!CreatePipe(&read_stdout,&newstdout,&sa,0))  //create stdout pipe
	{
		AfxMessageBox("Error on CreatePipe stdout",MB_OK|MB_ICONERROR);
		getch();
		CloseHandle(newstdin);
		CloseHandle(write_stdin);
		return -1;
	}
	
	//set startupinfo for the spawned process
	GetStartupInfo(&si);
	ZeroMemory( &pi, sizeof(pi) );

	/*
	The dwFlags member tells CreateProcess how to make the process.
	STARTF_USESTDHANDLES validates the hStd* members. STARTF_USESHOWWINDOW
	validates the wShowWindow member.
	*/
	si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = newstdout;
	si.hStdError = newstdout;     //set the new handles for the child process
	si.hStdInput = newstdin;

	// Copy CString to char*
	szCmd=new char[cmd.GetLength()+1];
	strcpy(szCmd,(LPCSTR)cmd);

	// Start the child process. 
	if( !CreateProcess( NULL,    // No module name (use command line). 
                        szCmd,   // Command line. 
                        NULL,    // Process handle not inheritable. 
                        NULL,    // Thread handle not inheritable. 
                        TRUE,    // Set handle inheritance to TRUE. //FALSE before
                        CREATE_NEW_CONSOLE, //CREATE_NO_WINDOW, // No Window for DOS-Commands. 
                        NULL,    // Use parent's environment block. 
                        NULL,    // Use parent's starting directory. 
                        &si,     // Pointer to STARTUPINFO structure.
                        &pi )    // Pointer to PROCESS_INFORMATION structure.
	  ) 
	{
		temp.Format(IDS_ERROREXECUTE,cmd);
		AfxMessageBox(temp,MB_OK|MB_ICONERROR);
	    getch();
		CloseHandle(newstdin);
		CloseHandle(newstdout);
		CloseHandle(read_stdout);
		CloseHandle(write_stdin);
		delete [] szCmd;
		return -1;
	}

	if(wait)
	{
		// Wait for process to terminate
		bzero(buf);
		do
		{
			DWORD bread,avail;

			if(!GetExitCodeProcess(pi.hProcess, &exitCode))
			{
				AfxMessageBox("Unexpected error on GetExitCodeProcess, contact Vendor.");
				// Close process and thread handles. 
				CloseHandle( pi.hProcess );
				CloseHandle( pi.hThread );
				CloseHandle(newstdin);            //clean stuff up
				CloseHandle(newstdout);
				CloseHandle(read_stdout);
				CloseHandle(write_stdin);
				delete [] szCmd;
				return -1;
			}
			if(exitCode==STILL_ACTIVE)
			{
				PeekNamedPipe(read_stdout,buf,1023,&bread,&avail,NULL);
				//check to see if there is any data to read from stdout
				if(bread != 0)
				{
					bzero(buf);
					if (avail > 1023)
					{
						while (bread >= 1023)
						{
							ReadFile(read_stdout,buf,1023,&bread,NULL);  //read the stdout pipe
							output+=buf;
							bzero(buf);
						}
					}
					else
					{
						ReadFile(read_stdout,buf,1023,&bread,NULL);
						output+=buf;
					}
				}
			}
		} while(exitCode==STILL_ACTIVE);
	}

	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	CloseHandle(newstdin);            //clean stuff up
	CloseHandle(newstdout);
	CloseHandle(read_stdout);
	CloseHandle(write_stdin);

	delete [] szCmd;

	return exitCode;
}

int CVPNConfig::GetIPSecStatus(CString &log)
{
	CString tmp;

	switch(GetOSVersion())
	{
	case OS_WIN2K:
		return CVPNConfig::ExecuteCmd("ipsecmon",log,FALSE);
		break;
	case OS_WINXP:
		tmp=GetIPSecTool();
		tmp+=" show all";
		return CVPNConfig::ExecuteCmd(tmp,log);
		break;
	}
	return -1;
}

int CVPNConfig::SendUDPEcho(const CString &destIp)
{
	// Here is a structure contains the port we'll use,
	// the protocol type and the IP address we'll communicate with.
	SOCKADDR_IN sockaddr;
	
	// This is our socket, it is the handle to the IO address to read/write packets
	SOCKET sock;
	
	// Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (!sock)
	{
		return -1;
	}
	
	//Now we'll set the sockaddr variables:
	sockaddr.sin_family = AF_INET; // Must be AF_INET
	// If this is the Server:
	sockaddr.sin_addr.s_addr = INADDR_ANY; // Means we will "answer" to all addresses.
	// IP of the client:
	sockaddr.sin_addr.s_addr = inet_addr(destIp); // IP to communicate with.
	
	// The following sets our communication port.
	// 'htons()' reverses the bytes (0x1020 would become 0x2010).
	// This metod is called Big Endian and it was first used on Unix systems, you
	// have to call it because all systems work that way
	sockaddr.sin_port = htons(7); // UDP echo
	
	// A server need to bind the socket to itself in order to receive all the packets
	// it gets from a port
	int ret = bind(sock, (SOCKADDR *)&sockaddr, sizeof(SOCKADDR));
	
	if (ret)
	{
		return -1;
	}
	
	// That's it, now let's send a message...
	char buffer[256];
	strcpy(buffer, "HELLO!!!");
	int len = sizeof(SOCKADDR);
	sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&sockaddr, sizeof(SOCKADDR));
	// Notice we use sendto() and NOT send(), because we use UDP!
	// Easy huh?? Let's receive a packet..
	
	closesocket(sock);

	return 0;
}

int CVPNConfig::GetOSVersion()
{
	OSVERSIONINFOEX osinfo;
	int retvalue;

	ZeroMemory(&osinfo, sizeof(OSVERSIONINFOEX));
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	retvalue = GetVersionEx((OSVERSIONINFO *) &osinfo);
	
	switch(osinfo.dwPlatformId)
	{
	case 1:
		switch(osinfo.dwMinorVersion)
		{
		case 0:
			//getVersion = "Windows 95"
			return OS_WIN95;
			break;
		case 10:
			//getVersion = "Windows 98"
			return OS_WIN98;
			break;
		case 90:
			//getVersion = "Windows Mellinnium"
			return OS_WINME;
			break;
		}
		break;
	case 2:
		switch(osinfo.dwMajorVersion)
		{
		case 3:
			//getVersion = "Windows NT 3.51"
			return OS_WINNT351;
			break;
		case 4:
			//getVersion = "Windows NT 4.0"
			return OS_WINNT40;
			break;
		case 5:
			if(osinfo.dwMinorVersion == 0)
				//getVersion = "Windows 2000"
				return OS_WIN2K;
			else
				//getVersion = "Windows XP"
				return OS_WINXP;
			break;
		}
	default:
		//getVersion = "Failed"
		return OS_UNKNOWN;
	} 
	return OS_UNKNOWN;

}

CString CVPNConfig::GetNetwork(const CString &ip, const CString &netmask)
{
	CString retVal;
	BYTE network1,network2,network3,network4;
	BYTE netmask1,netmask2,netmask3,netmask4;

	splitIp(ip,network1,network2,network3,network4);
	splitIp(netmask,netmask1,netmask2,netmask3,netmask4);

	network1&=netmask1;
	network2&=netmask2;
	network3&=netmask3;
	network4&=netmask4;

	joinIp(network1,network2,network3,network4,retVal);

	return retVal;
}

void CVPNConfig::splitIp(const CString &ip, BYTE &ip1, BYTE &ip2, BYTE &ip3, BYTE &ip4)
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

void CVPNConfig::joinIp(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, CString& ip)
{
	ip.Format("%i.%i.%i.%i",ip1,ip2,ip3,ip4);
}
