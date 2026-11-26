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

#if !defined(AFX_OPTIONSCONNECTION_H__5C90127B_DC71_4A48_AB83_D487FE47F885__INCLUDED_)
#define AFX_OPTIONSCONNECTION_H__5C90127B_DC71_4A48_AB83_D487FE47F885__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsConnection.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsConnection 
#include "resource.h"

class COptionsConnection : public CSAPrefsSubDlg
{
	DECLARE_DYNCREATE(COptionsConnection)

// Konstruktion
public:
	COptionsConnection();
	~COptionsConnection();

// Dialogfelddaten
	//{{AFX_DATA(COptionsConnection)
	enum { IDD = IDD_OPTIONS_FTPPROXY };
	CEdit	m_cFwPass;
	CEdit	m_cFwUser;
	CEdit	m_cFwPort;
	CEdit	m_cFwHost;
	CString	m_fwhost;
	CString	m_fwuser;
	CString	m_fwpass;
	int		m_logontype;
	int		m_fwport;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsConnection)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsConnection)
	afx_msg void OnRadio();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSCONNECTION_H__5C90127B_DC71_4A48_AB83_D487FE47F885__INCLUDED_
