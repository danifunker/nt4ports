// FileZilla - a Windows ftp client

// Copyright (C) 2004 - Tim Kosse <tim.kosse@gmx.de>

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
#include "OptionsTransferCompressionPage.h"

IMPLEMENT_DYNCREATE(COptionsTransferCompressionPage, CSAPrefsSubDlg)

COptionsTransferCompressionPage::COptionsTransferCompressionPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsTransferCompressionPage::IDD, pParent)
{
}

COptionsTransferCompressionPage::~COptionsTransferCompressionPage()
{
}

void COptionsTransferCompressionPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_OPTIONS_TRANSFER_COMPRESSION_LEVEL, m_level);
	DDX_Radio(pDX, IDC_OPTIONS_TRANSFER_COMPRESSION_USECOMPRESSION, m_useCompression);
}


BEGIN_MESSAGE_MAP(COptionsTransferCompressionPage, CSAPrefsSubDlg)
END_MESSAGE_MAP()