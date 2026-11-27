/* winmm.dll for testing scummvm.exe in ntsdk's Alpha emulator, ntrun, which can't start the
 * real one: no sound, MIDI, CD or joystick device; timeGetTime from GetTickCount and
 * timeSetEvent by a thread (SDL's timer needs it). Not for a real NT machine. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define NODEV 6 /* MMSYSERR_NODRIVER */
unsigned long joyGetDevCapsA(void) { return 165; }
unsigned long joyGetNumDevs(void) { return 0; }
unsigned long joyGetPosEx(void) { return 165; }
int mciGetErrorStringA(unsigned long e, char *s, unsigned n) { if (n) s[0] = 0; return 0; }
unsigned long mciSendCommandA(void) { return 263; }
unsigned long midiOutClose(void) { return NODEV; }
unsigned long midiOutGetDevCapsA(void) { return NODEV; }
unsigned long midiOutGetErrorTextA(unsigned long e, char *s, unsigned n) { if (n) s[0] = 0; return 0; }
unsigned long midiOutGetNumDevs(void) { return 0; }
unsigned long midiOutLongMsg(void) { return NODEV; }
unsigned long midiOutOpen(void) { return NODEV; }
unsigned long midiOutPrepareHeader(void) { return NODEV; }
unsigned long midiOutShortMsg(void) { return NODEV; }
unsigned long midiOutUnprepareHeader(void) { return NODEV; }
unsigned long timeBeginPeriod(unsigned u) { return 0; }
unsigned long timeEndPeriod(unsigned u) { return 0; }
unsigned long timeGetTime(void) { return GetTickCount(); }
typedef void (CALLBACK *TCB)(UINT, UINT, DWORD, DWORD, DWORD);
struct tev { UINT delay; TCB proc; DWORD user; UINT flags; volatile LONG stop; };
static struct tev evs[16];
static DWORD WINAPI tthread(LPVOID p) {
	struct tev *e = (struct tev *)p;
	UINT id = (UINT)(e - evs) + 1;
	do {
		Sleep(e->delay);
		if (e->stop) break;
		e->proc(id, 0, e->user, 0, 0);
	} while (e->flags & 1);
	return 0;
}
unsigned long timeSetEvent(UINT delay, UINT res, TCB proc, DWORD user, UINT flags) {
	int i; DWORD tid;
	for (i = 0; i < 16; i++) if (!evs[i].proc) break;
	if (i == 16) return 0;
	evs[i].delay = delay; evs[i].proc = proc; evs[i].user = user; evs[i].flags = flags; evs[i].stop = 0;
	if (!CreateThread(NULL, 0, tthread, &evs[i], 0, &tid)) { evs[i].proc = 0; return 0; }
	return i + 1;
}
unsigned long timeKillEvent(UINT id) { if (id >= 1 && id <= 16) { evs[id - 1].stop = 1; evs[id - 1].proc = 0; } return 0; }
unsigned long waveOutClose(void) { return NODEV; }
unsigned long waveOutGetErrorTextA(unsigned long e, char *s, unsigned n) { if (n) s[0] = 0; return 0; }
unsigned long waveOutOpen(void) { return NODEV; }
unsigned long waveOutPrepareHeader(void) { return NODEV; }
unsigned long waveOutUnprepareHeader(void) { return NODEV; }
unsigned long waveOutWrite(void) { return NODEV; }
BOOL WINAPI DllMain(HINSTANCE h, DWORD r, LPVOID p) { return TRUE; }
