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
#include "filezilla.h"
#include "OptionsConnection2.h"

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite COptionsConnection2

IMPLEMENT_DYNCREATE(COptionsConnection2, CSAPrefsSubDlg)

COptionsConnection2::COptionsConnection2() : CSAPrefsSubDlg(COptionsConnection2::IDD)
, m_bEnableIPV6(FALSE)
{
	//{{AFX_DATA_INIT(COptionsConnection2)
	m_UseKeepAlive = FALSE;
	m_IntervalLow = _T("");
	m_IntervalHigh = _T("");
	m_Timeout = 0;
	m_Delay = _T("");
	m_NumRetries = _T("");
	//}}AFX_DATA_INIT
}

COptionsConnection2::~COptionsConnection2()
{
}

void COptionsConnection2::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsConnection2)
	DDX_Control(pDX, IDC_ANONPWD, m_AnonPwdCtrl);
	DDX_Text(pDX, IDC_ANONPWD, m_AnonPwd);

	DDX_Check(pDX, IDC_CHECK1, m_UseKeepAlive);

	DDX_Control(pDX, IDC_INTERVALLOW, m_IntervalLowCtrl);
	DDX_Text(pDX, IDC_INTERVALLOW, m_IntervalLow);

	DDX_Control(pDX, IDC_INTERVALHIGH, m_IntervalHighCtrl);
	DDX_Text(pDX, IDC_INTERVALHIGH, m_IntervalHigh);

	DDX_Text(pDX, IDC_TIMEOUT, m_Timeout);

	DDX_Text(pDX, IDC_DELAY, m_Delay);
	DDX_Text(pDX, IDC_NUMRETRY, m_NumRetries);
	//}}AFX_DATA_MAP

	DDX_Check(pDX, IDC_OPTIONS_CONNECTION2_ENABLEIPV6, m_bEnableIPV6);
}

BEGIN_MESSAGE_MAP(COptionsConnection2, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsConnection2)
	ON_BN_CLICKED(IDC_CHECK1, OnCheckKeepAlive)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL COptionsConnection2::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	m_IntervalHighCtrl.EnableWindow(m_UseKeepAlive);
	m_IntervalLowCtrl.EnableWindow(m_UseKeepAlive);

	// Return TRUE unless you set the focus to a control.
	return TRUE;
}

void COptionsConnection2::OnCheckKeepAlive()
{
	UpdateData(TRUE);
	m_IntervalLowCtrl.EnableWindow(m_UseKeepAlive);
	m_IntervalHighCtrl.EnableWindow(m_UseKeepAlive);
}
