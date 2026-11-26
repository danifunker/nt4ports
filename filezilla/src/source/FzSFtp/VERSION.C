/*
 * PuTTY version numbering
 */

#define STR1(x) #x
#define STR(x) STR1(x)

#if defined SNAPSHOT

char ver[] = "Development snapshot " STR(SNAPSHOT);
char sshver[] = "PuTTY-FZ-Snapshot-" STR(SNAPSHOT);

#elif defined RELEASE

char ver[] = "Release " STR(RELEASE);
char sshver[] = "PuTTY-FZ-Release-" STR(RELEASE);

#else

char ver[] = "Unidentified build, " __DATE__ " " __TIME__;
char sshver[] = "PuTTY-FZ-Local: " __DATE__ " " __TIME__;

#endif

/*
 * SSH local version string MUST be under 40 characters. Here's a
 * compile time assertion to verify this.
 */
enum { vorpal_sword = 1 / (sizeof(sshver) <= 40) };
