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

// FileAttributes.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "FileAttributes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFileAttributes 


CFileAttributes::CFileAttributes(CWnd* pParent /*=NULL*/)
	: CDialog(CFileAttributes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileAttributes)
	m_Numeric = _T("");
	//}}AFX_DATA_INIT

	int m_nFileCount = 1;
	m_permbuffer[9] = 0;
}


void CFileAttributes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileAttributes)
	DDX_Control(pDX, IDC_PUBLICWRITE, m_cPublicWrite);
	DDX_Control(pDX, IDC_PUBLICREAD, m_cPublicRead);
	DDX_Control(pDX, IDC_PUBLICEXECUTE, m_cPublicExecute);
	DDX_Control(pDX, IDC_OWNERWRITE, m_cOwnerWrite);
	DDX_Control(pDX, IDC_OWNERREAD, m_cOwnerRead);
	DDX_Control(pDX, IDC_OWNEREXECUTE, m_cOwnerExecute);
	DDX_Control(pDX, IDC_GROUPWRITE, m_cGroupWrite);
	DDX_Control(pDX, IDC_GROUPREAD, m_cGroupRead);
	DDX_Control(pDX, IDC_GROUPEXECUTE, m_cGroupExecute);
	DDX_Control(pDX, IDC_TEXT, m_TextCtrl);
	DDX_Text(pDX, IDC_NUMERIC, m_Numeric);
	DDV_MaxChars(pDX, m_Numeric, 4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileAttributes, CDialog)
	//{{AFX_MSG_MAP(CFileAttributes)
	ON_BN_CLICKED(IDC_GROUPEXECUTE, OnGroupexecute)
	ON_BN_CLICKED(IDC_OWNEREXECUTE, OnOwnerexecute)
	ON_BN_CLICKED(IDC_OWNERREAD, OnOwnerread)
	ON_BN_CLICKED(IDC_OWNERWRITE, OnOwnerwrite)
	ON_BN_CLICKED(IDC_PUBLICEXECUTE, OnPublicexecute)
	ON_BN_CLICKED(IDC_PUBLICREAD, OnPublicread)
	ON_BN_CLICKED(IDC_PUBLICWRITE, OnPublicwrite)
	ON_BN_CLICKED(IDC_GROUPREAD, OnGroupread)
	ON_BN_CLICKED(IDC_GROUPWRITE, OnGroupwrite)
	ON_EN_CHANGE(IDC_NUMERIC, OnChangeNumeric)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFileAttributes 

BOOL CFileAttributes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_TextCtrl.SetWindowText(m_text);

	if (m_nFileCount == 1)
	{
		m_cGroupRead.SetButtonStyle(BS_AUTOCHECKBOX);
		m_cGroupWrite.SetButtonStyle(BS_AUTOCHECKBOX);
		m_cGroupExecute.SetButtonStyle(BS_AUTOCHECKBOX);
		m_cOwnerRead.SetButtonStyle(BS_AUTOCHECKBOX);
		m_cOwnerWrite.SetButtonStyle(BS_AUTOCHECKBOX);
		m_cOwnerExecute.SetButtonStyle(BS_AUTOCHECKBOX);
		m_cPublicRead.SetButtonStyle(BS_AUTOCHECKBOX);
		m_cPublicWrite.SetButtonStyle(BS_AUTOCHECKBOX);
		m_cPublicExecute.SetButtonStyle(BS_AUTOCHECKBOX);
	}
	else
	{
		m_cGroupRead.SetButtonStyle(BS_AUTO3STATE);
		m_cGroupWrite.SetButtonStyle(BS_AUTO3STATE);
		m_cGroupExecute.SetButtonStyle(BS_AUTO3STATE);
		m_cOwnerRead.SetButtonStyle(BS_AUTO3STATE);
		m_cOwnerWrite.SetButtonStyle(BS_AUTO3STATE);
		m_cOwnerExecute.SetButtonStyle(BS_AUTO3STATE);
		m_cPublicRead.SetButtonStyle(BS_AUTO3STATE);
		m_cPublicWrite.SetButtonStyle(BS_AUTO3STATE);
		m_cPublicExecute.SetButtonStyle(BS_AUTO3STATE);
	}

	m_cOwnerRead.SetCheck(m_permbuffer[0]);
	m_cOwnerWrite.SetCheck(m_permbuffer[1]);
	m_cOwnerExecute.SetCheck(m_permbuffer[2]);
	m_cGroupRead.SetCheck(m_permbuffer[3]);
	m_cGroupWrite.SetCheck(m_permbuffer[4]);
	m_cGroupExecute.SetCheck(m_permbuffer[5]);
	m_cPublicRead.SetCheck(m_permbuffer[6]);
	m_cPublicWrite.SetCheck(m_permbuffer[7]);
	m_cPublicExecute.SetCheck(m_permbuffer[8]);
	UpdateNumeric();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CFileAttributes::OnOK() 
{
	UpdateData(TRUE);

	m_permbuffer[0] = m_cOwnerRead.GetCheck();
	m_permbuffer[1] = m_cOwnerWrite.GetCheck();
	m_permbuffer[2] = m_cOwnerExecute.GetCheck();
	m_permbuffer[3] = m_cGroupRead.GetCheck();
	m_permbuffer[4] = m_cGroupWrite.GetCheck();
	m_permbuffer[5] = m_cGroupExecute.GetCheck();
	m_permbuffer[6] = m_cPublicRead.GetCheck();
	m_permbuffer[7] = m_cPublicWrite.GetCheck();
	m_permbuffer[8] = m_cPublicExecute.GetCheck();
	if (m_Numeric.GetLength() == 4)
		m_permbuffer[9] = m_Numeric[0] - '0';
	else
		m_permbuffer[9] = 0;
	
	CDialog::OnOK();
}

void CFileAttributes::OnOwnerread() 
{
	UpdateNumeric();
}

void CFileAttributes::OnOwnerwrite() 
{
	UpdateNumeric();
}

void CFileAttributes::OnOwnerexecute() 
{
	UpdateNumeric();
}

void CFileAttributes::OnGroupread() 
{
	UpdateNumeric();
}

void CFileAttributes::OnGroupwrite() 
{
	UpdateNumeric();
}

void CFileAttributes::OnGroupexecute() 
{
	UpdateNumeric();
}

void CFileAttributes::OnPublicread() 
{
	UpdateNumeric();
}

void CFileAttributes::OnPublicwrite() 
{
	UpdateNumeric();
}

void CFileAttributes::OnPublicexecute() 
{
	UpdateNumeric();	
}

void CFileAttributes::UpdateNumeric()
{
	UpdateData(TRUE);
	CString str;
	CString numeric;
	if (m_cOwnerRead.GetCheck()==2 || m_cOwnerWrite.GetCheck()==2 || m_cOwnerExecute.GetCheck()==2)
		numeric += "x";
	else
	{
		int value = m_cOwnerRead.GetCheck()*4+m_cOwnerWrite.GetCheck()*2+m_cOwnerExecute.GetCheck()*1;
			str.Format(_T("%d"),value);
		numeric += str;
	}
	if (m_cGroupRead.GetCheck()==2 || m_cGroupWrite.GetCheck()==2 || m_cGroupExecute.GetCheck()==2)
		numeric+="x";
	else
	{
		int value=m_cGroupRead.GetCheck()*4+m_cGroupWrite.GetCheck()*2+m_cGroupExecute.GetCheck()*1;
			str.Format(_T("%d"),value);
		numeric+=str;
	}
	if (m_cPublicRead.GetCheck()==2 || m_cPublicWrite.GetCheck()==2 || m_cPublicExecute.GetCheck()==2)
		numeric+="x";
	else
	{
		int value=m_cPublicRead.GetCheck()*4+m_cPublicWrite.GetCheck()*2+m_cPublicExecute.GetCheck()*1;
			str.Format(_T("%d"),value);
		numeric+=str;
	}
	if (m_Numeric.GetLength() == 4)
		m_Numeric = m_Numeric[0] + numeric;
	else
		m_Numeric=numeric;
	UpdateData(FALSE);	
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CFileAttributes::OnChangeNumeric() 
{
	UpdateData(TRUE);
	if (m_Numeric.GetLength()<3)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		return;
	}
	int offset = m_Numeric.GetLength() - 3;
	if ((m_Numeric[offset + 0]!='x' && m_Numeric[offset + 0]<'0' && m_Numeric[offset + 0]>'7') ||
		(m_Numeric[offset + 1]!='x' && m_Numeric[offset + 1]<'0' && m_Numeric[offset + 1]>'7') ||
		(m_Numeric[offset + 2]!='x' && m_Numeric[offset + 2]<'0' && m_Numeric[offset + 2]>'7') )
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		return;
	}
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	if (m_Numeric[offset + 0]=='x')
	{
		m_cOwnerRead.SetCheck(2);
		m_cOwnerWrite.SetCheck(2);
		m_cOwnerExecute.SetCheck(2);
	}
	else
	{
		int val=m_Numeric[offset + 0]-'0';
		m_cOwnerRead.SetCheck((val&4)?1:0);
		m_cOwnerWrite.SetCheck((val&2)?1:0);
		m_cOwnerExecute.SetCheck((val&1)?1:0);
	}
	if (m_Numeric[offset + 1]=='x')
	{
		m_cGroupRead.SetCheck(2);
		m_cGroupWrite.SetCheck(2);
		m_cGroupExecute.SetCheck(2);
	}
	else
	{
		int val=m_Numeric[offset + 1]-'0';
		m_cGroupRead.SetCheck((val&4)?1:0);
		m_cGroupWrite.SetCheck((val&2)?1:0);
		m_cGroupExecute.SetCheck((val&1)?1:0);
	}
	if (m_Numeric[offset + 2]=='x')
	{
		m_cPublicRead.SetCheck(2);
		m_cPublicWrite.SetCheck(2);
		m_cPublicExecute.SetCheck(2);
	}
	else
	{
		int val = m_Numeric[offset + 2]-'0';
		m_cPublicRead.SetCheck((val&4)?1:0);
		m_cPublicWrite.SetCheck((val&2)?1:0);
		m_cPublicExecute.SetCheck((val&1)?1:0);
	}
}

void CFileAttributes::SetFileCount(int nFileCount)
{
	ASSERT(nFileCount > 0);
	m_nFileCount = nFileCount;
}
