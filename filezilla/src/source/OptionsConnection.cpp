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

// OptionsConnection.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite COptionsConnection 

IMPLEMENT_DYNCREATE(COptionsConnection, CSAPrefsSubDlg)

COptionsConnection::COptionsConnection() : CSAPrefsSubDlg(COptionsConnection::IDD)
{
	//{{AFX_DATA_INIT(COptionsConnection)
	m_fwhost = _T("");
	m_fwuser = _T("");
	m_fwpass = _T("");
	m_logontype = -1;
	m_fwport = 0;
	//}}AFX_DATA_INIT
}

COptionsConnection::~COptionsConnection()
{
}

void COptionsConnection::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsConnection)
	DDX_Control(pDX, IDC_EDIT8, m_cFwPass);
	DDX_Control(pDX, IDC_EDIT7, m_cFwUser);
	DDX_Control(pDX, IDC_EDIT6, m_cFwPort);
	DDX_Control(pDX, IDC_EDIT5, m_cFwHost);
	DDX_Text(pDX, IDC_EDIT5, m_fwhost);
	DDX_Text(pDX, IDC_EDIT7, m_fwuser);
	DDX_Text(pDX, IDC_EDIT8, m_fwpass);
	DDX_Radio(pDX, IDC_RADIO3, m_logontype);
	DDX_Text(pDX, IDC_EDIT6, m_fwport);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsConnection, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsConnection)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio)
	ON_BN_CLICKED(IDC_RADIO6, OnRadio)
	ON_BN_CLICKED(IDC_RADIO7, OnRadio)
	ON_BN_CLICKED(IDC_RADIO8, OnRadio)
	ON_BN_CLICKED(IDC_RADIO9, OnRadio)
	ON_BN_CLICKED(IDC_RADIO10, OnRadio)
	ON_BN_CLICKED(IDC_RADIO11, OnRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsConnection 

void COptionsConnection::OnRadio() 
{
	UpdateData(TRUE);
	m_cFwHost.EnableWindow(m_logontype);
	m_cFwPort.EnableWindow(m_logontype);
	m_cFwUser.EnableWindow(m_logontype);
	m_cFwPass.EnableWindow(m_logontype);
	
}

BOOL COptionsConnection::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	OnRadio();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
