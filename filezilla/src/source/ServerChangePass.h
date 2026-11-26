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

#if !defined(AFX_SERVERCHANGEPASS_H__06F6E33C_FA8F_4082_8128_F458B8EF5C55__INCLUDED_)
#define AFX_SERVERCHANGEPASS_H__06F6E33C_FA8F_4082_8128_F458B8EF5C55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerChangePass.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CServerChangePass 

class CServerChangePass : public CDialog
{
// Konstruktion
public:
	CServerChangePass(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CServerChangePass)
	enum { IDD = IDD_SERVER_CHANGEPASS };
	CButton	m_cOK;
	CString	m_New2;
	CString	m_Old;
	CString	m_New;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CServerChangePass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CServerChangePass)
	afx_msg void OnChangeServerChangepassNew();
	afx_msg void OnChangeServerChangepassNew2();
	afx_msg void OnChangeServerChangepassOld();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SERVERCHANGEPASS_H__06F6E33C_FA8F_4082_8128_F458B8EF5C55__INCLUDED_
