s/#include "afxres.h"/\r\n\
#include \"winresrc.h\"\r\n\
#ifndef ID_APP_EXIT\r\n\
#define ID_APP_EXIT                     0xE141\r\n\
#endif\r\n\
#ifndef ID_VIEW_TOOLBAR\r\n\
#define ID_VIEW_TOOLBAR                 0xE800\r\n\
#endif\r\n\
#ifndef ID_VIEW_STATUS_BAR\r\n\
#define ID_VIEW_STATUS_BAR              0xE801\r\n\
#endif\r\n\
#ifndef ID_APP_ABOUT\r\n\
#define ID_APP_ABOUT                    0xE140\r\n\
#endif\r\n\
#ifndef AFX_IDS_APP_TITLE\r\n\
#define AFX_IDS_APP_TITLE               0xE000\r\n\
#endif\r\n\
#ifndef AFX_IDS_IDLEMESSAGE\r\n\
#define AFX_IDS_IDLEMESSAGE             0xE001\r\n\
#endif\r\n\
#ifndef ID_HELP\r\n\
#define ID_HELP                         0xE146\r\n\
#endif\r\n\
#ifndef IDC_STATIC\r\n\
#define IDC_STATIC -1\r\n\
#endif\r\n\
#ifndef AFX_IDS_SCCLOSE\r\n\
#define AFX_IDS_SCCLOSE                 0xEF06\r\n\
#endif\r\n\
/

s/^( +IDC_[A-Z_]+, 0x[0-9]+, [0-9]+, 0),?/\1,/
s/^((0x[0-9a-f]+, )+"\\000"),?/\1,/

:t
/^IDR_MAINFRAME TOOLBAR/,/^END/ {
  /END/!{
    $!{
      N;
      bt
    }
  }
  N;
  /#endif/!{
    s/^IDR.*END/#ifdef APSTUDIO_INVOKED\n\0\n#endif/;
  }
}    