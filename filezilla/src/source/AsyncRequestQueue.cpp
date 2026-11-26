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
#include "stdafx.h"
#include "AsyncRequestQueue.h"
#include "FileExistsDlg.h"
#include "VerifyCertDlg.h"
#include "EnterSomething.h"

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

std::list<CAsyncRequestQueue::t_QueueItem> CAsyncRequestQueue::m_RequestList;
BOOL CAsyncRequestQueue::m_bActive = FALSE;
std::set<CString> CAsyncRequestQueue::m_HostKeys;
std::list<CAsyncRequestQueue::t_SslCertHash> CAsyncRequestQueue::m_SessionCertHash;

void CAsyncRequestQueue::ProcessRequest(CAsyncRequestData *pData, CFileZillaApi  *pFileZillaApi, BOOL bFromQueue)
{
	ASSERT(pData);
	ASSERT(pFileZillaApi);

	if (!pData)
		return;
	if (!pFileZillaApi)
	{
		delete pData;
		return;
	}

	switch (pData->nRequestType)
	{
		case FZ_ASYNCREQUEST_OVERWRITE:
			{
				COverwriteRequestData *pFileExistData = reinterpret_cast<COverwriteRequestData *>(pData);
				if (CFileExistsDlg::ProcessRequest(pFileExistData, pFileZillaApi, TRUE))
					return;
			}
			break;
		case FZ_ASYNCREQUEST_NEWHOSTKEY:
			if (bFromQueue)
			{
				CNewHostKeyRequestData *pHostKeyData = reinterpret_cast<CNewHostKeyRequestData *>(pData);
				if (m_HostKeys.find(pHostKeyData->Hostkey) != m_HostKeys.end())
				{
					SendReply(0, pFileZillaApi, pData);
					return;
				}
			}
			break;
		case FZ_ASYNCREQUEST_CHANGEDHOSTKEY:
			if (bFromQueue)
			{
				CChangedHostKeyRequestData *pHostKeyData = reinterpret_cast<CChangedHostKeyRequestData *>(pData);
				if (m_HostKeys.find(pHostKeyData->Hostkey) != m_HostKeys.end())
				{
					SendReply(0, pFileZillaApi, pData);
					return;
				}
			}
			break;
		case FZ_ASYNCREQUEST_VERIFYCERT:
			{
				CVerifyCertRequestData *pVerifyCertData = reinterpret_cast<CVerifyCertRequestData *>(pData);
				ASSERT(pVerifyCertData->pCertData);
				if (bFromQueue)
				{
					std::list<t_SslCertHash>::iterator iter;
					for (iter = m_SessionCertHash.begin(); iter != m_SessionCertHash.end(); iter++)
						if (!memcmp(iter->hash, pVerifyCertData->pCertData->hash, 20))
						{
							SendReply(1, pFileZillaApi, pVerifyCertData);
							return;
						}
				}
				if (CVerifyCertDlg::IsHashTrusted((char*)pVerifyCertData->pCertData->hash))
				{
					SendReply(1, pFileZillaApi, pVerifyCertData);
					return;
				}
			}
			break;
	}

	t_QueueItem data;
	data.pData = pData;
	data.pFileZillaApi = pFileZillaApi;
	data.bFromQueue = bFromQueue;
	m_RequestList.push_back(data);
	
	if (m_bActive)
		return;

	m_bActive=TRUE;
	while (!m_RequestList.empty())
	{
		data=m_RequestList.front();
		m_RequestList.pop_front();

		switch(data.pData->nRequestType)
		{
			case FZ_ASYNCREQUEST_OVERWRITE:
				{
					COverwriteRequestData *pData = reinterpret_cast<COverwriteRequestData *>(data.pData);
					CFileExistsDlg::ProcessRequest(pData, data.pFileZillaApi, FALSE);
				}
				break;
			case FZ_ASYNCREQUEST_VERIFYCERT:
				{
					CVerifyCertDlg dlg;
					CVerifyCertRequestData *pData = reinterpret_cast<CVerifyCertRequestData *>(data.pData);
					ASSERT(pData->pCertData);
					dlg.m_CertData = *pData->pCertData;
					::EndMenu();
					int res = dlg.DoModal();
					int nAction = 0;
					if (res == IDOK)
					{
						t_SslCertHash hash;
						memcpy(hash.hash, pData->pCertData->hash, 20);
						std::list<t_SslCertHash>::iterator iter;
						for (iter = m_SessionCertHash.begin(); iter != m_SessionCertHash.end(); iter++)
							if (!memcmp(iter->hash, hash.hash, 20))
								break;
						if (iter == m_SessionCertHash.end())
							m_SessionCertHash.push_back(hash);

						if (dlg.m_bAlways)
							nAction = 2;
						else
							nAction = 1;
					}
					SendReply(nAction, data.pFileZillaApi, pData);
				}
				break;
			case FZ_ASYNCREQUEST_GSS_AUTHFAILED:
				{
					int nAction = 0;
					::EndMenu();
					if (AfxMessageBox(IDS_GSSERROR_AUTHFAILED, 
						MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)==IDYES)
						nAction = 1;
					SendReply(nAction, data.pFileZillaApi, data.pData);
				}
				break;
			case FZ_ASYNCREQUEST_GSS_NEEDPASS:
				{
					CGssNeedPassRequestData *pData = reinterpret_cast<CGssNeedPassRequestData *>(data.pData);
					int nAction = 0;
					CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTPASSWORD, IDS_INPUTDIALOGTEXT_INPUTPASSWORD, '*');
					::EndMenu();
					if (dlg.DoModal() == IDOK)
					{
						pData->pass = dlg.m_String;
						nAction = 1;
					}
					SendReply(nAction, data.pFileZillaApi, pData);
				}
				break;
			case FZ_ASYNCREQUEST_GSS_NEEDUSER:
				{
					CGssNeedUserRequestData *pData = reinterpret_cast<CGssNeedUserRequestData *>(data.pData);
					int nAction = 0;
					CEnterSomething dlg(IDS_INPUTDIALOGTITLE_INPUTUSER, IDS_INPUTDIALOGTEXT_INPUTUSER, '*');
					::EndMenu();
					if (dlg.DoModal() == IDOK)
					{
						pData->user = dlg.m_String;
						nAction = 1;
					}
					SendReply(nAction, data.pFileZillaApi, pData);
				}
				break;
			case FZ_ASYNCREQUEST_NEWHOSTKEY:
				{
					CNewHostKeyRequestData *pData = reinterpret_cast<CNewHostKeyRequestData *>(data.pData);
					int nAction = -1;
					CString str;
					str.Format(IDS_QUESTION_SFTP_NEWHOSTKEY, pData->Hostkey);
					::EndMenu();
					int res = AfxMessageBox(str, MB_YESNOCANCEL|MB_ICONQUESTION);
					if (res == IDYES)
						nAction = 1;
					else if (res == IDNO)
					{
						if (m_HostKeys.find(pData->Hostkey) == m_HostKeys.end())
							m_HostKeys.insert(pData->Hostkey);
						nAction = 0;
					}
					SendReply(nAction, data.pFileZillaApi, pData);
				}
				break;
			case FZ_ASYNCREQUEST_CHANGEDHOSTKEY:
				{
					CChangedHostKeyRequestData *pData = reinterpret_cast<CChangedHostKeyRequestData *>(data.pData);
					int nAction = -1;
					CString str;
					str.Format(IDS_QUESTION_SFTP_CHANGEDHOSTKEY, pData->Hostkey);
					::EndMenu();
					int res = AfxMessageBox(str, MB_YESNOCANCEL|MB_ICONQUESTION);
					if (res == IDYES)
						nAction = 1;
					else if (res == IDNO)
					{
						if (m_HostKeys.find(pData->Hostkey) == m_HostKeys.end())
							m_HostKeys.insert(pData->Hostkey);
						nAction = 0;
					}
					SendReply(nAction, data.pFileZillaApi, pData);
				}
				break;
			case FZ_ASYNCREQUEST_KEYBOARDINTERACTIVE:
				{
					CKeyboardInteractiveRequestData *pData = reinterpret_cast<CKeyboardInteractiveRequestData *>(data.pData);
					CEnterSomething dlg(IDS_INPUTDIALOGTITLE_KEYBOARDINTERACTIVE, pData->data, '*');
					::EndMenu();
					if (dlg.DoModal() == IDOK)
					{
						USES_CONVERSION;
						const char *lpszAscii = T2CA(dlg.m_String);
						strncpy(pData->data, lpszAscii, 20479);
						pData->data[20479] = 0;
						SendReply(1, data.pFileZillaApi, pData);
					}
					else
						SendReply(0, data.pFileZillaApi, pData);
				}
		}

		//Check if any requests can be processed without a dialog
		std::list<t_QueueItem>::iterator last = std::remove_if(m_RequestList.begin(), m_RequestList.end(), TryProcess);
		m_RequestList.erase(last, m_RequestList.end());
	}
	m_bActive = FALSE;
}

BOOL CAsyncRequestQueue::SendReply(int nAction, CFileZillaApi *pFileZillaApi, CAsyncRequestData *pData)
{
	if (!pData)
		return FALSE;
	if (!pFileZillaApi->IsValid()) //Make sure data.pFileZillaApi still exists
		return FALSE;
	
	return pFileZillaApi->SetAsyncRequestResult(nAction, pData) == FZ_REPLY_OK;
}

bool CAsyncRequestQueue::TryProcess(const t_QueueItem & data)
{
	switch (data.pData->nRequestType)
	{
		case FZ_ASYNCREQUEST_OVERWRITE:
			{
				COverwriteRequestData *pFileExistData = reinterpret_cast<COverwriteRequestData *>(data.pData);
				if (CFileExistsDlg::ProcessRequest(pFileExistData, data.pFileZillaApi, TRUE))
					return true;
			}
			break;
		case FZ_ASYNCREQUEST_NEWHOSTKEY:
			if (data.bFromQueue)
			{
				CNewHostKeyRequestData *pHostKeyData = reinterpret_cast<CNewHostKeyRequestData *>(data.pData);
				if (m_HostKeys.find(pHostKeyData->Hostkey) != m_HostKeys.end())
				{
					SendReply(0, data.pFileZillaApi, pHostKeyData);
					return true;
				}
			}
			break;
		case FZ_ASYNCREQUEST_CHANGEDHOSTKEY:
			if (data.bFromQueue)
			{
				CChangedHostKeyRequestData *pHostKeyData = reinterpret_cast<CChangedHostKeyRequestData *>(data.pData);
				if (m_HostKeys.find(pHostKeyData->Hostkey) != m_HostKeys.end())
				{
					SendReply(0, data.pFileZillaApi, pHostKeyData);
					return true;
				}
			}
			break;
		case FZ_ASYNCREQUEST_VERIFYCERT:
			{
				CVerifyCertRequestData *pVerifyCertData = reinterpret_cast<CVerifyCertRequestData *>(data.pData);
				ASSERT(pVerifyCertData->pCertData);

				if (data.bFromQueue)
				{
					std::list<t_SslCertHash>::iterator iter;
					for (iter = m_SessionCertHash.begin(); iter != m_SessionCertHash.end(); iter++)
						if (!memcmp(iter->hash, pVerifyCertData->pCertData->hash, 20))
						{
							SendReply(1, data.pFileZillaApi, pVerifyCertData);
							return true;
						}
				}
				if (CVerifyCertDlg::IsHashTrusted((char*)pVerifyCertData->pCertData->hash))
				{
					SendReply(1, data.pFileZillaApi, pVerifyCertData);
					return true;
				}
			}
			break;
		}
	return false;
}