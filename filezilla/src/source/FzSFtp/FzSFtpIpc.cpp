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

#include "putty.h"
#include "assert.h"
#include "FzSftpIpc.h"
#include "FzSftpIpcClient.h"

static CFzSFtpIpcClient m_DataChannel;

#ifdef __cplusplus
extern "C" { 
#endif   //_cplusplus 


BOOL FzSFtpIpc_Init(char *lpCmdLine)
{
	#define NUMARGS 7

	//Check how this program was started!!
	_int64 tmp[NUMARGS];
	int nCount=0;
	int i;
	DWORD dwFlags;
	
	//Get number of arguments and convert them into numbers
	//There have to be 7 args...
	LPSTR pos=strstr(lpCmdLine, " ");
	while (pos)
	{
		if ( nCount >= (NUMARGS-1) )
		{
			nCount=-1;
			break;
		}
		*pos=0;
		tmp[nCount++]=_atoi64(lpCmdLine);
		lpCmdLine=pos+1;
		pos=strstr(lpCmdLine, " ");
	}
	if (nCount==(NUMARGS-1) )
		tmp[nCount++]=_atoi64(lpCmdLine);
	else if (nCount!=NUMARGS)
		return FALSE;
	
	//Check each value if it's a valid handle
	for (i=0;i<NUMARGS;i++)
	{
		if (tmp[i]<=0 || tmp[i]>=0xFFFFFFFF || (!GetHandleInformation((HANDLE)tmp[i], &dwFlags) && GetLastError()!=ERROR_CALL_NOT_IMPLEMENTED))
			return FALSE;
	}

	if (!m_DataChannel.Init(tmp))
		return FALSE;

	return TRUE;
}

BOOL FzSFtpIpc_SendRequest(DWORD nID, DWORD nLength, const LPVOID pData)
{
	assert(nID);
	if (!m_DataChannel.SendRequest(nID, nLength, pData))
		cleanup_exit(1);
	return TRUE;
}

BOOL FzSFtpIpc_ReceiveRequest(DWORD *nID, DWORD *nLength, LPVOID pData)
{
	assert(nID && nLength && pData);
	if (!m_DataChannel.ReceiveRequest(*nID, *nLength, pData))
		cleanup_exit(1);
	return TRUE;
}

BOOL FzSFtpIpc_Trace(const char *msg)
{
	if (!bLTrace)
		return TRUE;
	assert(msg);
	if (!m_DataChannel.SendRequest(SFTP_DATAID_CTS_TRACE, strlen(msg)+1, (const LPVOID)msg))
		cleanup_exit(1);
	return TRUE;
}

BOOL FzSFtpIpc_Error(const char *msg)
{
	assert(msg);
	if (!m_DataChannel.SendRequest(SFTP_DATAID_CTS_ERROR, strlen(msg)+1, (const LPVOID)msg))
		cleanup_exit(1);
	return TRUE;
}

BOOL FzSFtpIpc_CriticalError(const char *msg)
{
	assert(msg);
	if (!m_DataChannel.SendRequest(SFTP_DATAID_CTS_CRITICALERROR, strlen(msg)+1, (const LPVOID)msg))
		cleanup_exit(1);
	return TRUE;
}

BOOL FzSFtpIpc_FatalError(const char *msg)
{
	assert(msg);
	if (!m_DataChannel.SendRequest(SFTP_DATAID_CTS_FATALERROR, strlen(msg)+1, (const LPVOID)msg))
		cleanup_exit(1);
	return TRUE;
}

BOOL FzSFtpIpc_Status(const char *msg)
{
	assert(msg);
	if (!m_DataChannel.SendRequest(SFTP_DATAID_CTS_STATUS, strlen(msg)+1, (const LPVOID)msg))
		cleanup_exit(1);
	return TRUE;
}

#ifdef __cplusplus
}
#endif   //_cplusplus 
