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

// DirectoryCache.cpp: Implementierung der Klasse CDirectoryCache.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirectoryCache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
std::list<CDirectoryCache::CCacheentry*> CDirectoryCache::m_CacheList;
int CDirectoryCache::m_nRefCount=0;
CCriticalSection CDirectoryCache::m_Sync;

CDirectoryCache::CDirectoryCache()
{
	m_nLockAquiredCount=0;
	Lock();
	m_nRefCount++;
	Unlock();
}

CDirectoryCache::~CDirectoryCache()
{
	Lock();
	m_nRefCount--;
	if (!m_nRefCount)
	{
		for (tCacheIter iter=m_CacheList.begin(); iter!=m_CacheList.end(); iter++)
		{
			CCacheentry *entry=*iter;
			for (tParentsIter parentsIter=entry->parents.begin(); parentsIter!=entry->parents.end(); parentsIter++)
				delete *parentsIter;
			entry->parents.clear();
			delete entry;
		}	
		m_CacheList.clear();
	}
	Unlock();
}

BOOL CDirectoryCache::Lookup(const CServerPath &path, const t_server &server, t_directory &directory, BOOL bIgnoreSettings /*=FALSE*/)
{
	if (!COptions::GetOptionVal(OPTION_USECACHE) && !bIgnoreSettings)
		return FALSE;
	Lock();
	for (tCacheIter iter=m_CacheList.begin(); iter!=m_CacheList.end(); iter++)
	{
		CCacheentry *entry=*iter;
		if (entry->directory.server==server && entry->directory.path==path)
		{
			CTimeSpan span=CTime::GetCurrentTime()-entry->createtime;
			if (span.GetTotalSeconds()>COptions::GetOptionVal(OPTION_MAXCACHETIME))
			{
				for (tParentsIter parentsIter=entry->parents.begin(); parentsIter!=entry->parents.end(); parentsIter++)
				delete *parentsIter;
				entry->parents.clear();
				delete entry;
				m_CacheList.erase(iter);
				Unlock();
				return FALSE;
			}
			
			directory=entry->directory;
			Unlock();
			directory.bCached=TRUE;
			return TRUE;
		}
	}
	Unlock();
	return FALSE;
}

BOOL CDirectoryCache::Lookup(const CServerPath &parent,CString dirname,const t_server server,t_directory &directory, BOOL bIgnoreSettings /*=FALSE*/)
{
	if (!COptions::GetOptionVal(OPTION_USECACHE) && !bIgnoreSettings)
		return FALSE;
	Lock();
	if (dirname!="..")
	{
		for (tCacheIter iter=m_CacheList.begin(); iter!=m_CacheList.end(); iter++)
		{
			CCacheentry *entry=*iter;
			if (entry->directory.server==server)
			{
				for (tParentsIter parentsIter=entry->parents.begin(); parentsIter!=entry->parents.end(); parentsIter++)
				{
					CCacheentry::t_parents *pParent=*parentsIter;
					if (pParent->parent==parent && pParent->dirname==dirname)
					{
						CTimeSpan span=CTime::GetCurrentTime()-entry->createtime;
						if (span.GetTotalSeconds()>COptions::GetOptionVal(OPTION_MAXCACHETIME))
						{
							for (tParentsIter parentsIter=entry->parents.begin(); parentsIter!=entry->parents.end(); parentsIter++)
								delete *parentsIter;
							entry->parents.clear();
							delete entry;
							m_CacheList.erase(iter);
							Unlock();
							return FALSE;
						}
						directory=entry->directory;
						Unlock();
						directory.bCached=TRUE;
						return TRUE;
					}
				}
			}
		}
	}
	else
	{
		//Search for own entry
		t_directory tmp;
		CCacheentry *entry=0;
		for (tCacheIter iter=m_CacheList.begin(); iter!=m_CacheList.end(); iter++)
		{
			entry=*iter;
			if (entry->directory.server==server && entry->directory.path==parent)
			{
				tmp=entry->directory;
				break;
			}
			entry=0;
		}
		if (!entry)
		{
			Unlock();
			return FALSE; //Own directory not cached
		}
		if (entry->parents.empty())
		{
			Unlock();
			return FALSE; //Index to last used parent not cached
		}

		tParentsIter parentsIter;
		for (parentsIter=entry->parents.begin(); parentsIter!=entry->parents.end(); parentsIter++)
			if ((*parentsIter)->dirname!="..")
				break;
		if (parentsIter==entry->parents.end())
		{
			Unlock();
			return FALSE;
		}

		BOOL res=Lookup((*parentsIter)->parent, server, tmp);
		if (!res) //Parent directory not cached
		{
			Unlock();
			return FALSE;
		}
		directory=tmp;
		Unlock();
		directory.bCached=TRUE;
		return TRUE;
	}
	Unlock();
	return FALSE;
}

void CDirectoryCache::Store(const t_directory &directory, BOOL bModifiedEntry /*=FALSE*/)
{
	Lock();
	t_directory tmp;
	for (tCacheIter iter=m_CacheList.begin(); iter!=m_CacheList.end(); iter++)
	{
		CCacheentry *entry=*iter;
		if (entry->directory.server==directory.server && entry->directory.path==directory.path)
		{
			if (!bModifiedEntry)
				entry->createtime=CTime::GetCurrentTime(); //Set time only for new entries
			entry->directory=directory;
			Unlock();
			return;
		}
	}
	//If modified entry is not in cache, don't store new one
	if (!bModifiedEntry)
	{
		CCacheentry *entry=new CCacheentry;
		entry->createtime=CTime::GetCurrentTime();
		entry->directory=directory;
		m_CacheList.push_front(entry);
	}
	Unlock();
}

void CDirectoryCache::Store(const t_directory &directory,const CServerPath &parent,CString dirname, BOOL bModifiedEntry /*=FALSE*/)
{
	Lock();
	t_directory tmp;
	for (tCacheIter iter=m_CacheList.begin(); iter!=m_CacheList.end(); iter++)
	{
		CCacheentry *entry=*iter;
		if (entry->directory.server==directory.server && entry->directory.path==directory.path)
		{
			if (!bModifiedEntry)
				entry->createtime=CTime::GetCurrentTime();
			entry->directory=directory;
			
			//Search if parent already exists
			for (tParentsIter parentsIter=entry->parents.begin(); parentsIter!=entry->parents.end(); parentsIter++)
			{
				CCacheentry::t_parents *pParent=*parentsIter;
				if (pParent->parent==parent && pParent->dirname==dirname)
				{
					Unlock();
					return;
				}
			}
			//Store parent
			CCacheentry::t_parents *pParent=new CCacheentry::t_parents;
			pParent->parent=parent;
			pParent->dirname=dirname;
			entry->parents.push_front(pParent);
			Unlock();
			return;
		}
	}
	if (!bModifiedEntry)
	{			
		CCacheentry *entry=new CCacheentry;
		entry->createtime=CTime::GetCurrentTime();
		entry->directory=directory;
	
		//Store parent
		CCacheentry::t_parents *pParent=new CCacheentry::t_parents;
		pParent->parent=parent;
		pParent->dirname=dirname;
		entry->parents.push_front(pParent);
		m_CacheList.push_front(entry);
	}
	Unlock();
}

void CDirectoryCache::Purge(const CServerPath &path, const t_server &Server)
{
	//Remove entry in physical parent
	if (path.HasParent())
	{
		CServerPath path2=path.GetParent();
		CString name=path.GetLastSegment();
		t_directory dir;
		BOOL res=Lookup(path2,Server,dir);
		if (res)
		{
			BOOL found=FALSE;
			for (int i=0;i<dir.num;i++)
			{
				if (dir.direntry[i].name==name)
				{
					ASSERT(dir.direntry[i].dir);
					found=TRUE;
					break;
				}
			}
			if (found)
			{
				t_directory::t_direntry *direntry=new t_directory::t_direntry[dir.num-1];
				int j=0;
				for (int i=0;i<dir.num;i++)
				{
					if (dir.direntry[i].name==name)
						continue;
					direntry[j]=dir.direntry[i];
					j++;
				}
				delete [] dir.direntry;
				dir.direntry=direntry;
				dir.num--;
				Store(dir,TRUE);
			}
		}
	}
	//Remove path from cache
	Lock();
	for (tCacheIter iter=m_CacheList.begin(); iter!=m_CacheList.end(); iter++)
	{
		CCacheentry *entry=*iter;
		if (entry->directory.server==Server && entry->directory.path==path)
		{
			m_CacheList.erase(iter);
			//Remove entries in all logical parents
			tParentsIter parentsIter;
			for (parentsIter=entry->parents.begin(); parentsIter!=entry->parents.end(); parentsIter++)
			{
				CCacheentry::t_parents *parents=*parentsIter;
				if (parents->dirname!="..")
				{
					t_directory pdir;
					if (Lookup(parents->parent,Server,pdir))
					{
						BOOL found=FALSE;
						for (int i=0;i<pdir.num;i++)
						{
							if (pdir.direntry[i].name==parents->dirname)
							{
								ASSERT(pdir.direntry[i].dir);
								found=TRUE;
								break;
							}
						}
						if (found)
						{
							t_directory::t_direntry *direntry=new t_directory::t_direntry[pdir.num-1];
							int j=0;
							for (int i=0;i<pdir.num;i++)
							{
								if (pdir.direntry[i].name==parents->dirname)
									continue;
								direntry[j]=pdir.direntry[i];
								j++;
							}
							delete [] pdir.direntry;
							pdir.direntry=direntry;
							pdir.num--;
							Store(pdir,TRUE);
						}
					}
				}
			}
			
			std::list<CServerPath> childrenToPurge;

			//Search for dirs which have the purged dir as parent and remove any references
			for (tCacheIter iter2=m_CacheList.begin(); iter2!=m_CacheList.end(); iter2++)
			{
				CCacheentry *entry=*iter2;
				for (tParentsIter parentsIter = entry->parents.begin(); parentsIter != entry->parents.end(); parentsIter++)
				{
					CCacheentry::t_parents *parents=*parentsIter;
					if (parents->parent==path)
					{
						entry->parents.erase(parentsIter);
						delete parents;

						childrenToPurge.push_back(entry->directory.path);
						break;
					}
				}	
			}
			
			//Free memory
			for (parentsIter =entry->parents.begin(); parentsIter!=entry->parents.end(); parentsIter++)
				delete *parentsIter;
			entry->parents.clear();
			delete entry;
			
			// Purge all children
			for (std::list<CServerPath>::iterator citer = childrenToPurge.begin(); citer != childrenToPurge.end(); citer++)
			{
				Purge(*citer, Server);
			}
			
			break;
		}
	}
	Unlock();
}

void CDirectoryCache::Rename(const CServerPath &path, CString oldName, CString newName, const t_server &server)
{
	CServerPath oldpath = path;
	CServerPath newpath = path;
	VERIFY(oldpath.AddSubdir(oldName));
	VERIFY(newpath.AddSubdir(newName));

	//Go through all entries
	Lock();
	
	for (tCacheIter iter = m_CacheList.begin(); iter != m_CacheList.end(); iter++)
	{
		CCacheentry *entry=*iter;
		if (entry->directory.server == server)
		{
			if (entry->directory.path == oldpath)
				entry->directory.path = newpath;
			else if (entry->directory.path.IsSubdirOf(oldpath))
			{
				std::list<CString> tmp;
				while (entry->directory.path != oldpath)
				{
					ASSERT(entry->directory.path.HasParent());
					tmp.push_back(entry->directory.path.GetLastSegment());
					entry->directory.path=entry->directory.path.GetParent();
				}
				entry->directory.path=newpath;
				for (std::list<CString>::iterator iter=tmp.begin();iter!=tmp.end();iter++)
					VERIFY(entry->directory.path.AddSubdir(*iter));
			}
			for (tParentsIter parentsIter=entry->parents.begin(); parentsIter!=entry->parents.end(); parentsIter++)
			{
				CCacheentry::t_parents *pParent = *parentsIter;
				if (pParent->parent == oldpath)
					pParent->parent=newpath;
				else if (pParent->parent.IsSubdirOf(oldpath))
				{
					std::list<CString> tmp;
					while (pParent->parent != oldpath)
					{
						ASSERT(pParent->parent.HasParent());
						tmp.push_back(pParent->parent.GetLastSegment());
						pParent->parent = pParent->parent.GetParent();
					}
					pParent->parent = newpath;
					for (std::list<CString>::iterator iter = tmp.begin(); iter != tmp.end(); iter++)
						VERIFY(pParent->parent.AddSubdir(*iter));
				}				
				else if (pParent->parent == path && pParent->dirname == oldName)
					pParent->dirname = newName;
			}
		}
	}
	Unlock();
}


void CDirectoryCache::Lock()
{
	if (!m_nLockAquiredCount)
		m_Sync.Lock();
	m_nLockAquiredCount++;
}

void CDirectoryCache::Unlock()
{
	ASSERT(m_nLockAquiredCount>0);
	m_nLockAquiredCount--;
	if (!m_nLockAquiredCount)
		m_Sync.Unlock();
}

#define WriteLine(str) file.Write(CString(str)+_T("\r\n"), (_tcslen(str)+2)*sizeof(TCHAR))

BOOL CDirectoryCache::Dump(LPCTSTR pFileName)
{
	CFile file;
	if (!file.Open(pFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite, 0))
		return FALSE;
	Lock();
	TRY
	{
		WriteLine(_T("FileZilla Directory Cache Dump\r\n") );
		CString str;
		str.Format( _T("Dumping %d cached directories\r\n"), m_CacheList.size());
		WriteLine(str);
		int count=1;
		for (tCacheIter iter=m_CacheList.begin(); iter!=m_CacheList.end(); iter++, count++)
		{
			CCacheentry *pEntry=*iter;
			ASSERT(pEntry);
			str.Format( _T("Entry %d:\r\nPath: %s"), count, pEntry->directory.path.GetPath());
			WriteLine(str);
			if (!pEntry->parents.empty())
				WriteLine( _T("Parents:") );
			for (tParentsIter parentsIter=pEntry->parents.begin(); parentsIter!=pEntry->parents.end(); parentsIter++)
			{
				CCacheentry::t_parents *pParent=*parentsIter;
				str.Format( _T("  %s - %s"), pParent->dirname, pParent->parent.GetPath());
				WriteLine(str);
			}
			str.Format(_T("Server: %s@%s:%d, type: %d"), pEntry->directory.server.user, pEntry->directory.server.host, pEntry->directory.server.port, pEntry->directory.server.nServerType);
			WriteLine(str);
			str.Format(_T("Directory contains %d items:"), pEntry->directory.num);
			WriteLine(str);
			for (int i=0;i<pEntry->directory.num; i++)
			{
				const t_directory::t_direntry *pDirEntry=&pEntry->directory.direntry[i];

				CString str2 = _T("  ") + pDirEntry->permissionstr;
				while (str2.GetLength()<12)
					str2 +=  _T(" ");

				str2 += _T(" ") + pDirEntry->ownergroup;
				while (str2.GetLength()<25)
					str2 +=  _T(" ");
				
				CString datestr;
				if (pDirEntry->date.hasdate)
				{
					CString timestr;
					if (pDirEntry->date.hastime)
						timestr.Format( _T("%02d:%02d"), pDirEntry->date.hour, pDirEntry->date.minute);
					datestr.Format( _T("%4d-%02d-%02d %s"), pDirEntry->date.year, pDirEntry->date.month, pDirEntry->date.day, timestr);
				}

				str.Format( _T("%s %s%s %10I64d %16s"),
					str2,
					pDirEntry->dir?"D":" ", 
					pDirEntry->bUnsure?"U":" ", 
					pDirEntry->size,
					datestr);

				str += _T("  ")+pDirEntry->name;
				WriteLine(str);
			}
			WriteLine( _T("") );
		}
	}
	CATCH_ALL(e)
	{
		file.Close();
		Unlock();
		return FALSE;
	}
	END_CATCH_ALL
	Unlock();
	file.Close();
	return TRUE;
}