/* tmschema.h for the NT 4.0 build of FileZilla: the parts and states it draws, and the
   schema's declaration macros as misc/MinTrayBtn.cpp uses them. Written for this port;
   the values are those of the Windows XP theme schema. NT 4.0 never draws with them. */
#ifndef FZ_NT4_TMSCHEMA_H
#define FZ_NT4_TMSCHEMA_H
#define BP_PUSHBUTTON 1
#define PBS_NORMAL 1
#define PBS_HOT 2
#define PBS_PRESSED 3
#define PBS_DISABLED 4
#define PBS_DEFAULTED 5
#define CP_DROPDOWNBUTTON 1
#define CBXS_NORMAL 1
#define CBXS_HOT 2
#define CBXS_PRESSED 3
#define CBXS_DISABLED 4
#define WP_CAPTION 1
#define WP_MINBUTTON 15
#define WP_CLOSEBUTTON 18
#define MINBS_NORMAL 1
#define MINBS_HOT 2
#define MINBS_PUSHED 3
#define MINBS_DISABLED 4

#define BEGIN_TM_PART_STATES(name) enum name##STATES {
#define TM_STATE(val, prefix, name) prefix##_##name = val,
#define END_TM_PART_STATES() };
#endif
