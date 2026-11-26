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

// OptionsTypePage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsTypePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsTypePage 


COptionsTypePage::COptionsTypePage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsTypePage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsTypePage)
	m_nTypeMode = -1;
	m_TypeName = _T("");
	//}}AFX_DATA_INIT
}


void COptionsTypePage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsTypePage)
	DDX_Control(pDX, IDC_TYPEREMOVE, m_cTypeRemove);
	DDX_Control(pDX, IDC_TYPELIST, m_cTypeList);
	DDX_Control(pDX, IDC_TYPEADD, m_cTypeAdd);
	DDX_Radio(pDX, IDC_TYPEMODE, m_nTypeMode);
	DDX_Text(pDX, IDC_TYPENAME, m_TypeName);
	DDV_MaxChars(pDX, m_TypeName, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsTypePage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsTypePage)
	ON_LBN_SELCHANGE(IDC_TYPELIST, OnSelchangeTypelist)
	ON_BN_CLICKED(IDC_TYPEADD, OnTypeadd)
	ON_BN_CLICKED(IDC_TYPEREMOVE, OnTyperemove)
	ON_EN_CHANGE(IDC_TYPENAME, OnChangeTypename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsTypePage 

void COptionsTypePage::OnSelchangeTypelist() 
{
	m_cTypeRemove.EnableWindow(m_cTypeList.GetCurSel()!=LB_ERR);
}

void COptionsTypePage::OnTypeadd() 
{
	UpdateData(TRUE);
	m_TypeName.MakeUpper();
	if (m_TypeName!="")
		m_cTypeList.AddString(m_TypeName);
	m_TypeName="";
	m_cTypeAdd.EnableWindow(FALSE);
	m_cTypeRemove.EnableWindow(FALSE);
	UpdateData(FALSE);
	
}

void COptionsTypePage::OnTyperemove() 
{
	if (m_cTypeList.GetCurSel()==LB_ERR)
		return;
	m_cTypeList.DeleteString(m_cTypeList.GetCurSel());	
	m_cTypeRemove.EnableWindow(FALSE);
	
}

BOOL COptionsTypePage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	m_cTypeList.ResetContent();
	int i=0;
	while ((i=m_AsciiFiles.Find( _T(";") ))!=-1 && i)
	{
		m_cTypeList.AddString(m_AsciiFiles.Left(i));
		m_AsciiFiles=m_AsciiFiles.Mid(i+1);
	}
	m_AsciiFiles="";
	
	m_cTypeAdd.EnableWindow(FALSE);
	if (m_cTypeList.GetCount())
		m_cTypeList.SetCurSel(0);
	else
		m_cTypeRemove.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void COptionsTypePage::OnChangeTypename() 
{
	UpdateData(TRUE);
	m_cTypeAdd.EnableWindow(m_TypeName!="" && m_cTypeList.FindStringExact(-1, m_TypeName)==LB_ERR);
}
