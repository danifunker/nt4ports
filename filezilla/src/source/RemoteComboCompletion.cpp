// FileZilla - a Windows ftp client

// Copyright (C) 2003-2004 - Tim Kosse <tim.kosse@gmx.de>

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
#include "RemoteComboCompletion.h"
#include "MainFrm.h"
#include "CommandQueue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CRemoteComboCompletion::CRemoteComboCompletion()
{
	m_pServer = NULL;
	m_pPath = NULL;
}

CRemoteComboCompletion::~CRemoteComboCompletion()
{
	delete m_pServer;
	delete m_pPath;
}

void CRemoteComboCompletion::OnChangeDir(CString dir)
{
	if (m_pServer)
	{
		CServerPath path(*m_pPath);
		if (path.ChangePath(dir))
		{
			SetWindowText(path.GetPath());
			
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
			if (!pMainFrame->m_pCommandQueue->List(path))
			{
				SetWindowText(m_pPath->GetPath());
				MessageBeep(MB_ICONEXCLAMATION);
			}
		}
		else
		{
			SetWindowText(m_pPath->GetPath());
			MessageBeep(MB_ICONEXCLAMATION);
		}	
	}
}

void CRemoteComboCompletion::SetServer(const t_server &server)
{
	if (!m_pServer)
		m_pServer = new t_server;
	else
	{
		if (*m_pServer != server)
			ResetContent();
	}
	*m_pServer = server;					
}

void CRemoteComboCompletion::SetPath(const CServerPath &path)
{
	if (!m_pPath)
		m_pPath = new CServerPath;;
	*m_pPath = path;
}
