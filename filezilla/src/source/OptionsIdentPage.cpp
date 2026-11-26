// OptionsIdentPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "OptionsIdentPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsIdentPage 


COptionsIdentPage::COptionsIdentPage(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(COptionsIdentPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsIdentPage)
	m_bIdent = FALSE;
	m_bIdentConnect = FALSE;
	m_bSameIP = FALSE;
	m_System = _T("");
	m_UserID = _T("");
	//}}AFX_DATA_INIT
}


void COptionsIdentPage::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsIdentPage)
	DDX_Control(pDX, IDC_OPTIONS_IDENT_IDENTCONNECT, m_cIdentConnect);
	DDX_Control(pDX, IDC_OPTIONS_IDENT_SAMEIP, m_cSameIP);
	DDX_Control(pDX, IDC_OPTIONS_IDENT_SYSTEM, m_cSystem);
	DDX_Control(pDX, IDC_OPTIONS_IDENT_USERID, m_cUserID);
	DDX_Check(pDX, IDC_OPTIONS_IDENT_IDENT, m_bIdent);
	DDX_Check(pDX, IDC_OPTIONS_IDENT_IDENTCONNECT, m_bIdentConnect);
	DDX_Check(pDX, IDC_OPTIONS_IDENT_SAMEIP, m_bSameIP);
	DDX_Text(pDX, IDC_OPTIONS_IDENT_SYSTEM, m_System);
	DDX_Text(pDX, IDC_OPTIONS_IDENT_USERID, m_UserID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsIdentPage, CSAPrefsSubDlg)
	//{{AFX_MSG_MAP(COptionsIdentPage)
	ON_BN_CLICKED(IDC_OPTIONS_IDENT_IDENT, OnOptionsIdentIdent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COptionsIdentPage 

void COptionsIdentPage::OnOptionsIdentIdent() 
{
	if (!UpdateData(TRUE))
		return;
	m_cUserID.EnableWindow(m_bIdent);
	m_cSystem.EnableWindow(m_bIdent);
	m_cSameIP.EnableWindow(m_bIdent);
	m_cIdentConnect.EnableWindow(m_bIdent);
}

BOOL COptionsIdentPage::OnInitDialog() 
{
	CSAPrefsSubDlg::OnInitDialog();
	
	OnOptionsIdentIdent();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
