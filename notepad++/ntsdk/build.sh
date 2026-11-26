#!/bin/sh
# Build Notepad++ 3.3 for NT 4.0 on Alpha, MIPS or PowerPC with tenox7/ntsdk
# (Visual C++ 4.1): no NT machine. The same compiles as ppc/build.bat.
#   notepad++/ntsdk/build.sh alpha|mips|ppc /path/to/npp.3.3.src   (patch applied)
# Output: so-CPU/SciLexer.dll and no-CPU/notepad++.exe in the source folder.
# JOBS: compiles at a time (default 6).
set -eu
here=$(cd "$(dirname "$0")" && pwd)
cpu=${1:?usage: build.sh alpha|mips|ppc npp.3.3.src}
src=$(cd "${2:?usage: build.sh alpha|mips|ppc npp.3.3.src}" && pwd)
case $cpu in alpha|mips|ppc) ;; *) echo "build.sh: alpha, mips or ppc" >&2; exit 2 ;; esac
jobs=${JOBS:-6}

rm -rf "$src/compat"
cp -R "$here/../compat" "$src/compat"
cp "$here/../pint/scifiles.txt" "$here/../pint/nppfiles.txt" "$src/"

docker run --rm -v "$src":/src tenox7/ntsdk nt40 $cpu sh -c '
set -e
cpu='$cpu' jobs='$jobs'
so=/src/so-$cpu no=/src/no-$cpu
rm -rf $so $no; mkdir -p $so $no
# The x86 SDK'"'"'s commctrl.h and prsht.h (Visual C++ 4.2'"'"'s, CPU-neutral) before the RISC
# SDK'"'"'s: they have the common controls of Internet Explorer 3 (TCITEM and the rest).
mkdir -p /src/ie3
cp /opt/nt40/x86/msdev/include/COMMCTRL.H /opt/nt40/x86/msdev/include/PRSHT.H /src/ie3/
INCLUDE="Z:\\src\\ie3;$INCLUDE"; export INCLUDE

# Scintilla
cd /src/scintilla/win32
xargs -n1 -P$jobs sh -c "cl -nologo -c -TP -O1 -MT -GX -W1 -DSCI_LEXER -DNDEBUG -DWIN32 \
    -D_WIN32_WINNT=0x0400 -FInpp_vc4.h -I../../compat -I../include -I../src -Fo$so/ \$0 \
    >/tmp/cl.\$\$ 2>&1 || { cat /tmp/cl.\$\$; echo COMPILE FAILED \$0; exit 255; }" \
    < /src/scifiles.txt
# The RISC SDKs have no rc; the x86 SDK makes the same resources.
ntsdk nt40 x86 rc -fo$so/ScintRes.res ScintRes.rc
rm -f $so/ScintillaWinS.obj
link -nologo -DLL -DEF:Scintilla.def -OUT:$so/SciLexer.dll $so/*.obj $so/ScintRes.res \
    kernel32.lib user32.lib gdi32.lib imm32.lib ole32.lib uuid.lib

# Notepad++
cd /src/PowerEditor/src
I="-I../../compat -IWinControls/AboutDlg -I../../scintilla/include -I../include -IWinControls
   -IWinControls/ImageListSet -IWinControls/OpenSaveFileDialog -IWinControls/SplitterContainer
   -IWinControls/StaticDialog -IWinControls/TabBar -IWinControls/ToolBar -IMISC/Process
   -IScitillaComponent -IMISC -IMISC/SysMsg -IWinControls/StatusBar -I. -IWinControls/StaticDialog/RunDlg
   -ITinyXml -IWinControls/ColourPicker -IMISC/RegExt -IWinControls/TrayIcon -IWinControls/shortcut
   -IWinControls/Grid -IWinControls/ContextMenu -IMISC/PluginsManager"
export I
{ cat /src/nppfiles.txt; echo ../../compat/npp_ministl.cpp; } | \
xargs -n1 -P$jobs sh -c "cl -nologo -c -O2 -MT -GX -W1 -DWIN32 -DNDEBUG -D_WINDOWS -D_WIN32_WINNT=0x0400 \
    -DNPP_NT4_HEADERS -FInpp_vc4.h -FInpp_nt4compat.h \$I -Fo$no/ \$0 \
    >/tmp/cl.\$\$ 2>&1 || { cat /tmp/cl.\$\$; echo COMPILE FAILED \$0; exit 255; }"
# npp_nt4.rc includes the dialogs (-d __GNUC__): see ../README.md.
P="Z:\\src\\PowerEditor\\src"
INCLUDE="$INCLUDE;$P;$P\\WinControls;$P\\WinControls\\ColourPicker;$P\\ScitillaComponent;$P\\MISC\\RegExt;$P\\WinControls\\StaticDialog\\RunDlg;$P\\WinControls\\shortcut;$P\\WinControls\\Grid;$P\\WinControls\\StaticDialog;$P\\WinControls\\TabBar;Z:\\src\\scintilla\\include"
ntsdk nt40 x86 rc -i $(echo "$INCLUDE" | sed "s/;/ -i /g") -d __GNUC__ -d NDEBUG -fo$no/Notepad_plus.res npp_nt4.rc
link -nologo -subsystem:windows -OUT:$no/notepad++.exe $no/*.obj $no/Notepad_plus.res comctl32.lib \
    shell32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib ole32.lib kernel32.lib
chown -R '$(id -u):$(id -g)' $so $no
'
ls -l "$src/so-$cpu/SciLexer.dll" "$src/no-$cpu/notepad++.exe"
