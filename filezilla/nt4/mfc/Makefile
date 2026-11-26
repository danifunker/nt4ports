# Visual C++ 6.0's MFC, built as nafxcw.lib (static, release, MBCS, multithreaded) for a
# CPU it was never built for. Run by nt4/mfc/build-mfc.sh inside the SDK container:
#   make -f /fz/nt4/mfc/Makefile CPU=mips|ppc|arm M=/mfc
# M is the folder that nt4/mfc/prepare.py made. The objects and lib/nafxcw.lib go in M.
# The objects are those of MFC's own makefile for NAFXCW, less the database classes, the
# OLE controls and their container (_AFX_NO_OCC_SUPPORT) and the DLL-only ones.

N    := /fz/nt4
M    ?= /mfc
S    := $(M)/src
O    := $(M)/obj
CC   := cl
LIB_ := lib

OBJS := objcore except validadd dumpcont dumpflt arccore arcobj arcex arcstrm \
  dumpinit dumpout afxasert afxmem afxabort \
  filecore filetxt filemem fileshrd filex filest \
  array_b array_d array_p array_o array_s array_u array_w list_o list_p list_s \
  map_pp map_pw map_so map_sp map_ss map_wo map_wp plex \
  strcore strex timecore afxdbcs afxstate afxtls fixalloc mtcore mtex \
  wincore winfrm winfrm2 winfrmx winmdi tooltip winmini winhand winmain barcore bartool \
  bardlg barstat bardock dockcont dockstat dcprev dcmeta trckrect barcool \
  winctrl1 winctrl2 winctrl3 winctrl4 winbtn dlgcore dlgdata dlgfloat dlgprop dlgcomm \
  dlgfile dlgprnt dlgclr dlgfnt dlgfr ccdata dlgtempl winctrl6 winctrl7 \
  wingdi wingdix winstr winmenu auxdata afxcrit afxtrace winutil winocc \
  cmdtarg doccore doctempl docsingl docmulti docmgr viewcore viewprnt winsplit viewscrl \
  viewform viewedit viewprev viewcmn docmapi \
  filefind \
  thrdcore appcore appinit appterm appui appui1 appui2 appui3 appgray appdlg app3d appprnt \
  apphelp apphelpx filelist app3ds nolib appmodul oleexp \
  sockcore olelock \
  oleinit olecli1 olecli2 olecli3 olecnvrt oledobj1 oledobj2 oledisp1 oledisp2 oledlgs1 \
  oledlgs2 oledlgs3 oledata olevar olevar1 oledoc1 oledoc2 oledrop1 oledrop2 olemsgf \
  oleenum olefact oleipfrm olelink olemisc olestrm olesvr1 olesvr2 olereg oletsvr oleui1 \
  oleui2 oleunk oleverb olecall viewrich oledll oletyplb olemon winctrl5
OBJS := $(OBJS:%=$(O)/%.obj)

DEFS := -DWIN32 -D_WINDOWS -DNDEBUG -D_MBCS -D_MT -D_WIN32_WINNT=0x0400 -DWINVER=0x0400 \
        -D_WIN32_IE=0x0400 -D_AFX_NO_OLEDB_SUPPORT -D_AFX_NO_OCC_SUPPORT -D_AFX_NO_DHTML_SUPPORT \
        -D_AFX_NO_DAO_SUPPORT
ifeq ($(CPU),mips)
DEFS += -D_MIPS_ -FI$(N)/include/fz_vc4.h
endif
ifeq ($(CPU),ppc)
DEFS += -D_PPC_ -FI$(N)/include/fz_vc4.h
endif
EH   := -GX
ifeq ($(CPU),arm)
DEFS += -D_ARM_ -FI$(N)/mfc/arm-ie4.h
# Microsoft C 19 as a compiler of 1998, as PiXPSP1 builds MFC: the for scope, wchar_t and
# implicit int of then, and nothing the PINT SDK's older C library lacks (security
# cookies, thread-safe statics, sized delete).
DEFS += -Zc:forScope- -Zc:wchar_t- -wd4430 -GS- -GR- -Zc:sizedDealloc- -Zc:threadSafeInit- \
        -Zc:tlsGuards- -Zc:twoPhase-
EH   := -EHsc
endif
CFLAGS := -nologo -c -W1 -O1 -Gy $(EH) -Zl -MT $(DEFS) -I$(S)

all: $(M)/lib/nafxcw.lib

$(M)/lib/nafxcw.lib: $(wildcard $(OBJS))
	mkdir -p $(M)/lib
	rm -f $@
	$(LIB_) -nologo -out:$@ $^

# Every object that compiles goes in: one that fails is reported (make -k) and left out,
# and the link of FileZilla says whether it was needed.
objs: $(OBJS)

# The precompiled header, made with objcore.cpp as MFC's makefile makes it.
$(O)/objcore.obj: $(S)/objcore.cpp
	@mkdir -p $(O)
	$(CC) $(CFLAGS) -Ycstdafx.h -Fp$(O)/stdafx.pch -Fo$(O)/objcore.obj $<

$(O)/%.obj: $(S)/%.cpp $(O)/objcore.obj
	$(CC) $(CFLAGS) -Yustdafx.h -Fp$(O)/stdafx.pch -Fo$@ $<

.PHONY: all objs
