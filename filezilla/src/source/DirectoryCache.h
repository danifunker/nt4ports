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

//CDirectoryCache
//This thread-safe class caches directory entries of FTP servers.
//Multiple instances of this class share the same cache, cache will be destroyed when 
//last instance will be destroyed
//Copyright 2001/2002 Tim Kosse (Tim.Kosse@gmx.de)

#if !defined(AFX_DIRECTORYCACHE_H__74B6B2DA_16C5_4458_BC9D_EA8167D27719__INCLUDED_)
#define AFX_DIRECTORYCACHE_H__74B6B2DA_16C5_4458_BC9D_EA8167D27719__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDirectoryCache  
{
public:
	CDirectoryCache();
	virtual ~CDirectoryCache();
	
	void Store(const t_directory &directory, BOOL bModifiedEntry = FALSE); //Store a directory
	
	void Store(const t_directory &directory,const CServerPath &parent,CString dirname, BOOL bModifiedEntry = FALSE); 
	//Store a directory, parent path and relative name given
	
	BOOL Lookup(const CServerPath &path,const t_server &server,t_directory &directory, BOOL bIgnoreSettings = FALSE);
	//Looks up a directory witch the full path specified
	
	BOOL Lookup(const CServerPath &parent,CString dirname,const t_server server,t_directory &directory, BOOL bIgnoreSettings = FALSE);
	//Looks up a directory specified by parent path and relative path 

	void Purge(const CServerPath &path, const t_server &Server);
	//Removes old entries from cache

	//Locking functions
	void Lock();
	void Unlock();

	//Directory manipulation routines
	void Rename(const CServerPath &path, CString oldName, CString newName, const t_server &server);

	BOOL Dump(LPCTSTR pFileName);
	//Dumps the contents of the cache into the specified file

protected:
	class CCacheentry
	{
	public:
		t_directory directory;
		CTime createtime;
		typedef struct t_parents
		{
			CServerPath parent;
			CString dirname;
		} t_parents;
		std::list<t_parents*> parents;
	};
	
	typedef std::list<CCacheentry::t_parents *>::iterator tParentsIter;
	typedef std::list<CCacheentry*>::iterator tCacheIter;

	static std::list<CCacheentry*> m_CacheList;
	static int m_nRefCount;
	static CCriticalSection m_Sync;
	int m_nLockAquiredCount;
};

#endif // !defined(AFX_DIRECTORYCACHE_H__74B6B2DA_16C5_4458_BC9D_EA8167D27719__INCLUDED_)
