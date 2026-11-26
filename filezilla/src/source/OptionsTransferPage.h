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

#if !defined(AFX_OPTIONSTRANSFERPAGE_H__F72C2169_58AA_4119_B433_C39F3DF8F944__INCLUDED_)
#define AFX_OPTIONSTRANSFERPAGE_H__F72C2169_58AA_4119_B433_C39F3DF8F944__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsTransferPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsTransferPage 

class COptionsTransferPage : public CSAPrefsSubDlg
{
// Konstruktion
public:
	COptionsTransferPage(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(COptionsTransferPage)
	enum { IDD = IDD_OPTIONS_TRANSFER };
	CEdit	m_cMaxConnCount;
	BOOL	m_bPreserveTime;
	CString	m_MaxPrimarySize;
	CString	m_MaxConnCount;
	BOOL	m_bUseMultiple;
	int		m_nFileExistsAction;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COptionsTransferPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsTransferPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnOptionsTransferUsemultiple();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_vmsall;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OPTIONSTRANSFERPAGE_H__F72C2169_58AA_4119_B433_C39F3DF8F944__INCLUDED_
