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

#if !defined(AFX_OPTIONSGSSPAGE_H__23C7AF6F_0448_4039_903B_B9CE80789A7F__INCLUDED_)
#define AFX_OPTIONSGSSPAGE_H__23C7AF6F_0448_4039_903B_B9CE80789A7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsGssPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsGssPage 

class COptionsGssPage : public CSAPrefsSubDlg
{
// Konstruktion
public:
	CString m_GssServers;
	COptionsGssPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(COptionsGssPage)
	enum { IDD = IDD_OPTIONS_GSSPAGE };
	CButton	m_cServerRemove;
	CListBox	m_cServerList;
	CButton	m_cServerAdd;
	CEdit	m_cServerName;
	CString	m_ServerName;
	BOOL	m_bUseGSS;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsGssPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsGssPage)
	afx_msg void OnUsegss();
	afx_msg void OnServerremove();
	afx_msg void OnServeradd();
	afx_msg void OnChangeGssservername();
	afx_msg void OnSelchangeServerlist();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSGSSPAGE_H__23C7AF6F_0448_4039_903B_B9CE80789A7F__INCLUDED_
