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
#include "LocalComboCompletion.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CLocalComboCompletion::CLocalComboCompletion()
{

}

CLocalComboCompletion::~CLocalComboCompletion()
{

}

void CLocalComboCompletion::MakeLong(CString &DirName)
{
	CFileFind find;
	BOOL found=find.FindFile(DirName+_T("\\."));
	if (found)
	{
		find.FindNextFile();
		DirName=find.GetFilePath();
	}
}

void CLocalComboCompletion::OnChangeDir(CString dir)
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
	dir.TrimLeft(_T(" "));
	dir.TrimRight(_T(" "));
	dir.TrimRight(_T("\\"));
	if (dir == _T(""))
	{
		pMainFrame->SetLocalFolder(dir);
		return;
	}
	if (dir.Right(1)==_T(":") )
	{
		CFileStatus64 status;
		if (GetStatus64(dir+"\\*.*", status))
		{
			pMainFrame->SetLocalFolder(dir);
			return;
		}
		else
		{
			AfxMessageBox(IDS_ERRORMSG_PATHNOTFOUND, MB_ICONEXCLAMATION);
			return;
		}
	}
	
	CFileStatus64 status;
	if (GetStatus64(dir, status))
	{
		if (!(status.m_attribute&0x10))
			AfxMessageBox(IDS_ERRORMSG_PATHNOTFOUND, MB_ICONEXCLAMATION);
		else
		{
			CMainFrame *pMainFrame=DYNAMIC_DOWNCAST(CMainFrame,GetParentFrame());
			MakeLong(dir);
			pMainFrame->SetLocalFolder(dir);
		}
	}
	else
		AfxMessageBox(IDS_ERRORMSG_PATHNOTFOUND, MB_ICONEXCLAMATION);
}
