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
// VPNSession.h: interface for the CVPNSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VPNSESSION_H__9FB7F051_4656_43DC_B2DB_71015456C1CB__INCLUDED_)
#define AFX_VPNSESSION_H__9FB7F051_4656_43DC_B2DB_71015456C1CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VPN_AUTH_PSK 0
#define VPN_AUTH_CERTIFICATE 1

#define VPN_IPSECENC_NONE 0
#define VPN_IPSECENC_DES  1
#define VPN_IPSECENC_3DES 2

#define VPN_IPSECAUTH_NONE 0
#define VPN_IPSECAUTH_MD5  1
#define VPN_IPSECAUTH_SHA  2

class CVPNSession  
{
public:
	BOOL m_allowLocalTraffic;
	BOOL m_blockNonIPSec;
	CString GetFirstVPNIP();
	CVPNSession();
	virtual ~CVPNSession();

	CString m_name;
	CString	m_gateway;
	CString	m_network;
	CString	m_netmask;
	CString	m_ras;
	CString	m_l2tp;
	CString m_authValue;
	int     m_authType;        // 0: PSK  ; 1: Certificate
	int		m_ipsecMode;       // 0: ESP  ; 1: AH
	int		m_ipsecEncryption; // 0: none ; 1: des ; 2: 3des
	int		m_ipsecAuth;       // 0: none ; 1: md5 ; 2: sha (sha1)
	BOOL	m_ipsecPFS;
	int		m_ipsecRekeyData;
	int		m_ipsecRekeyTime;

protected:
	void splitIp(const CString& ip, BYTE& ip1, BYTE& ip2, BYTE& ip3, BYTE& ip4);
	void joinIp(BYTE ip1, BYTE ip2, BYTE ip3, BYTE ip4, CString& ip);



};

#endif // !defined(AFX_VPNSESSION_H__9FB7F051_4656_43DC_B2DB_71015456C1CB__INCLUDED_)
