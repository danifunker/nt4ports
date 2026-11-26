// FileZilla - a Windows ftp client

// Copyright (C) 2002-2004 - Tim Kosse <tim.kosse@gmx.de>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#if !defined(AFX_OPTIONSFIREWALLPAGE_H__FAEF3F03_6566_49A3_8083_F1890FC546D6__INCLUDED_)
#define AFX_OPTIONSFIREWALLPAGE_H__FAEF3F03_6566_49A3_8083_F1890FC546D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsFirewallPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsFirewallPage 

class COptionsFirewallPage : public CSAPrefsSubDlg
{
// Konstruktion
public:
	COptionsFirewallPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(COptionsFirewallPage)
	enum { IDD = IDD_OPTIONS_FIREWALL };
	CEdit	m_cTransferIP;
	CEdit	m_cTransferIP6;
	CEdit	m_cPortRangeLow;
	CEdit	m_cPortRangeHigh;
	BOOL	m_bPasv;
	BOOL	m_bLimitPortRange;
	CString	m_PortRangeHigh;
	CString	m_PortRangeLow;
	CString	m_TransferIP;
	BOOL	m_bUseTransferIP;
	CString	m_TransferIP6;
	BOOL	m_bUseTransferIP6;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsFirewallPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsFirewallPage)
	afx_msg void OnOptionsFirewallLimitports();
	virtual BOOL OnInitDialog();
	afx_msg void OnOptionsFirewallUseTransferIP();
	afx_msg void OnOptionsFirewallUseTransferIP6();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSFIREWALLPAGE_H__FAEF3F03_6566_49A3_8083_F1890FC546D6__INCLUDED_
