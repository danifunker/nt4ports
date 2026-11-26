/* idna.h for the NT 4.0 build of FileZilla, in place of GNU libidn: host names are
   passed on unchanged when they are ASCII, and refused otherwise (FileZilla then
   uses the name as typed). Written for this port: nt4/nt4compat.cpp. */
#ifndef FZ_NT4_IDNA_H
#define FZ_NT4_IDNA_H
#ifdef __cplusplus
extern "C" {
#endif
#define IDNA_ALLOW_UNASSIGNED 1
#define IDNA_SUCCESS 0
int idna_to_ascii_8z(const char *input, char **output, int flags);
#ifdef __cplusplus
}
#endif
#endif
