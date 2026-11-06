#!/bin/sh
# Build Notepad++ 3.3 for PINT with the PINT 4.0 SDK.
# Usage: notepad++/pint/build.sh /path/to/npp.3.3.src   (a copy that has the patch applied)
# Needs the SDK's one-time compiler setup: see pint/README.md.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
src=$(cd "$1" && pwd)

cp -R "$here/../compat" "$src/compat"
cp "$here/scifiles.txt" "$here/nppfiles.txt" "$src/"

docker run --rm --platform linux/amd64 -v msvc-arm32:/opt/msvc -v "$src":/src \
    tenox7/pint40sdk bash -c '
    set -e
    cd /src/scintilla/win32
    mkdir -p ../bin obj
    cl -nologo -c -TP -O1 -MT -EHsc -GR- -W1 -DSCI_LEXER -DNDEBUG -DWIN32=100 \
       -D_WIN32_WINNT=0x0400 -I../../compat -I../include -I../src -Foobj/ \
       $(cat /src/scifiles.txt) ../../compat/npp_rt.cpp
    rc -foobj/ScintRes.res ScintRes.rc
    link -nologo -DLL -DEF:Scintilla.def -OUT:../bin/SciLexer.dll obj/*.obj \
         obj/ScintRes.res kernel32.lib user32.lib gdi32.lib imm32.lib ole32.lib uuid.lib

    cd /src/PowerEditor/src
    mkdir -p obj
    I="-I../../compat -I../../scintilla/include -I../include -I. -IWinControls/AboutDlg
       -IWinControls -IWinControls/ImageListSet -IWinControls/OpenSaveFileDialog
       -IWinControls/SplitterContainer -IWinControls/StaticDialog -IWinControls/TabBar
       -IWinControls/ToolBar -IMISC/Process -IScitillaComponent -IMISC -IMISC/SysMsg
       -IWinControls/StatusBar -IWinControls/StaticDialog/RunDlg -ITinyXml
       -IWinControls/ColourPicker -IMISC/RegExt -IWinControls/TrayIcon -IWinControls/shortcut
       -IWinControls/Grid -IWinControls/ContextMenu -IMISC/PluginsManager"
    cl -nologo -c -O2 -MT -EHsc -GR- -W1 -DWIN32=100 -D_WIN32_WINNT=0x0400 -DNDEBUG \
       -D_WINDOWS -DNPP_NT4_HEADERS -FInpp_nt4compat.h $I -Foobj/ \
       $(cat /src/nppfiles.txt) ../../compat/npp_ministl.cpp ../../compat/npp_rt.cpp
    rc -d __GNUC__ -d NDEBUG -I. -I../../scintilla/include -IWinControls \
       -IWinControls/ColourPicker -IScitillaComponent -IMISC/RegExt \
       -IWinControls/StaticDialog/RunDlg -IWinControls/shortcut -IWinControls/Grid \
       -IWinControls/StaticDialog -IWinControls/TabBar -foobj/Notepad_plus.res npp_nt4.rc
    link -nologo -subsystem:windows -OUT:obj/notepad++.exe obj/*.obj obj/Notepad_plus.res \
         comctl32.lib shell32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib ole32.lib \
         kernel32.lib'
