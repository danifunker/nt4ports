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

// OptionsFirewallPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsFirewallPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsFirewallPage 


COptionsFirewallPage::COptionsFirewallPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsFirewallPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsFirewallPage)
	m_bPasv = FALSE;
	m_bLimitPortRange = FALSE;
	m_PortRangeHigh = _T("");
	m_PortRangeLow = _T("");
	m_TransferIP = _T("");
	m_bUseTransferIP = FALSE;
	//}}AFX_DATA_INIT
}


void COptionsFirewallPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsFirewallPage)
	DDX_Control(pDX, IDC_OPTIONS_FIREWALL_TRANSFERIP, m_cTransferIP);
	DDX_Control(pDX, IDC_OPTIONS_FIREWALL_TRANSFERIP_IPV6, m_cTransferIP6);
	DDX_Control(pDX, IDC_OPTIONS_FIREWALL_RANGELOW, m_cPortRangeLow);
	DDX_Control(pDX, IDC_OPTIONS_FIREWALL_RANGEHIGH, m_cPortRangeHigh);
	DDX_Check(pDX, IDC_OPTIONS_FIREWALL_PASV, m_bPasv);
	DDX_Check(pDX, IDC_OPTIONS_FIREWALL_LIMITPORTS, m_bLimitPortRange);
	DDX_Text(pDX, IDC_OPTIONS_FIREWALL_RANGEHIGH, m_PortRangeHigh);
	DDV_MaxChars(pDX, m_PortRangeHigh, 5);
	DDX_Text(pDX, IDC_OPTIONS_FIREWALL_RANGELOW, m_PortRangeLow);
	DDV_MaxChars(pDX, m_PortRangeLow, 5);
	DDX_Text(pDX, IDC_OPTIONS_FIREWALL_TRANSFERIP, m_TransferIP);
	DDX_Check(pDX, IDC_OPTIONS_FIREWALL_USETRANSFERIP, m_bUseTransferIP);
	DDX_Text(pDX, IDC_OPTIONS_FIREWALL_TRANSFERIP_IPV6, m_TransferIP6);
	DDX_Check(pDX, IDC_OPTIONS_FIREWALL_USETRANSFERIP_IPV6, m_bUseTransferIP6);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsFirewallPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsFirewallPage)
	ON_BN_CLICKED(IDC_OPTIONS_FIREWALL_LIMITPORTS, OnOptionsFirewallLimitports)
	ON_BN_CLICKED(IDC_OPTIONS_FIREWALL_USETRANSFERIP, OnOptionsFirewallUseTransferIP)
	ON_BN_CLICKED(IDC_OPTIONS_FIREWALL_USETRANSFERIP_IPV6, OnOptionsFirewallUseTransferIP6)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsFirewallPage 

void COptionsFirewallPage::OnOptionsFirewallLimitports() 
{
	if (!UpdateData(TRUE))
		return;
	m_cPortRangeLow.EnableWindow(m_bLimitPortRange);
	m_cPortRangeHigh.EnableWindow(m_bLimitPortRange);
}

BOOL COptionsFirewallPage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	m_cPortRangeLow.EnableWindow(m_bLimitPortRange);
	m_cPortRangeHigh.EnableWindow(m_bLimitPortRange);

	m_cTransferIP.EnableWindow(m_bUseTransferIP);
	m_cTransferIP6.EnableWindow(m_bUseTransferIP6);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void COptionsFirewallPage::OnOptionsFirewallUseTransferIP()
{
	UpdateData(TRUE);
	m_cTransferIP.EnableWindow(m_bUseTransferIP);
}

void COptionsFirewallPage::OnOptionsFirewallUseTransferIP6()
{
	UpdateData(TRUE);
	m_cTransferIP6.EnableWindow(m_bUseTransferIP6);
}
