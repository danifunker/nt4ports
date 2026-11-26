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

#pragma once

// COptionsLoggingPage-Dialogfeld

class COptionsLoggingPage : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(COptionsLoggingPage)

public:
	COptionsLoggingPage(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~COptionsLoggingPage();

// Dialogfelddaten
	enum { IDD = IDD_OPTIONS_LOGGING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	void DisplayFontName();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChangefont();
	afx_msg void OnUseCustomFont();
	afx_msg void OnLogtofile();
	CString m_LogFile;
	BOOL m_bLogToFile;
	virtual BOOL OnInitDialog();
	BOOL m_bUseCustomFont;
	CString m_FontName;
	int m_nFontSize;
	int m_timestamps;
};
