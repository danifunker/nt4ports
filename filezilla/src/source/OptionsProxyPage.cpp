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

// OptionsProxyPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsProxyPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite COptionsProxyPage 

IMPLEMENT_DYNCREATE(COptionsProxyPage, CSAPrefsSubDlg)

COptionsProxyPage::COptionsProxyPage() : CSAPrefsSubDlg(COptionsProxyPage::IDD)
{
	//{{AFX_DATA_INIT(COptionsProxyPage)
	m_Use = FALSE;
	m_Host = _T("");
	m_Pass = _T("");
	m_Port = _T("");
	m_User = _T("");
	m_Type = -1;
	//}}AFX_DATA_INIT
}

COptionsProxyPage::~COptionsProxyPage()
{
}

void COptionsProxyPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsProxyPage)
	DDX_Control(pDX, IDC_PROXYUSER, m_UserCtrl);
	DDX_Control(pDX, IDC_PROXYPORT, m_PortCtrl);
	DDX_Control(pDX, IDC_PROXYPASS, m_PassCtrl);
	DDX_Control(pDX, IDC_PROXYHOST, m_HostCtrl);
	DDX_Control(pDX, IDC_CHECK1, m_UseCtrl);
	DDX_Check(pDX, IDC_CHECK1, m_Use);
	DDX_Text(pDX, IDC_PROXYHOST, m_Host);
	DDX_Text(pDX, IDC_PROXYPASS, m_Pass);
	DDV_MaxChars(pDX, m_Pass, 255);
	DDX_Text(pDX, IDC_PROXYPORT, m_Port);
	DDX_Text(pDX, IDC_PROXYUSER, m_User);
	DDV_MaxChars(pDX, m_User, 255);
	DDX_Radio(pDX, IDC_RADIO1, m_Type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsProxyPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsProxyPage)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsProxyPage 

void COptionsProxyPage::OnRadio1() 
{
	SetCtrlState();
}

void COptionsProxyPage::OnRadio2() 
{
	SetCtrlState();
}

void COptionsProxyPage::OnRadio3() 
{
	SetCtrlState();
}

void COptionsProxyPage::SetCtrlState()
{
	UpdateData(TRUE);
	if (m_Type==0)
	{
		m_HostCtrl.EnableWindow(FALSE);
		m_PortCtrl.EnableWindow(FALSE);
		m_UserCtrl.EnableWindow(FALSE);
		m_PassCtrl.EnableWindow(FALSE);
		m_UseCtrl.EnableWindow(FALSE);
	}
	else
	{
		m_HostCtrl.EnableWindow(TRUE);
		m_PortCtrl.EnableWindow(TRUE);
		if (m_Type==3 || m_Type==4)
			m_UseCtrl.EnableWindow(TRUE);
		else
			m_UseCtrl.EnableWindow(FALSE);
		if (m_Use && (m_Type==3 || m_Type==4))
		{
			m_UserCtrl.EnableWindow(TRUE);
			m_PassCtrl.EnableWindow(TRUE);
		}
		else
		{
			m_UserCtrl.EnableWindow(FALSE);
			m_PassCtrl.EnableWindow(FALSE);
		}
	}	
}

BOOL COptionsProxyPage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	SetCtrlState();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void COptionsProxyPage::OnCheck1() 
{
	SetCtrlState();	
}

void COptionsProxyPage::OnRadio4() 
{
	SetCtrlState();	
}

void COptionsProxyPage::OnRadio5() 
{
	SetCtrlState();	
}
