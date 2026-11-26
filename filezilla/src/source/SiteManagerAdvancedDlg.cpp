// SiteManagerAdvancedDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "SiteManagerAdvancedDlg.h"
#include "SiteManager.h"
#include "misc\SBDestination.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSiteManagerAdvancedDlg 


CSiteManagerAdvancedDlg::CSiteManagerAdvancedDlg(t_SiteManagerItem *pItem)
	: CDialog(CSiteManagerAdvancedDlg::IDD, NULL)
	, m_nUTF8(0)
{
	ASSERT(pItem);
	m_pSiteManagerItem = pItem;

	//{{AFX_DATA_INIT(CSiteManagerAdvancedDlg)
	m_nTimeZoneOffset = 0;
	m_nPassive = -1;
	m_LocalDir = _T("");
	m_RemoteDir = _T("");
	//}}AFX_DATA_INIT
}


void CSiteManagerAdvancedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSiteManagerAdvancedDlg)
	DDX_Control(pDX, IDC_SITEMANAGER_ADVANCED_TZSPIN, m_SpinButton);
	DDX_Control(pDX, IDC_SITEMANAGER_ADVANCED_TZSPIN2, m_SpinButton2);
	DDX_Text(pDX, IDC_SITEMANAGER_ADVANCED_TZOFFSET, m_nTimeZoneOffset);
	DDV_MinMaxInt(pDX, m_nTimeZoneOffset, -24, 24);
	DDX_Text(pDX, IDC_SITEMANAGER_ADVANCED_TZOFFSET2, m_nTimeZoneOffset2);
	DDV_MinMaxInt(pDX, m_nTimeZoneOffset2, -59, 59);
	DDX_Radio(pDX, IDC_SITEMANAGER_ADVANCED_PASSIVE1, m_nPassive);
	DDX_Text(pDX, IDC_LOCALDIR, m_LocalDir);
	DDX_Text(pDX, IDC_REMOTEDIR, m_RemoteDir);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_SITEMANAGER_ADVANCED_UTF8, m_nUTF8);
}


BEGIN_MESSAGE_MAP(CSiteManagerAdvancedDlg, CDialog)
	//{{AFX_MSG_MAP(CSiteManagerAdvancedDlg)
	ON_BN_CLICKED(IDC_SITEMANAGER_LOCALDIR_BROWSE, OnSitemanagerLocaldirBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSiteManagerAdvancedDlg 

BOOL CSiteManagerAdvancedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_SpinButton.SetRange(-24, 24);
	m_SpinButton2.SetRange(-59, 59);

	CString str;
	GetDlgItemText(IDC_SITEMANAGER_ADVANCED_SITENAME, str);
	CString sitename;
	if (m_pSiteManagerItem->nLogonType)
		sitename.Format(str, m_pSiteManagerItem->name, m_pSiteManagerItem->User + "@" + m_pSiteManagerItem->Host);
	else
		sitename.Format(str, m_pSiteManagerItem->name, m_pSiteManagerItem->Host);
	SetDlgItemText(IDC_SITEMANAGER_ADVANCED_SITENAME, sitename);

	m_nPassive = m_pSiteManagerItem->nPasv;
	m_nTimeZoneOffset = m_pSiteManagerItem->nTimeZoneOffset;
	m_nTimeZoneOffset2 = m_pSiteManagerItem->nTimeZoneOffset2;

	m_LocalDir = m_pSiteManagerItem->LocalDir;
	m_RemoteDir = m_pSiteManagerItem->RemoteDir;
	
	m_nUTF8 = m_pSiteManagerItem->nUTF8;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CSiteManagerAdvancedDlg::OnOK() 
{
	if (!UpdateData())
		return;

	if (m_LocalDir!="")
	{
		m_LocalDir.Replace( _T("/"), _T("\\") );
		m_LocalDir.TrimRight( _T("\\") );
		m_LocalDir+="\\";
		
	}
	if (m_RemoteDir!="")
	{
		CServerPath path(m_RemoteDir);
		m_RemoteDir = path.GetPath();
	}
	
	m_pSiteManagerItem->LocalDir = m_LocalDir;
	m_pSiteManagerItem->RemoteDir = m_RemoteDir;
	m_pSiteManagerItem->nPasv = m_nPassive;
	m_pSiteManagerItem->nTimeZoneOffset = m_nTimeZoneOffset;
	m_pSiteManagerItem->nTimeZoneOffset2 = m_nTimeZoneOffset2;
	m_pSiteManagerItem->nUTF8 = m_nUTF8;
	
	CDialog::OnOK();
}

void CSiteManagerAdvancedDlg::OnSitemanagerLocaldirBrowse() 
{
	UpdateData(TRUE);
	CSBDestination sb(m_hWnd, IDS_BROWSEFORFOLDER);
	sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT);
	sb.SetInitialSelection(m_LocalDir);
	if (sb.SelectFolder())
	{
        m_LocalDir = sb.GetSelectedFolder();
		UpdateData(FALSE);
	}
}
