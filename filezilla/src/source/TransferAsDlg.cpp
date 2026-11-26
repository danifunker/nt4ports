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

// TransferAsDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "TransferAsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTransferAsDlg 


CTransferAsDlg::CTransferAsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTransferAsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransferAsDlg)
	m_Pass = _T("");
	m_bTransferNow = FALSE;
	m_User = _T("");
	//}}AFX_DATA_INIT
}


void CTransferAsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransferAsDlg)
	DDX_Control(pDX, IDOK, m_OkCtrl);
	DDX_Text(pDX, IDC_PASS, m_Pass);
	DDX_Check(pDX, IDC_TRANSFERNOW, m_bTransferNow);
	DDX_Text(pDX, IDC_USER, m_User);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransferAsDlg, CDialog)
	//{{AFX_MSG_MAP(CTransferAsDlg)
	ON_EN_CHANGE(IDC_USER, OnChangeUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTransferAsDlg 

void CTransferAsDlg::OnChangeUser() 
{
	UpdateData(TRUE);
	m_OkCtrl.EnableWindow(m_User!="");
}

BOOL CTransferAsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_OkCtrl.EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
