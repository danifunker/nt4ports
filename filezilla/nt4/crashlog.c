/* crashlog.c: when FileZilla faults, FileZilla-crash.txt beside FileZilla.exe gets the
   exception, the address it happened at, the return address register and the words on
   the stack that point into FileZilla.exe: with the linker's map (FileZilla.map), that is
   where it was and how it got there. Windows' own error box comes up after it, as before.
   For the RISC CPUs, where NT 4.0 has no debugger to hand. The log is written by a thread
   of its own, which waits for a fault from the start: a thread that has overflowed its
   stack has none left to write it with. Written for the NT 4.0 port.
   Licence: GPL v2 or later, as FileZilla. */
#include <windows.h>
#include <stdio.h>
#include <string.h>

static LPTOP_LEVEL_EXCEPTION_FILTER previous;
static HANDLE fault, written;
static EXCEPTION_POINTERS *faulted;
static DWORD faulted_thread;

static void write_log(EXCEPTION_POINTERS *ep)
{
	char path[MAX_PATH + 32], *slash;
	DWORD base, size, pc = 0, ra = 0, sp = 0, *p;
	IMAGE_NT_HEADERS *nt;
	CONTEXT *c = ep->ContextRecord;
	EXCEPTION_RECORD *er = ep->ExceptionRecord;
	SYSTEMTIME st;
	FILE *f;
	int n;

	GetModuleFileNameA(NULL, path, MAX_PATH);
	slash = strrchr(path, '\\');
	strcpy(slash ? slash + 1 : path, "FileZilla-crash.txt");
	f = fopen(path, "a");
	if (!f)
		return;

	base = (DWORD)GetModuleHandleA(NULL);
	nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
	size = nt->OptionalHeader.SizeOfImage;
#if defined(_M_PPC)
	pc = c->Iar; ra = c->Lr; sp = c->Gpr1;
#elif defined(_M_MRX000)
	pc = c->Fir; ra = c->IntRa; sp = c->IntSp;
#elif defined(_M_ALPHA)
	pc = (DWORD)c->Fir; ra = (DWORD)c->IntRa; sp = (DWORD)c->IntSp;
#elif defined(_M_ARM)
	pc = c->Pc; ra = c->Lr; sp = c->Sp;
#endif
	GetLocalTime(&st);
	fprintf(f, "FileZilla crashed %04d-%02d-%02d %02d:%02d:%02d, thread %lu\n",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, faulted_thread);
	fprintf(f, "exception %08lx at %08lx", er->ExceptionCode, (DWORD)er->ExceptionAddress);
	if (er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && er->NumberParameters >= 2)
		fprintf(f, " (%s %08lx)", er->ExceptionInformation[0] ? "writing" : "reading",
			(DWORD)er->ExceptionInformation[1]);
	fprintf(f, "\nimage %08lx-%08lx  pc %08lx  return address %08lx  sp %08lx\n",
		base, base + size, pc, ra, sp);
	fprintf(f, "stack words pointing into the image:\n");
	n = 0;
	for (p = (DWORD *)sp; p && n < 64 && p < (DWORD *)sp + 4096 && !IsBadReadPtr(p, 4); p++)
		if (*p > base + 0x1000 && *p < base + size) {
			fprintf(f, "  sp+%04lx: %08lx\n", (DWORD)((char *)p - (char *)sp), *p);
			n++;
		}
	fprintf(f, "\n");
	fclose(f);
}

static DWORD WINAPI writer(LPVOID unused)
{
	for (;;) {
		WaitForSingleObject(fault, INFINITE);
		write_log(faulted);
		SetEvent(written);
	}
}

static LONG WINAPI fz_crash_filter(EXCEPTION_POINTERS *ep)
{
	static LONG busy;
	/* One fault at a time; the writer has a minute. */
	if (InterlockedExchange(&busy, 1) == 0) {
		faulted = ep;
		faulted_thread = GetCurrentThreadId();
		SetEvent(fault);
		WaitForSingleObject(written, 60000);
		busy = 0;
	}
	return previous ? previous(ep) : EXCEPTION_CONTINUE_SEARCH;
}

void fz_install_crash_log(void)
{
	DWORD id;
	fault = CreateEvent(NULL, FALSE, FALSE, NULL);
	written = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (fault && written && CreateThread(NULL, 0, writer, NULL, 0, &id))
		previous = SetUnhandledExceptionFilter(fz_crash_filter);
}

/* For finding out how a program got somewhere (a debugging aid, called by hand): the same
   record as a fault's, of this place, and the program goes on. */
static LONG log_and_continue(EXCEPTION_POINTERS *ep)
{
	faulted = ep;
	faulted_thread = GetCurrentThreadId();
	write_log(ep);
	return EXCEPTION_EXECUTE_HANDLER;
}

void fz_log_here(void)
{
	typedef VOID (WINAPI *fn)(DWORD, DWORD, DWORD, CONST DWORD *);
	static fn raise;
	if (!raise)
		raise = (fn)GetProcAddress(GetModuleHandleA("kernel32.dll"), "RaiseException");
	__try {
		raise(0xE0F20000, 0, 0, NULL);
	} __except (log_and_continue(GetExceptionInformation())) {
	}
}
