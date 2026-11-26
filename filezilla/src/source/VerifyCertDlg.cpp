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
// along with this program; if not, write Until the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, BosUntiln, MA  02111-1307, USA.

// VerifyCertDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "resource.h"
#include "VerifyCertDlg.h"
#include "openssl\ssl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CVerifyCertDlg 


CVerifyCertDlg::CVerifyCertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVerifyCertDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVerifyCertDlg)
	m_bAlways = FALSE;
	//}}AFX_DATA_INIT
}


void CVerifyCertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVerifyCertDlg)
	DDX_Control(pDX, IDC_VERIFY_CERT_ALWAYS, m_cAlways);
	DDX_Check(pDX, IDC_VERIFY_CERT_ALWAYS, m_bAlways);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVerifyCertDlg, CDialog)
	//{{AFX_MSG_MAP(CVerifyCertDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CVerifyCertDlg 

BOOL CVerifyCertDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_VERIFY_CERT_ISSUER_ORGANIZATION)->SetWindowText(m_CertData.issuer.Organization);
	GetDlgItem(IDC_VERIFY_CERT_ISSUER_COMMONNAME)->SetWindowText(m_CertData.issuer.CommonName);
	GetDlgItem(IDC_VERIFY_CERT_ISSUER_UNIT)->SetWindowText(m_CertData.issuer.Unit);
	GetDlgItem(IDC_VERIFY_CERT_ISSUER_MAIL)->SetWindowText(m_CertData.issuer.Mail);
	GetDlgItem(IDC_VERIFY_CERT_ISSUER_COUNTRY)->SetWindowText(m_CertData.issuer.Country);
	GetDlgItem(IDC_VERIFY_CERT_ISSUER_STATEPROVINCE)->SetWindowText(m_CertData.issuer.StateProvince);
	GetDlgItem(IDC_VERIFY_CERT_ISSUER_TOWN)->SetWindowText(m_CertData.issuer.Town);
	GetDlgItem(IDC_VERIFY_CERT_ISSUER_OTHER)->SetWindowText(m_CertData.issuer.Other);
	
	GetDlgItem(IDC_VERIFY_CERT_SUBJECT_ORGANIZATION)->SetWindowText(m_CertData.subject.Organization);
	GetDlgItem(IDC_VERIFY_CERT_SUBJECT_COMMONNAME)->SetWindowText(m_CertData.subject.CommonName);
	GetDlgItem(IDC_VERIFY_CERT_SUBJECT_UNIT)->SetWindowText(m_CertData.subject.Unit);
	GetDlgItem(IDC_VERIFY_CERT_SUBJECT_MAIL)->SetWindowText(m_CertData.subject.Mail);
	GetDlgItem(IDC_VERIFY_CERT_SUBJECT_COUNTRY)->SetWindowText(m_CertData.subject.Country);
	GetDlgItem(IDC_VERIFY_CERT_SUBJECT_STATEPROVINCE)->SetWindowText(m_CertData.subject.StateProvince);
	GetDlgItem(IDC_VERIFY_CERT_SUBJECT_TOWN)->SetWindowText(m_CertData.subject.Town);
	GetDlgItem(IDC_VERIFY_CERT_SUBJECT_OTHER)->SetWindowText(m_CertData.subject.Other);

	TRY
	{
		COleDateTime validFrom(m_CertData.validFrom.y, m_CertData.validFrom.M, m_CertData.validFrom.d, m_CertData.validFrom.h, m_CertData.validFrom.m, m_CertData.validFrom.s);
		GetDlgItem(IDC_VERIFY_CERT_VALIDFROM)->SetWindowText(validFrom.Format() + _T(" GMT"));
	}
	CATCH_ALL(e)
	{
		GetDlgItem(IDC_VERIFY_CERT_VALIDFROM)->SetWindowText(_T("Invalid date/time"));
	}
	END_CATCH_ALL

	TRY
	{
		COleDateTime validUntil(m_CertData.validUntil.y, m_CertData.validUntil.M, m_CertData.validUntil.d, m_CertData.validUntil.h, m_CertData.validUntil.m, m_CertData.validUntil.s);
		GetDlgItem(IDC_VERIFY_CERT_VALIDUNTIL)->SetWindowText(validUntil.Format() + _T(" GMT"));
	}
	CATCH_ALL(e)
	{
		GetDlgItem(IDC_VERIFY_CERT_VALIDUNTIL)->SetWindowText(_T("Invalid date/time"));
	}
	END_CATCH_ALL

	CString hashStr;
	TCHAR hexChar[2];
	hexChar[1] = 0;
	for (int i = 0; i < 20; i++)
	{
		int digit = m_CertData.hash[i] >> 4;
		if (digit > 9)
			hexChar[0] = 'A' + digit - 10;
		else
			hexChar[0] = '0' + digit;
		hashStr += hexChar;

		digit = m_CertData.hash[i] & 0xF;
		if (digit > 9)
			hexChar[0] = 'A' + digit - 10;
		else
			hexChar[0] = '0' + digit;
		hashStr += hexChar;

		hashStr += " ";
	}
	GetDlgItem(IDC_VERIFY_CERT_HASH)->SetWindowText(hashStr);

	CString str;
	switch (m_CertData.verificationResult)
	{
		case X509_V_OK:
			str.LoadString(IDS_CERT_OK);
			break;
		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
			str.LoadString(IDS_CERT_ERR_UNABLE_TO_GET_ISSUER_CERT);
			break;
		case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
			str.LoadString(IDS_CERT_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE);
			break;
		case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
			str.LoadString(IDS_CERT_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY);
			break;
		case X509_V_ERR_CERT_SIGNATURE_FAILURE:
			str.LoadString(IDS_CERT_ERR_CERT_SIGNATURE_FAILURE);
			break;
		case X509_V_ERR_CERT_NOT_YET_VALID:
			str.LoadString(IDS_CERT_ERR_CERT_NOT_YET_VALID);
			break;
		case X509_V_ERR_CERT_HAS_EXPIRED:
			str.LoadString(IDS_CERT_ERR_CERT_HAS_EXPIRED);
			break;
		case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
			str.LoadString(IDS_CERT_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD);
			break;
		case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
			str.LoadString(IDS_CERT_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD);
			break;
		case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
			str.LoadString(IDS_CERT_ERR_DEPTH_ZERO_SELF_SIGNED_CERT);
			break;
		case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
			str.LoadString(IDS_CERT_ERR_SELF_SIGNED_CERT_IN_CHAIN);
			break;
		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
			str.LoadString(IDS_CERT_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY);
			break;
		case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
			str.LoadString(IDS_CERT_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE);
			break;
		case X509_V_ERR_INVALID_CA:
			str.LoadString(IDS_CERT_ERR_INVALID_CA);
			break;
		case X509_V_ERR_PATH_LENGTH_EXCEEDED:
			str.LoadString(IDS_CERT_ERR_PATH_LENGTH_EXCEEDED);
			break;
		case X509_V_ERR_INVALID_PURPOSE:
			str.LoadString(IDS_CERT_ERR_INVALID_PURPOSE);
			break;
		case X509_V_ERR_CERT_UNTRUSTED:
			str.LoadString(IDS_CERT_ERR_CERT_UNTRUSTED);
			break;
		case X509_V_ERR_CERT_REJECTED:
			str.LoadString(IDS_CERT_ERR_CERT_REJECTED);
			break;
		case X509_V_ERR_KEYUSAGE_NO_CERTSIGN:
			str.LoadString(IDS_CERT_ERR_KEYUSAGE_NO_CERTSIGN);
			break;
		case IDS_CERT_ERR_CERT_CHAIN_TOO_LONG:
			str.LoadString(IDS_CERT_ERR_CERT_CHAIN_TOO_LONG);
			break;
		default:
			str.LoadString(IDS_CERT_ERR_UNKNOWN);
	}
	CString depthStr;
	if (m_CertData.verificationResult != X509_V_OK)
		depthStr.Format(IDS_CERT_ERRDEPTH, m_CertData.verificationDepth + 1);
	GetDlgItem(IDC_VERIFY_CERT_SUMMARY)->SetWindowText(str + "\n" + depthStr);

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CERT));
	CStatic *ctrl=((CStatic *)GetDlgItem(IDC_VERIFY_CERT_ICON));
	if (ctrl)
		ctrl->SetIcon(hIcon);

	return TRUE;  // return TRUE unless you set the focus Until a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

CString	GetProgramDir()
{
	CString	RtnVal;
	TCHAR	FileName[MAX_PATH];
	GetModuleFileName(NULL, FileName, MAX_PATH);
	RtnVal = FileName;
	RtnVal = RtnVal.Left(RtnVal.ReverseFind('\\'));
	return RtnVal;
}

BOOL CVerifyCertDlg::IsHashTrusted(char *hash)
{
	HANDLE hFile = CreateFile(GetProgramDir() + _T("\\Trusted Certs.dat"), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		unsigned char sha1[20];
		while (TRUE)
		{
			DWORD numread;
			if (!ReadFile(hFile, sha1, 20, &numread, 0) || numread!=20)
				break;
			
			if (!memcmp(hash, sha1, 20))
			{
				CloseHandle(hFile);
				return TRUE;
			}
		}
		CloseHandle(hFile);
	}

	return FALSE;
}

void CVerifyCertDlg::OnOK() 
{
	UpdateData();
	
	CMutex mutex(FALSE, _T("FileZilla SSL Cert Storage Mutex"));
	CSingleLock(&mutex, TRUE);
	if (m_bAlways && !IsHashTrusted((char *)m_CertData.hash))
	{
		HANDLE hFile = CreateFile(GetProgramDir() + _T("\\Trusted Certs.dat"), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (SetFilePointer(hFile, 0, 0, FILE_END) != INVALID_SET_FILE_POINTER || GetLastError()==NOERROR)
			{
				DWORD numwritten;
				WriteFile(hFile, m_CertData.hash, 20, &numwritten, 0);
			}
			CloseHandle(hFile);
		}
	}
	
	CDialog::OnOK();
}
