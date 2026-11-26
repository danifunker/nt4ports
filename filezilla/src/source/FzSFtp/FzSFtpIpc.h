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

#include "..\SFtpCommandIDs.h"

#ifdef __cplusplus
extern "C" { 
#endif   //_cplusplus 

BOOL bLTrace;

BOOL FzSFtpIpc_SendRequest(const DWORD nID, const DWORD nLength, const LPVOID pData);
BOOL FzSFtpIpc_ReceiveRequest(DWORD *nID, DWORD *nLength, LPVOID pData);

BOOL FzSFtpIpc_Init(char *lpCmdLine);

BOOL FzSFtpIpc_Error(const char *msg);

#define FzSFtpIpc_Error1(str, i) { char tmp[1024]; sprintf(tmp, str, i); FzSFtpIpc_Error(tmp); }
#define FzSFtpIpc_Error2(str, i, j) { char tmp[1024]; sprintf(tmp, str, i, j); FzSFtpIpc_Error(tmp); }
#define FzSFtpIpc_Error3(str, i, j, k) { char tmp[1024]; sprintf(tmp, str, i, j, k); FzSFtpIpc_Error(tmp); }
#define FzSFtpIpc_Error4(str, i, j, k, l) { char tmp[1024]; sprintf(tmp, str, i, j, k, l); FzSFtpIpc_Error(tmp); }
#define FzSFtpIpc_Error5(str, i, j, k, l, m) { char tmp[1024]; sprintf(tmp, str, i, j, k, l, m); FzSFtpIpc_Error(tmp); }


BOOL FzSFtpIpc_CriticalError(const char *msg);

#define FzSFtpIpc_CriticalError1(str, i) { char tmp[1024]; sprintf(tmp, str, i); FzSFtpIpc_CriticalError(tmp); }
#define FzSFtpIpc_CriticalError2(str, i, j) { char tmp[1024]; sprintf(tmp, str, i, j); FzSFtpIpc_CriticalError(tmp); }
#define FzSFtpIpc_CriticalError3(str, i, j, k) { char tmp[1024]; sprintf(tmp, str, i, j, k); FzSFtpIpc_CriticalError(tmp); }
#define FzSFtpIpc_CriticalError4(str, i, j, k, l) { char tmp[1024]; sprintf(tmp, str, i, j, k, l); FzSFtpIpc_CriticalError(tmp); }
#define FzSFtpIpc_CriticalError5(str, i, j, k, l, m) { char tmp[1024]; sprintf(tmp, str, i, j, k, l, m); FzSFtpIpc_CriticalError(tmp); }


BOOL FzSFtpIpc_FatalError(const char *msg);

#define FzSFtpIpc_FatalError1(str, i) { char tmp[1024]; sprintf(tmp, str, i); FzSFtpIpc_FatalError(tmp); }
#define FzSFtpIpc_FatalError2(str, i, j) { char tmp[1024]; sprintf(tmp, str, i, j); FzSFtpIpc_FatalError(tmp); }
#define FzSFtpIpc_FatalError3(str, i, j, k) { char tmp[1024]; sprintf(tmp, str, i, j, k); FzSFtpIpc_FatalError(tmp); }
#define FzSFtpIpc_FatalError4(str, i, j, k, l) { char tmp[1024]; sprintf(tmp, str, i, j, k, l); FzSFtpIpc_FatalError(tmp); }
#define FzSFtpIpc_FatalError5(str, i, j, k, l, m) { char tmp[1024]; sprintf(tmp, str, i, j, k, l, m); FzSFtpIpc_FatalError(tmp); }


BOOL FzSFtpIpc_Status(const char *msg);

#define FzSFtpIpc_Status1(str, i) { char tmp[1024]; sprintf(tmp, str, i); FzSFtpIpc_Status(tmp); }
#define FzSFtpIpc_Status2(str, i, j) { char tmp[1024]; sprintf(tmp, str, i, j); FzSFtpIpc_Status(tmp); }
#define FzSFtpIpc_Status3(str, i, j, k) { char tmp[1024]; sprintf(tmp, str, i, j, k); FzSFtpIpc_Status(tmp); }
#define FzSFtpIpc_Status4(str, i, j, k, l) { char tmp[1024]; sprintf(tmp, str, i, j, k, l); FzSFtpIpc_Status(tmp); }
#define FzSFtpIpc_Status5(str, i, j, k, l, m) { char tmp[1024]; sprintf(tmp, str, i, j, k, l, m); FzSFtpIpc_Status(tmp); }


BOOL FzSFtpIpc_Trace(const char *msg);

#define FzSFtpIpc_Trace1(str, i) { char tmp[1024]; sprintf(tmp, str, i); FzSFtpIpc_Trace(tmp); }
#define FzSFtpIpc_Trace2(str, i, j) { char tmp[1024]; sprintf(tmp, str, i, j); FzSFtpIpc_Trace(tmp); }
#define FzSFtpIpc_Trace3(str, i, j, k) { char tmp[1024]; sprintf(tmp, str, i, j, k); FzSFtpIpc_Trace(tmp); }
#define FzSFtpIpc_Trace4(str, i, j, k, l) { char tmp[1024]; sprintf(tmp, str, i, j, k, l); FzSFtpIpc_Trace(tmp); }
#define FzSFtpIpc_Trace5(str, i, j, k, l, m) { char tmp[1024]; sprintf(tmp, str, i, j, k, l, m); FzSFtpIpc_Trace(tmp); }

#ifdef __cplusplus
}
#endif   //_cplusplus 
