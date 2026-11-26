# FileZilla 2.2.32 for Windows NT 4.0: GNU make over Microsoft's own tools.
# Run by nt4/build.sh inside the SDK container, never by hand:
#   make -f /fz/nt4/Makefile CPU=alpha|mips|ppc|arm O=/out
# /fz is the filezilla folder of the repository, O the output folder.
# The compiler, INCLUDE and LIB come from the container's environment (build.sh sets them).

FZ   := /fz
S    := $(FZ)/src/source
SF   := $(S)/FzSFtp
Z    := $(FZ)/zlib
N    := $(FZ)/nt4
O    ?= /out

CC   := cl
LD   := link
LIB_ := lib
RC   ?= rc

# Alpha: Visual C++ 6.0 for Alpha, its own MFC and STL. MIPS and PowerPC: Visual C++ 4.1
# with MFC 6 built by nt4/mfc and the STL of nt4/stl; ARM: the PINT SDK, the same.
# The MFC built here has no OLE control container and no DHTML, so FileZilla is compiled
# without them too, for the same class layouts.
COMMON := -nologo -W1 -DWIN32 -D_WINDOWS -DNDEBUG -D_WIN32_WINNT=0x0400 -DWINVER=0x0400 \
          -D_WIN32_IE=0x0400 -DFZ_NT4 -I$(N)/include
ifeq ($(CPU),alpha)
COMMON += -D_ALPHA_
# Visual C++ 6.0 for Alpha has the 64-bit integer functions in its library but leaves
# _INTEGRAL_MAX_BITS undefined, which hides them in stdlib.h.
COMMON += -D_INTEGRAL_MAX_BITS=64
else
COMMON += -I$(N)/stl -D_AFX_NO_OCC_SUPPORT -D_AFX_NO_DHTML_SUPPORT
endif
ifeq ($(CPU),mips)
COMMON += -D_MIPS_ -FI$(N)/include/fz_vc4.h
endif
ifeq ($(CPU),ppc)
COMMON += -D_PPC_ -FI$(N)/include/fz_vc4.h
endif
EH := -GX
ifeq ($(CPU),arm)
# Microsoft C 19 as a compiler of 1998 (as nt4/mfc/Makefile builds MFC), with the
# Internet Explorer 4 headers MFC 6 needs (nt4/mfc/arm-ie4.h).
COMMON += -D_ARM_ -FI$(N)/mfc/arm-ie4.h -Zc:forScope- -Zc:wchar_t- -wd4430 -GS- -GR- \
          -Zc:sizedDealloc- -Zc:threadSafeInit- -Zc:tlsGuards- -Zc:twoPhase-
EH := -EHsc
endif
COPT   ?= -O1
CFLAGS := $(COMMON) $(COPT) -MT
CXXFLAGS := $(CFLAGS) $(EH) -DFILEZILLA -D_MBCS -I$(S) -I$(S)/misc -I$(Z)

ZOBJ := adler32 compress crc32 deflate infback inffast inflate inftrees trees uncompr zutil
ZOBJ := $(ZOBJ:%=$(O)/z/%.obj)

FZSRC := ApiLog AsyncGssSocketLayer AsyncProxySocketLayer AsyncRequestQueue AsyncSocketEx \
  AsyncSocketExLayer AsyncSslSocketLayer CommandQueue ControlSocket Crypt DirTreeCtrl \
  DirectoryCache EnterSomething FileAttributes FileChangeNotify FileExistsDlg FileZilla \
  FileZillaApi FtpControlSocket FtpListCtrl FtpListResult FtpTreeCtrl FtpTreeView FtpView \
  FzApiStructures IdentServerControl IdentServerDataSocket LocalComboCompletion \
  LocalFileListCtrl LocalView LocalView2 MFC64bitFix MainFrm MainThread ManualTransferDlg \
  Options OptionsConnection OptionsConnection2 OptionsDebugPage OptionsDirCachePage \
  OptionsFirewallPage OptionsGssPage OptionsIdentPage OptionsInterfacePage OptionsLanguagePage \
  OptionsLocalViewPage OptionsLoggingPage OptionsMiscPage OptionsPaneLayoutPage \
  OptionsProxyPage OptionsRemoteViewPage OptionsSpeedLimitPage OptionsSshPage \
  OptionsTransferCompressionPage OptionsTransferPage OptionsTypePage OptionsViewEditPage \
  PathFunctions QueueCtrl QueueView RemoteComboCompletion RetryServerList SFtpIpc \
  ServerChangePass ServerPath SftpControlSocket SiteManager SiteManagerAdvancedDlg SpeedLimit \
  SpeedLimitRuleDlg StatusCtrl StatusView TextProgressCtrl TransferAsDlg TransferSocket \
  VerifyCertDlg splitex structures version \
  misc/BrowseForFolder misc/CBase64Coding misc/ComboCompletion misc/CoolBtn misc/HyperLink \
  misc/Led misc/MarkupSTL misc/MinTrayBtn misc/SAPrefsDialog misc/SAPrefsStatic \
  misc/SAPrefsSubDlg misc/SBDestination misc/SystemTray misc/TransparentDialogBar \
  misc/VisualStylesXP misc/hookwnd misc/utf8
FZOBJ := $(FZSRC:%=$(O)/fz/%.obj) $(O)/fz/nt4compat.obj $(O)/fz/imports.obj $(O)/fz/crashlog.obj
ifdef DEBUGHOOKS
FZOBJ += $(O)/fz/debughooks.obj
endif

SFC := BE_NONE CONSOLE INT64 MISC NOISE PAGEANTC PSFTP SETTINGS SFTP SSH SSHAES SSHBLOWF SSHBN \
  SSHCRC SSHCRCDA SSHDES SSHDH SSHDSS SSHMD5 SSHPUBK SSHRAND SSHRSA SSHSH512 SSHSHA SSHZLIB \
  TREE234 VERSION WINNET WINSTORE cmdline pinger timing wildcard windefs winmisc winsftp
SFOBJ := $(SFC:%=$(O)/sf/%.obj) $(O)/sf/FzSFtpIpc.obj $(O)/sf/FzSFtpIpcClient.obj

LIBS := nafxcw.lib libcmt.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib \
        advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib version.lib \
        winspool.lib
SFLIBS := libcmt.lib kernel32.lib user32.lib gdi32.lib advapi32.lib wsock32.lib comdlg32.lib \
        shell32.lib

LDFLAGS := -nologo -subsystem:windows,4.0 -release -opt:ref $(LDEXTRA)
# MFC 6's headers name urlmon.lib, wininet.lib and olepro32.lib, which the NT 4.0 SDKs
# may not have; FileZilla uses none of them.
ifneq ($(CPU),alpha)
LDFLAGS += -nodefaultlib:urlmon.lib -nodefaultlib:wininet.lib -nodefaultlib:olepro32.lib
endif
# The PINT SDK's headers name libraries it hasn't (uuid3.lib): on ARM the libraries are
# only those named here.
ifeq ($(CPU),arm)
LDFLAGS += -nodefaultlib
LIBS += oldnames.lib
SFLIBS += oldnames.lib
endif

all: $(O)/FileZilla.exe $(O)/FzSFtp.exe

dirs:
	mkdir -p $(O)/z $(O)/fz/misc $(O)/sf

$(O)/zlib.lib: $(ZOBJ)
	$(LIB_) -nologo -out:$@ $^

$(O)/z/%.obj: $(Z)/%.c | dirs
	$(CC) $(CFLAGS) -c -Fo$@ $<

# The precompiled header, for FileZilla's C++. Visual C++ 4.1 for PowerPC crashes in its
# code generator (mdmisc.c, line 654) on many files compiled with one, and is fine
# without, so it has none on MIPS and PowerPC.
ifeq ($(filter mips ppc,$(CPU)),)
PCH  := $(O)/fz/stdafx.pch
YC   := -Ycstdafx.h -Fp$(PCH)
YU   := -Yustdafx.h -Fp$(PCH)
endif

$(O)/fz/StdAfx.obj: $(S)/StdAfx.cpp $(S)/StdAfx.h $(wildcard $(N)/include/*.h $(N)/stl/*) | dirs
	$(CC) $(CXXFLAGS) -c $(YC) -Fo$(O)/fz/StdAfx.obj $<
ifdef PCH
$(PCH): $(O)/fz/StdAfx.obj
endif

$(O)/fz/%.obj: $(S)/%.cpp $(O)/fz/StdAfx.obj
	$(CC) $(CXXFLAGS) -c $(YU) -Fo$@ $<

$(O)/fz/nt4compat.obj: $(N)/nt4compat.cpp $(O)/fz/StdAfx.obj
	$(CC) $(CXXFLAGS) -c $(YU) -Fo$@ $<

$(O)/fz/imports.obj: $(N)/imports.c | dirs
	$(CC) $(CFLAGS) -c -Fo$@ $<

$(O)/fz/crashlog.obj: $(N)/crashlog.c | dirs
	$(CC) $(CFLAGS) -c -Fo$@ $<

$(O)/fz/debughooks.obj: $(N)/debughooks.c | dirs
	$(CC) $(CFLAGS) -c -Fo$@ $<

$(O)/fz/FileZilla.res: $(S)/FileZilla.rc | dirs
	cd $(S) && $(RC) -l 0x409 -dNDEBUG $(RCFLAGS) -i$(S) -i$(S)/res -fo$@ FileZilla.rc

$(O)/FileZilla.exe: $(O)/fz/StdAfx.obj $(FZOBJ) $(O)/zlib.lib $(O)/fz/FileZilla.res
	$(LD) $(LDFLAGS) -map:$(O)/FileZilla.map -out:$@ $^ $(LIBS)

$(O)/sf/%.obj: $(SF)/%.C | dirs
	$(CC) $(CFLAGS) -DNO_SECURITY -DWIN32_LEAN_AND_MEAN -I$(SF) -c -Fo$@ $<
$(O)/sf/%.obj: $(SF)/%.c | dirs
	$(CC) $(CFLAGS) -DNO_SECURITY -DWIN32_LEAN_AND_MEAN -I$(SF) -c -Fo$@ $<
$(O)/sf/%.obj: $(SF)/%.cpp | dirs
	$(CC) $(CFLAGS) $(EH) -DNO_SECURITY -DWIN32_LEAN_AND_MEAN -I$(SF) -c -Fo$@ $<

$(O)/sf/FzSFtp.res: $(SF)/FzSFtp.rc | dirs
	cd $(SF) && $(RC) -l 0x409 -dNDEBUG $(RCFLAGS) -i$(SF) -fo$@ FzSFtp.rc

$(O)/FzSFtp.exe: $(SFOBJ) $(O)/sf/FzSFtp.res
	$(LD) $(LDFLAGS) -out:$@ $^ $(SFLIBS)

.PHONY: all dirs
