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

#if !defined(AFX_LOCALCOMBOCOMPLETION_H__E07B117B_2FED_4CCD_A736_F37457E1F19A__INCLUDED_)
#define AFX_LOCALCOMBOCOMPLETION_H__E07B117B_2FED_4CCD_A736_F37457E1F19A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MISC\ComboCompletion.h"

class CLocalComboCompletion : public CComboCompletion  
{
public:
	virtual void OnChangeDir(CString dir);
	CLocalComboCompletion();
	virtual ~CLocalComboCompletion();
protected:
	void MakeLong(CString &DirName);

};

#endif // !defined(AFX_LOCALCOMBOCOMPLETION_H__E07B117B_2FED_4CCD_A736_F37457E1F19A__INCLUDED_)
