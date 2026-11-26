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

// OptionsGssPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsGssPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsGssPage 


COptionsGssPage::COptionsGssPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsGssPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsGssPage)
	m_ServerName = _T("");
	m_bUseGSS = FALSE;
	//}}AFX_DATA_INIT
}


void COptionsGssPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsGssPage)
	DDX_Control(pDX, IDC_SERVERREMOVE, m_cServerRemove);
	DDX_Control(pDX, IDC_SERVERLIST, m_cServerList);
	DDX_Control(pDX, IDC_SERVERADD, m_cServerAdd);
	DDX_Control(pDX, IDC_GSSSERVERNAME, m_cServerName);
	DDX_Text(pDX, IDC_GSSSERVERNAME, m_ServerName);
	DDX_Check(pDX, IDC_USEGSS, m_bUseGSS);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsGssPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsGssPage)
	ON_BN_CLICKED(IDC_USEGSS, OnUsegss)
	ON_BN_CLICKED(IDC_SERVERREMOVE, OnServerremove)
	ON_BN_CLICKED(IDC_SERVERADD, OnServeradd)
	ON_EN_CHANGE(IDC_GSSSERVERNAME, OnChangeGssservername)
	ON_LBN_SELCHANGE(IDC_SERVERLIST, OnSelchangeServerlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsGssPage 

void COptionsGssPage::OnUsegss() 
{
	UpdateData(TRUE);
	m_cServerList.EnableWindow(m_bUseGSS);
	m_cServerName.EnableWindow(m_bUseGSS);
	OnChangeGssservername();
	OnSelchangeServerlist();	
}

void COptionsGssPage::OnServerremove() 
{
	if (m_cServerList.GetCurSel()==LB_ERR)
		return;
	m_cServerList.DeleteString(m_cServerList.GetCurSel());	
	m_cServerRemove.EnableWindow(FALSE);	
}

void COptionsGssPage::OnServeradd() 
{
	UpdateData(TRUE);
	m_ServerName.MakeLower();
	if (m_ServerName!="")
		m_cServerList.AddString(m_ServerName);
	m_ServerName="";
	m_cServerAdd.EnableWindow(FALSE);
	m_cServerRemove.EnableWindow(FALSE);
	UpdateData(FALSE);	
}

void COptionsGssPage::OnChangeGssservername() 
{
	UpdateData(TRUE);
	m_cServerAdd.EnableWindow(m_ServerName!="" && m_cServerList.FindStringExact(-1, m_ServerName)==LB_ERR && m_bUseGSS);	
}

void COptionsGssPage::OnSelchangeServerlist() 
{
	UpdateData(TRUE);
	m_cServerRemove.EnableWindow(m_cServerList.GetCurSel()!=LB_ERR && m_bUseGSS);	
}

BOOL COptionsGssPage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	m_cServerList.ResetContent();
	int i;
	
	while ((i=m_GssServers.Find( _T(";") ))!=-1 && i)
	{
		m_cServerList.AddString(m_GssServers.Left(i));
		m_GssServers=m_GssServers.Mid(i+1);
	}
	m_GssServers="";
	
	OnUsegss();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
