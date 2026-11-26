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

#if !defined(AFX_MANUALTRANSFERDLG_H__4C56F6FE_6E62_455D_BEC0_A2716FCCE7F4__INCLUDED_)
#define AFX_MANUALTRANSFERDLG_H__4C56F6FE_6E62_455D_BEC0_A2716FCCE7F4__INCLUDED_

#include "FileZillaApi.h"	// Hinzugefügt von der Klassenansicht
#include "structures.h"	// Hinzugefügt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManualTransferDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CManualTransferDlg 

class CManualTransferDlg : protected CDialog
{
// Konstruktion
public:
	int Show(t_transferfile &transferfile, t_server *pServer, CString LocalPath, CServerPath RemotePath);
	CManualTransferDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CManualTransferDlg)
	enum { IDD = IDD_MANUALTRANSFER };
	CComboBox	m_ServerType;
	BOOL	m_bBypassProxy;
	CString	m_Host;
	CString	m_LocalFile;
	int		m_nLogontype;
	CString	m_Pass;
	CString	m_Port;
	CString	m_RemoteFile;
	CString	m_User;
	int		m_nServer;
	int		m_nUpDown;
	BOOL	m_bStartNow;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CManualTransferDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	void SetCtrlState();
	t_transferfile m_Transferfile;
	CServerPath m_RemotePath;
	CString m_LocalPath;
	t_server *m_pServer;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CManualTransferDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnManualtransferServer();
	afx_msg void OnManualtransferLogontype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_MANUALTRANSFERDLG_H__4C56F6FE_6E62_455D_BEC0_A2716FCCE7F4__INCLUDED_
