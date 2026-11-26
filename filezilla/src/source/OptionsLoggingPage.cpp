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

#include "stdafx.h"
#include "FileZilla.h"
#include "OptionsLoggingPage.h"
#include ".\optionsloggingpage.h"


// COptionsLoggingPage-Dialogfeld

IMPLEMENT_DYNAMIC(COptionsLoggingPage, CSAPrefsSubDlg)
COptionsLoggingPage::COptionsLoggingPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsLoggingPage::IDD, pParent)
	, m_LogFile(_T(""))
	, m_bLogToFile(FALSE)
	, m_bUseCustomFont(FALSE)
{
	m_nFontSize = 0;
}

COptionsLoggingPage::~COptionsLoggingPage()
{
}

void COptionsLoggingPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_OPTIONS_LOGGING_FILENAME, m_LogFile);
	DDX_Check(pDX, IDC_OPTIONS_LOGGING_LOGTOFILE, m_bLogToFile);
	DDX_Check(pDX, IDC_OPTIONS_LOGGING_USECUSTOMFONT, m_bUseCustomFont);
	DDX_Check(pDX, IDC_OPTIONS_LOGGING_TIMESTAMP, m_timestamps);
}


BEGIN_MESSAGE_MAP(COptionsLoggingPage, CSAPrefsSubDlg)
	ON_BN_CLICKED(IDC_OPTIONS_LOGGING_CHANGEFONT, OnChangefont)
	ON_BN_CLICKED(IDC_OPTIONS_LOGGING_USECUSTOMFONT, OnUseCustomFont)
	ON_BN_CLICKED(IDC_OPTIONS_LOGGING_LOGTOFILE, OnLogtofile)
END_MESSAGE_MAP()


// COptionsLoggingPage-Meldungshandler

void COptionsLoggingPage::OnChangefont()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	// Initialize with default font
	HFONT hSysFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	CFont* pFont = CFont::FromHandle( hSysFont );
	pFont->GetLogFont( &lf );

	if (m_nFontSize != 0)
	{
		CDC *pDC = GetDC();
		lf.lfHeight = -((m_nFontSize * GetDeviceCaps(pDC->GetSafeHdc(), LOGPIXELSY)) / 72);
		ReleaseDC(pDC);	
	}
	if (m_FontName != "")
		_tcscpy(lf.lfFaceName, m_FontName);
	
	CFontDialog dlg(&lf, CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT);	

	int res = dlg.DoModal();
	if (res == IDOK)
	{
		m_FontName = dlg.GetFaceName();
		m_nFontSize = dlg.GetSize() / 10;
		DisplayFontName();
	}
}

void COptionsLoggingPage::OnUseCustomFont()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_OPTIONS_LOGGING_CHANGEFONT)->EnableWindow(m_bUseCustomFont);
}

void COptionsLoggingPage::OnLogtofile()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_OPTIONS_LOGGING_FILENAME)->EnableWindow(m_bLogToFile);	
}

BOOL COptionsLoggingPage::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	GetDlgItem(IDC_OPTIONS_LOGGING_FILENAME)->EnableWindow(m_bLogToFile);		
	GetDlgItem(IDC_OPTIONS_LOGGING_CHANGEFONT)->EnableWindow(m_bUseCustomFont);

	DisplayFontName();

	return TRUE;
}

void COptionsLoggingPage::DisplayFontName()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	// Initialize with default font
	HFONT hSysFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	CFont* pFont = CFont::FromHandle( hSysFont );
	pFont->GetLogFont( &lf );

	CDC *pDC = GetDC();
	if (m_nFontSize == 0)
		m_nFontSize = -lf.lfHeight * 72 / GetDeviceCaps(pDC->GetSafeHdc(), LOGPIXELSY);
	if (m_FontName == "")
		m_FontName = lf.lfFaceName;

	CString name;
	name.Format(_T("%s, %dpt"), m_FontName, m_nFontSize);
	GetDlgItem(IDC_OPTIONS_LOGGING_FONTDATA)->SetWindowText(name);
	ReleaseDC(pDC);
}