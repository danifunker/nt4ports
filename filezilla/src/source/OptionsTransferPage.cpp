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

// OptionsTransferPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsTransferPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsTransferPage 


COptionsTransferPage::COptionsTransferPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsTransferPage::IDD, pParent)
	, m_vmsall(FALSE)
{
	//{{AFX_DATA_INIT(COptionsTransferPage)
	m_bPreserveTime = FALSE;
	m_MaxPrimarySize = _T("");
	m_MaxConnCount = _T("");
	m_bUseMultiple = FALSE;
	m_nFileExistsAction = -1;
	//}}AFX_DATA_INIT
}


void COptionsTransferPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsTransferPage)
	DDX_Control(pDX, IDC_OPTIONS_TRANSFER_MAXTRANSFERCONNECTIONS, m_cMaxConnCount);
	DDX_Check(pDX, IDC_OPTIONS_TRANSFER_PRESERVEDATETIME, m_bPreserveTime);
	DDX_Text(pDX, IDC_OPTIONS_TRANSFER_MAXPRIMARYSIZE, m_MaxPrimarySize);
	DDV_MaxChars(pDX, m_MaxPrimarySize, 6);
	DDX_Text(pDX, IDC_OPTIONS_TRANSFER_MAXTRANSFERCONNECTIONS, m_MaxConnCount);
	DDV_MaxChars(pDX, m_MaxConnCount, 2);
	DDX_Check(pDX, IDC_OPTIONS_TRANSFER_USEMULTIPLE, m_bUseMultiple);
	DDX_Radio(pDX, IDC_OPTIONS_TRANSFER_FILEEXISTS_RADIO1, m_nFileExistsAction);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_OPTIONS_TRANSFER_VMSALLREVISIONS, m_vmsall);
}


BEGIN_MESSAGE_MAP(COptionsTransferPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsTransferPage)
	ON_BN_CLICKED(IDC_OPTIONS_TRANSFER_USEMULTIPLE, OnOptionsTransferUsemultiple)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsTransferPage 

BOOL COptionsTransferPage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	m_cMaxConnCount.EnableWindow(m_bUseMultiple);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void COptionsTransferPage::OnOptionsTransferUsemultiple() 
{
	UpdateData(TRUE);
	m_cMaxConnCount.EnableWindow(m_bUseMultiple);
}
