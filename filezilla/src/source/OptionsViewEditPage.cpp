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

// OptionsViewEditPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsViewEditPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsViewEditPage 


COptionsViewEditPage::COptionsViewEditPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsViewEditPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsViewEditPage)
	m_Custom = _T("");
	m_Default = _T("");
	//}}AFX_DATA_INIT
}


void COptionsViewEditPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsViewEditPage)
	DDX_Text(pDX, IDC_OPTIONS_VIEWEDIT_CUSTOM, m_Custom);
	DDX_Text(pDX, IDC_OPTIONS_VIEWEDIT_DEFAULT, m_Default);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsViewEditPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsViewEditPage)
	ON_BN_CLICKED(IDC_OPTIONS_VIEWEDIT_DEFAULT_BROWSE, OnOptionsVieweditDefaultBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsViewEditPage 

BOOL COptionsViewEditPage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	//Reformat and file associations so that they fill the list box poperly
	CString ext;
	CString prog;
	m_Custom="";
	BOOL bDoExt=TRUE;
	while (m_Custom2!="")
	{
		int pos=m_Custom2.Find( _T(";") );
		if (bDoExt)
		{
			if (!pos || pos==-1 || pos==m_Custom2.GetLength()-1)
				break;
			ext+=m_Custom2.Left(pos);
			m_Custom2=m_Custom2.Mid(pos+1);
			if (m_Custom2.Left(1)== _T(" "))
			{
				ext+=_T(";");
				m_Custom2=m_Custom2.Mid(1);
			}
			else
				bDoExt=FALSE;
		}
		else
		{
			if (!pos || pos==m_Custom2.GetLength()-1)
				break;
			if (pos!=-1)
			{
				prog+=m_Custom2.Left(pos);
				m_Custom2=m_Custom2.Mid(pos+1);
			}
			else
			{
				prog=m_Custom2;
				m_Custom2="";
			}
			if (m_Custom2.Left(1)== _T(" "))
			{
				prog+=_T(";");
				m_Custom2=m_Custom2.Mid(1);
				if (m_Custom2!="")
					continue;
			}
			
			if (ext.Find( _T(" ") )!=-1)
				m_Custom+=_T("\"") + ext + _T("\" ");
			else
				m_Custom+=ext + _T(" ");
			m_Custom+=prog + _T("\r\n");
			ext="";
			prog="";
			bDoExt=TRUE;
		}
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void COptionsViewEditPage::OnOptionsVieweditDefaultBrowse() 
{
	UpdateData(TRUE);
	
	CFileDialog dlg(TRUE, 0, 0, OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("All files|*.*||"), this);
	if (dlg.DoModal()==IDOK)
	{
		m_Default=dlg.GetPathName();
		UpdateData(FALSE);
	}
}
