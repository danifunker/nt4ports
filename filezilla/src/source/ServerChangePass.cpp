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

// ServerChangePass.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "ServerChangePass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CServerChangePass 


CServerChangePass::CServerChangePass(CWnd* pParent /*=NULL*/)
	: CDialog(CServerChangePass::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServerChangePass)
	m_New2 = _T("");
	m_Old = _T("");
	m_New = _T("");
	//}}AFX_DATA_INIT
}


void CServerChangePass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerChangePass)
	DDX_Control(pDX, IDOK, m_cOK);
	DDX_Text(pDX, IDC_SERVER_CHANGEPASS_NEW2, m_New2);
	DDX_Text(pDX, IDC_SERVER_CHANGEPASS_OLD, m_Old);
	DDX_Text(pDX, IDC_SERVER_CHANGEPASS_NEW, m_New);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerChangePass, CDialog)
	//{{AFX_MSG_MAP(CServerChangePass)
	ON_EN_CHANGE(IDC_SERVER_CHANGEPASS_NEW, OnChangeServerChangepassNew)
	ON_EN_CHANGE(IDC_SERVER_CHANGEPASS_NEW2, OnChangeServerChangepassNew2)
	ON_EN_CHANGE(IDC_SERVER_CHANGEPASS_OLD, OnChangeServerChangepassOld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CServerChangePass 

void CServerChangePass::OnChangeServerChangepassNew() 
{
	UpdateData(TRUE);
	if (m_Old=="")
		m_cOK.EnableWindow(FALSE);
	else if (m_New=="")
		m_cOK.EnableWindow(FALSE);
	else if (m_New!=m_New2)
		m_cOK.EnableWindow(FALSE);
	else
		m_cOK.EnableWindow(TRUE);
}

void CServerChangePass::OnChangeServerChangepassNew2() 
{
	UpdateData(TRUE);
	if (m_Old=="")
		m_cOK.EnableWindow(FALSE);
	else if (m_New=="")
		m_cOK.EnableWindow(FALSE);
	else if (m_New!=m_New2)
		m_cOK.EnableWindow(FALSE);
	else
		m_cOK.EnableWindow(TRUE);
}

void CServerChangePass::OnChangeServerChangepassOld() 
{
	UpdateData(TRUE);
	if (m_Old=="")
		m_cOK.EnableWindow(FALSE);
	else if (m_New=="")
		m_cOK.EnableWindow(FALSE);
	else if (m_New!=m_New2)
		m_cOK.EnableWindow(FALSE);
	else
		m_cOK.EnableWindow(TRUE);
}

BOOL CServerChangePass::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_cOK.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
