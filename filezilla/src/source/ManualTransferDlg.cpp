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

// ManualTransferDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "ManualTransferDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CManualTransferDlg


CManualTransferDlg::CManualTransferDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManualTransferDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManualTransferDlg)
	m_bBypassProxy = FALSE;
	m_Host = _T("");
	m_LocalFile = _T("");
	m_nLogontype = -1;
	m_Pass = _T("");
	m_Port = _T("");
	m_RemoteFile = _T("");
	m_User = _T("");
	m_nServer = -1;
	m_nUpDown = -1;
	m_bStartNow = FALSE;
	//}}AFX_DATA_INIT
}


void CManualTransferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManualTransferDlg)
	DDX_Control(pDX, IDC_MANUALTRANSFER_SERVERTYPE, m_ServerType);
	DDX_Check(pDX, IDC_MANUALTRANSFER_BYPASSPROXY, m_bBypassProxy);
	DDX_Text(pDX, IDC_MANUALTRANSFER_HOST, m_Host);
	DDX_Text(pDX, IDC_MANUALTRANSFER_LOCALFILE, m_LocalFile);
	DDX_Radio(pDX, IDC_MANUALTRANSFER_LOGONTYPE1, m_nLogontype);
	DDX_Text(pDX, IDC_MANUALTRANSFER_PASS, m_Pass);
	DDX_Text(pDX, IDC_MANUALTRANSFER_PORT, m_Port);
	DDX_Text(pDX, IDC_MANUALTRANSFER_REMOTEFILE, m_RemoteFile);
	DDX_Text(pDX, IDC_MANUALTRANSFER_USER, m_User);
	DDX_Radio(pDX, IDC_MANUALTRANSFER_SERVER1, m_nServer);
	DDX_Radio(pDX, IDC_MANUALTRANSFER_UPDOWN1, m_nUpDown);
	DDX_Check(pDX, IDC_MANUALTRANSFER_STARTNOW, m_bStartNow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CManualTransferDlg, CDialog)
	//{{AFX_MSG_MAP(CManualTransferDlg)
	ON_BN_CLICKED(IDC_MANUALTRANSFER_SERVER1, OnManualtransferServer)
	ON_BN_CLICKED(IDC_MANUALTRANSFER_SERVER2, OnManualtransferServer)
	ON_BN_CLICKED(IDC_MANUALTRANSFER_LOGONTYPE1, OnManualtransferLogontype)
	ON_BN_CLICKED(IDC_MANUALTRANSFER_LOGONTYPE2, OnManualtransferLogontype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CManualTransferDlg

int CManualTransferDlg::Show(t_transferfile &transferfile, t_server *pServer, CString LocalPath, CServerPath RemotePath)
{
	m_pServer=pServer;
	m_LocalPath=LocalPath;
	m_RemotePath=RemotePath;
	m_LocalPath.TrimRight( _T("\\") );
	m_nUpDown=0;
	if (DoModal()==IDOK)
	{
		transferfile=m_Transferfile;
		return m_bStartNow?2:1;
	}
	return 0;
}

void CManualTransferDlg::OnOK()
{
	UpdateData(TRUE);

	BOOL bError=FALSE;
	m_LocalFile.Replace( _T("/"), _T("\\") );
	while (m_LocalFile.Replace( _T("\\\\"), _T("\\")));
	m_LocalFile.TrimLeft( _T("\\") );
	if (m_LocalFile=="" && ( m_nUpDown==1 || m_LocalPath=="\\" ) )
		bError=TRUE;
	else
	{
		int pos=m_LocalFile.Find( _T(":") );
		if (pos!=m_LocalFile.ReverseFind(':'))
			bError=TRUE;
		else if (pos!=-1 && pos!=1)
			bError=TRUE;
		else
		{
			if (pos==1 && m_LocalFile.GetLength()<3)
				bError=TRUE;
			else if (pos==1 && m_LocalFile[2]!='\\')
				bError=TRUE;
			else if (m_LocalFile.Find( _T("?") )!=-1 ||
					m_LocalFile.Find( _T("*") )!=-1 ||
					m_LocalFile.Find( _T("\"") )!=-1 ||
					m_LocalFile.Find( _T("<") )!=-1 ||
					m_LocalFile.Find( _T(">") )!=-1 ||
					m_LocalFile.Find( _T("|") )!=-1)
				bError=TRUE;
			else if (pos==-1)
				m_LocalFile=m_LocalPath + _T("\\") + m_LocalFile;
			else if (m_LocalFile.Right(1)=="\\" && m_nUpDown==1)
				bError=TRUE;
		}
	}
	if (bError)
	{
		UpdateData(FALSE);
		GetDlgItem(IDC_MANUALTRANSFER_LOCALFILE)->SetFocus();
		AfxMessageBox(IDS_MANUALTRANSFER_LOCALFILE);
		return;
	}

	CServerPath path=m_RemotePath;
	if (m_pServer)
		path.SetServer(*m_pServer);

	if (!path.ChangePath(m_RemoteFile, TRUE))
	{
		if (!path.ChangePath(m_RemoteFile))
			bError=TRUE;
		else
		{
			if (!m_nUpDown)
				bError=TRUE;
			else
			{
				int pos=m_LocalFile.ReverseFind('\\');
				ASSERT(pos!=-1);
				m_Transferfile.remotefile=m_LocalFile.Mid(pos+1);
				m_Transferfile.remotepath=path;
				m_RemoteFile=path.GetPath()+m_LocalFile.Mid(pos+1);
			}
		}
	}
	else
	{
		if ( !m_nUpDown && m_LocalFile.Right(1) == _T("\\") )
			m_LocalFile+=m_RemoteFile;
		m_Transferfile.remotefile=m_RemoteFile;
		m_Transferfile.remotepath=path;
		m_RemoteFile=path.GetPath()+m_RemoteFile;
		ASSERT(m_RemoteFile!="");
	}

	if (bError)
	{
		UpdateData(FALSE);
		GetDlgItem(IDC_MANUALTRANSFER_REMOTEFILE)->SetFocus();
		AfxMessageBox(IDS_MANUALTRANSFER_REMOTEFILE);
		return;
	}
	m_Transferfile.localfile=m_LocalFile;

	m_Transferfile.get=m_nUpDown==0;

	if (!m_nUpDown)
		m_Transferfile.size=-1;
	else
	{
		if (!GetLength64(m_Transferfile.localfile, m_Transferfile.size))
			m_Transferfile.size=-1;
	}

	if (!m_nServer)
		m_Transferfile.server=*m_pServer;
	else
	{
		if (m_Host=="")
		{
			GetDlgItem(IDC_MANUALTRANSFER_HOST)->SetFocus();
			AfxMessageBox(IDS_MANUALTRANSFER_HOST, MB_ICONEXCLAMATION);
			return;
		}
		int pos=m_Host.Find( _T("://") );
		if (pos!=-1)
		{
			CString tmp=m_Host.Left(pos+3);
			tmp.MakeLower();
			if (tmp==_T("ftp://"))
			{
				m_Host=m_Host.Right(m_Host.GetLength()-pos-3);
				m_ServerType.SetCurSel(0);
			}
			else if (tmp==_T("ftps://") )
			{
				m_Host=m_Host.Right(m_Host.GetLength()-pos-3);
				m_ServerType.SetCurSel(1);
			}
			else if (tmp==_T("sftp://") )
			{
				m_Host=m_Host.Right(m_Host.GetLength()-pos-3);
				m_ServerType.SetCurSel(3);
			}
			else
			{
				GetDlgItem(IDC_MANUALTRANSFER_HOST)->SetFocus();
				AfxMessageBox(IDS_ERRORMSG_URLSYNTAXINVALID,MB_ICONEXCLAMATION);
				return;
			}
		}
		pos=m_Host.Find('/');
		CString path="";
		if (pos!=-1)
		{
			GetDlgItem(IDC_MANUALTRANSFER_HOST)->SetFocus();
			AfxMessageBox(IDS_MANUALTRANSFER_HOST, MB_ICONEXCLAMATION);
			return;
		}
		pos=m_Host.ReverseFind('@');
		if (pos!=-1)
		{
			int pos2=m_Host.Find(':');
			if (pos2==-1 || !pos2 || pos2>=(pos-1))
			{
				GetDlgItem(IDC_MANUALTRANSFER_HOST)->SetFocus();
				AfxMessageBox(IDS_ERRORMSG_URLSYNTAXINVALID,MB_ICONEXCLAMATION);
				return;
			}
			m_nLogontype=1;
			m_User=m_Host.Left(pos2);
			m_Pass=m_Host.Mid(pos2+1, pos-pos2-1);
			m_Host=m_Host.Right(m_Host.GetLength()-pos-1);
		}
		pos=m_Host.ReverseFind(':');
		if (pos!=-1)
		{
			m_Port=m_Host.Right(m_Host.GetLength()-pos-1);
			m_Host=m_Host.Left(pos);
		}
		m_Transferfile.server.host=m_Host;

		if (m_Port=="")
			m_Port="21";

		int nPort=_ttoi(m_Port);
		if (nPort<1 || nPort>65535)
		{
			GetDlgItem(IDC_MANUALTRANSFER_PORT)->SetFocus();
			AfxMessageBox(IDS_MANUALTRANSFER_PORT,MB_ICONEXCLAMATION);
			return;
		}
		m_Transferfile.server.port=nPort;

		if (m_nLogontype)
		{
			if (m_User=="")
			{
				BOOL bUseGSS=FALSE;
				if (COptions::GetOptionVal(OPTION_USEGSS))
				{
					USES_CONVERSION;

					CString GssServers=COptions::GetOption(OPTION_GSSSERVERS);
					hostent *fullname=gethostbyname(T2CA(m_Host));
					CString host;
					if (fullname)
						host=fullname->h_name;
					else
						host=m_Host;
					host.MakeLower();
					int i;
					while ((i=GssServers.Find( _T(";") ))!=-1)
					{
						if (("."+GssServers.Left(i))==host.Right(GssServers.Left(i).GetLength()+1) || GssServers.Left(i)==host)
						{
							bUseGSS=TRUE;
							break;
						}
						GssServers=GssServers.Mid(i+1);
					}
				}
				if (!bUseGSS)
				{
					GetDlgItem(IDC_MANUALTRANSFER_USER)->SetFocus();
					AfxMessageBox(IDS_MANUALTRANSFER_USER,MB_ICONEXCLAMATION);
					return;
				}
				else
				{
					m_User="";
					m_Pass="";
				}
				m_Transferfile.server.user=m_User;
				m_Transferfile.server.pass=m_Pass;
			}
			else
			{
				m_Transferfile.server.user=m_User;
				m_Transferfile.server.pass=m_Pass;
			}
		}
		else
		{
			m_Transferfile.server.user="anonymous";
			m_Transferfile.server.pass=COptions::GetOption(OPTION_ANONPWD);
		}

		switch (m_ServerType.GetCurSel())
		{
		case 0:
			m_Transferfile.server.nServerType=FZ_SERVERTYPE_FTP;
			break;
		case 1:
			m_Transferfile.server.nServerType=FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_LAYER_SSL_IMPLICIT;
			break;
		case 2:
			m_Transferfile.server.nServerType=FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_LAYER_SSL_EXPLICIT;
			break;
		case 3:
			m_Transferfile.server.nServerType=FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_SUB_FTP_SFTP;
			break;
		case 4:
			m_Transferfile.server.nServerType = FZ_SERVERTYPE_FTP|FZ_SERVERTYPE_LAYER_TLS_EXPLICIT;
		default:
			m_Transferfile.server.nServerType=FZ_SERVERTYPE_FTP;
		}

		m_Transferfile.server.fwbypass=m_bBypassProxy;
	}

	UpdateData(FALSE);

	CDialog::OnOK();
}

BOOL CManualTransferDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!m_pServer)
	{
		GetDlgItem(IDC_MANUALTRANSFER_SERVER1)->EnableWindow(FALSE);
		m_nServer=1;
	}
	else
		m_nServer=0;

	m_nLogontype=0;

	m_ServerType.SetCurSel(0);

	UpdateData(FALSE);
	SetCtrlState();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CManualTransferDlg::SetCtrlState()
{
	UpdateData(TRUE);
	if (!m_nServer)
	{
		GetDlgItem(IDC_MANUALTRANSFER_HOST)->EnableWindow(FALSE);
		GetDlgItem(IDC_MANUALTRANSFER_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_MANUALTRANSFER_LOGONTYPE1)->EnableWindow(FALSE);
		GetDlgItem(IDC_MANUALTRANSFER_LOGONTYPE2)->EnableWindow(FALSE);
		GetDlgItem(IDC_MANUALTRANSFER_USER)->EnableWindow(FALSE);
		GetDlgItem(IDC_MANUALTRANSFER_PASS)->EnableWindow(FALSE);
		GetDlgItem(IDC_MANUALTRANSFER_BYPASSPROXY)->EnableWindow(FALSE);
		GetDlgItem(IDC_MANUALTRANSFER_SERVERTYPE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_MANUALTRANSFER_HOST)->EnableWindow(TRUE);
		GetDlgItem(IDC_MANUALTRANSFER_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_MANUALTRANSFER_LOGONTYPE1)->EnableWindow(TRUE);
		GetDlgItem(IDC_MANUALTRANSFER_LOGONTYPE2)->EnableWindow(TRUE);
		if (!m_nLogontype)
		{
			GetDlgItem(IDC_MANUALTRANSFER_USER)->EnableWindow(FALSE);
			GetDlgItem(IDC_MANUALTRANSFER_PASS)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_MANUALTRANSFER_USER)->EnableWindow(TRUE);
			GetDlgItem(IDC_MANUALTRANSFER_PASS)->EnableWindow(TRUE);
		}
		GetDlgItem(IDC_MANUALTRANSFER_BYPASSPROXY)->EnableWindow(TRUE);
		GetDlgItem(IDC_MANUALTRANSFER_SERVERTYPE)->EnableWindow(TRUE);
	}

}

void CManualTransferDlg::OnManualtransferServer()
{
	SetCtrlState();
}

void CManualTransferDlg::OnManualtransferLogontype()
{
	SetCtrlState();
}
