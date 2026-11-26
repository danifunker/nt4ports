// SpeedLimitRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "filezilla.h"
#include "SpeedLimitRuleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpeedLimitRuleDlg dialog


CSpeedLimitRuleDlg::CSpeedLimitRuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeedLimitRuleDlg::IDD, pParent)
{
	m_Day[ 0] = TRUE;
	m_Day[ 1] = TRUE;
	m_Day[ 2] = TRUE;
	m_Day[ 3] = TRUE;
	m_Day[ 4] = TRUE;
	m_Day[ 5] = TRUE;
	m_Day[ 6] = TRUE;
	//{{AFX_DATA_INIT(CSpeedLimitRuleDlg)
	m_DateCheck = FALSE;
	m_Date = CTime::GetCurrentTime();
	m_FromCheck = FALSE;
	m_FromTime = CTime::GetCurrentTime();
	m_ToCheck = FALSE;
	m_ToTime = CTime::GetCurrentTime();
	m_Speed = 8;
	//}}AFX_DATA_INIT
}


void CSpeedLimitRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpeedLimitRuleDlg)
	DDX_Control(pDX, IDC_TO_TIME, m_ToCtrl);
	DDX_Control(pDX, IDC_FROM_TIME, m_FromCtrl);
	DDX_Control(pDX, IDC_DATE_VALUE, m_DateCtrl);
	DDX_Check(pDX, IDC_DATE_CHECK, m_DateCheck);
	DDX_DateTimeCtrl(pDX, IDC_DATE_VALUE, m_Date);
	DDX_Check(pDX, IDC_FROM_CHECK, m_FromCheck);
	DDX_DateTimeCtrl(pDX, IDC_FROM_TIME, m_FromTime);
	DDX_Check(pDX, IDC_TO_CHECK, m_ToCheck);
	DDX_DateTimeCtrl(pDX, IDC_TO_TIME, m_ToTime);
	DDX_Text(pDX, IDC_SPEED, m_Speed);
	DDV_MinMaxInt(pDX, m_Speed, 1, 1000000000);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_DAY1, m_Day[ 0]);
	DDX_Check(pDX, IDC_CHECK_DAY2, m_Day[ 1]);
	DDX_Check(pDX, IDC_CHECK_DAY3, m_Day[ 2]);
	DDX_Check(pDX, IDC_CHECK_DAY4, m_Day[ 3]);
	DDX_Check(pDX, IDC_CHECK_DAY5, m_Day[ 4]);
	DDX_Check(pDX, IDC_CHECK_DAY6, m_Day[ 5]);
	DDX_Check(pDX, IDC_CHECK_DAY7, m_Day[ 6]);
}


BEGIN_MESSAGE_MAP(CSpeedLimitRuleDlg, CDialog)
	//{{AFX_MSG_MAP(CSpeedLimitRuleDlg)
	ON_BN_CLICKED(IDC_DATE_CHECK, OnDateCheck)
	ON_BN_CLICKED(IDC_TO_CHECK, OnToCheck)
	ON_BN_CLICKED(IDC_FROM_CHECK, OnFromCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpeedLimitRuleDlg message handlers

void CSpeedLimitRuleDlg::OnDateCheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_DateCtrl.EnableWindow( m_DateCheck);
}

void CSpeedLimitRuleDlg::OnToCheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_ToCtrl.EnableWindow( m_ToCheck);
}

void CSpeedLimitRuleDlg::OnFromCheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_FromCtrl.EnableWindow( m_FromCheck);
}

BOOL CSpeedLimitRuleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_DateCtrl.EnableWindow( m_DateCheck);
	m_ToCtrl.EnableWindow( m_ToCheck);
	m_FromCtrl.EnableWindow( m_FromCheck);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CSpeedLimit * CSpeedLimitRuleDlg::GetSpeedLimit()
{
	CSpeedLimit *res = new CSpeedLimit();

	res->m_DateCheck = m_DateCheck;
	res->m_Date = m_Date;
	res->m_FromCheck = m_FromCheck;
	res->m_FromTime = m_FromTime;
	res->m_ToCheck = m_ToCheck;
	res->m_ToTime = m_ToTime;
	res->m_Speed = m_Speed;

	for ( int i = 0; i < 7; i++)
		res->m_Day[ i] = m_Day[ i];

	return res;
}

void CSpeedLimitRuleDlg::FillFromSpeedLimt(CSpeedLimit *sl)
{
	m_DateCheck = sl->m_DateCheck;
	m_Date = sl->m_Date;
	m_FromCheck = sl->m_FromCheck;
	m_FromTime = sl->m_FromTime;
	m_ToCheck = sl->m_ToCheck;
	m_ToTime = sl->m_ToTime;
	m_Speed = sl->m_Speed;

	for ( int i = 0; i < 7; i++)
		m_Day[ i] = sl->m_Day[ i];
}

