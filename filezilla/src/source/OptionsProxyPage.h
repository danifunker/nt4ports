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

#if !defined(AFX_OPTIONSPROXYPAGE_H__2B8DB4F4_CC35_4918_8FDF_F28AB024DFA0__INCLUDED_)
#define AFX_OPTIONSPROXYPAGE_H__2B8DB4F4_CC35_4918_8FDF_F28AB024DFA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsProxyPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsProxyPage 

class COptionsProxyPage : public CSAPrefsSubDlg
{
	DECLARE_DYNCREATE(COptionsProxyPage)

// Konstruktion
public:
	COptionsProxyPage();
	~COptionsProxyPage();

// Dialogfelddaten
	//{{AFX_DATA(COptionsProxyPage)
	enum { IDD = IDD_CONNECTION4 };
	CEdit	m_UserCtrl;
	CEdit	m_PortCtrl;
	CEdit	m_PassCtrl;
	CEdit	m_HostCtrl;
	CButton	m_UseCtrl;
	BOOL	m_Use;
	CString	m_Host;
	CString	m_Pass;
	CString	m_Port;
	CString	m_User;
	int		m_Type;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsProxyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	void SetCtrlState();
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsProxyPage)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheck1();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSPROXYPAGE_H__2B8DB4F4_CC35_4918_8FDF_F28AB024DFA0__INCLUDED_
