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

#if !defined(AFX_ASYNCREQUESTQUEUE_H__CC573525_CC1A_469B_8797_D2F852582D9F__INCLUDED_)
#define AFX_ASYNCREQUESTQUEUE_H__CC573525_CC1A_469B_8797_D2F852582D9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAsyncRequestQueue  
{
public:
	static void ProcessRequest(CAsyncRequestData *pData, CFileZillaApi  *pFileZillaApi, BOOL bFromQueue);
	static BOOL m_bActive;

protected:
	struct t_QueueItem
	{
		CAsyncRequestData *pData;
		CFileZillaApi *pFileZillaApi;
		BOOL bFromQueue;
	};

	static BOOL SendReply(int nAction, CFileZillaApi *pFileZillaApi, CAsyncRequestData *pData);
	static bool TryProcess(const t_QueueItem & data);

	static std::list<t_QueueItem> m_RequestList;
	static std::set<CString> m_HostKeys;

	struct t_SslCertHash
	{
		char hash[20];
	};
	static std::list<t_SslCertHash> m_SessionCertHash;
};

#endif // !defined(AFX_ASYNCREQUESTQUEUE_H__CC573525_CC1A_469B_8797_D2F852582D9F__INCLUDED_)
