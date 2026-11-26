/* debughooks.c: a debugging aid, linked by make DEBUGHOOKS=1 only, never in a release.
   It logs the stack (crashlog.c's fz_log_here) of whatever ends the process or its message
   loop or raises an exception, by taking their import names as imports.c does; and it has
   fz_trace_msg and fz_trace_text, which write a line straight to C:\fz\msgtrace.txt, for
   tracing calls put in by hand. With them the PowerPC queue crash (QueueView.cpp) was
   found. Written for the NT 4.0 port. Licence: GPL v2 or later, as FileZilla. */
#include <windows.h>
void fz_log_here(void);

static VOID WINAPI hook_ExitProcess(UINT code)
{
	typedef VOID (WINAPI *fn)(UINT);
	fz_log_here();
	((fn)GetProcAddress(GetModuleHandleA("kernel32.dll"), "ExitProcess"))(code);
}

static VOID WINAPI hook_PostQuitMessage(int code)
{
	typedef VOID (WINAPI *fn)(int);
	fz_log_here();
	((fn)GetProcAddress(GetModuleHandleA("user32.dll"), "PostQuitMessage"))(code);
}

static BOOL WINAPI hook_TerminateProcess(HANDLE h, UINT code)
{
	typedef BOOL (WINAPI *fn)(HANDLE, UINT);
	fz_log_here();
	return ((fn)GetProcAddress(GetModuleHandleA("kernel32.dll"), "TerminateProcess"))(h, code);
}

static VOID WINAPI hook_RaiseException(DWORD code, DWORD flags, DWORD n, CONST DWORD *args)
{
	typedef VOID (WINAPI *fn)(DWORD, DWORD, DWORD, CONST DWORD *);
	static fn real;
	if (!real)
		real = (fn)GetProcAddress(GetModuleHandleA("kernel32.dll"), "RaiseException");
	if (code != 0xE0F20000)
		fz_log_here();
	real(code, flags, n, args);
}

void *__imp_RaiseException = (void *)hook_RaiseException;
void *__imp_TerminateProcess = (void *)hook_TerminateProcess;
void *__imp_ExitProcess = (void *)hook_ExitProcess;
void *__imp_PostQuitMessage = (void *)hook_PostQuitMessage;

/* Every window message MFC hands to a window, in and out, straight to the file: the
   system keeps what was written even when the process dies. */
void fz_trace_msg(char dir, const char *cls, HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HANDLE h;
	char line[200];
	DWORD n;
	if (!h)
		h = CreateFileA("C:\\fz\\msgtrace.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	n = wsprintfA(line, "%lu %c %s %08lx %04x %08lx %08lx\r\n", GetCurrentThreadId(), dir, cls,
		(DWORD)hwnd, msg, (DWORD)wp, (DWORD)lp);
	WriteFile(h, line, n, &n, NULL);
}

void fz_trace_text(const char *text)
{
	char line[400];
	lstrcpynA(line, text, 380);
	fz_trace_msg('L', line, 0, 0, 0, 0);
}
