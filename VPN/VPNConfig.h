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
// VPNConfig.h: interface for the CVPNConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VPNCONFIG_H__4C1393AA_C6B4_4153_925B_BEBA17976F11__INCLUDED_)
#define AFX_VPNCONFIG_H__4C1393AA_C6B4_4153_925B_BEBA17976F11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VPNSession.h"

#define OS_UNKNOWN  -1
#define OS_WIN95    0
#define OS_WIN98    1
#define OS_WINME    2
#define OS_WINNT40  3
#define OS_WINNT351 4
#define OS_WIN2K    5
#define OS_WINXP    6

class CVPNConfig  
{
public:
	CString GetNetwork(const CString& ip,const CString& netmask);
	static int GetOSVersion();
	int SendUDPEcho(const CString& destIp);
	int GetIPSecStatus(CString& log);
	static int ExecuteCmd(const CString& cmd, CString& output, BOOL wait=TRUE);
	int GetLanIPs(CArray<CString,CString>& IPs, CArray<CString,CString>& netmasks);
	int RemoveIPSec();
	int ActivateIPSec(int i, const CString& myIP, CString& log);
	static CString GetIPSecTool();
	int SaveConfig();
	void splitIp(const CString &ip, BYTE &ip1, BYTE &ip2, BYTE &ip3, BYTE &ip4);
	void joinIp(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, CString& ip);

	CArray<CVPNSession,CVPNSession> m_vpnSessions;

	CVPNConfig();
	virtual ~CVPNConfig();

};

#endif // !defined(AFX_VPNCONFIG_H__4C1393AA_C6B4_4153_925B_BEBA17976F11__INCLUDED_)
