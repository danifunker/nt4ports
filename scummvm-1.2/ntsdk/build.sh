#!/bin/sh
# Build ScummVM 1.2.1 with SDL 1.2.15 and zlib for NT 4.0 on Alpha with Visual C++ 6.0 for
# Alpha, run by tenox7/ntsdk: no NT machine.
#   build.sh /path/to/scummvm-1.2.1 /path/to/SDL-1.2.15
# ScummVM needs patches/scummvm-1.2.1-nt4.diff. VC6 is the folder with VC98 and COMMON of the
# Visual C++ 6.0 for Alpha CD (filezilla/nt4/get-vc6.sh unpacks them from the ISO). The script
# puts scummvm/sdl/SDL_config.h in SDL's include folder. Output: obj-alpha/scummvm.exe in the
# ScummVM folder. JOBS: compiles at a time (default 6). KEEPGOING=1: compile every file and
# list the ones that fail, without linking.
set -eu
here=$(cd "$(dirname "$0")/.." && pwd)
top=$(cd "$here/.." && pwd)
usage="usage: build.sh scummvm-1.2.1 SDL-1.2.15"
src=$(cd "${1:?$usage}" && pwd)
sdl=$(cd "${2:?$usage}" && pwd)
vc6=$(cd "${VC6:?set VC6 to the folder with VC98 and COMMON of the Visual C++ 6.0 for Alpha CD}" && pwd)
jobs=${JOBS:-6}
keep=${KEEPGOING:-0}
o=obj-alpha
cp "$top/scummvm/sdl/SDL_config.h" "$sdl/include/SDL_config.h"
rm -rf "$src/$o"; mkdir -p "$src/$o/obj" "$src/$o/log"
cp "$top/scummvm/sdl/csrc.txt" "$src/$o/sdl.txt"
cp "$here/csrc.txt" "$src/$o/svm.txt"
cp "$here/compat/svm_vc6.h" "$src/$o/svm_vc6.h"
cp -R "$top/filezilla/zlib" "$src/$o/zlib"

# The engines and features of ScummVM 1.2.1's own Visual Studio projects (dists/msvc9), less
# the libraries not built here (MP3, Ogg Vorbis, FLAC), the MT-32 emulator, translations and
# the assembly scalers. SCUMM_NEED_ALIGNMENT: Alpha faults on unaligned reads.
# DISABLE_DOSBOX_OPL: DOSBox's OPL emulator takes the addresses of function templates
# instantiated by their arguments alone, which VC6 can't tell apart; MAME's plays AdLib.
defs="-DWIN32 -D_WINDOWS -DNDEBUG -DRELEASE_BUILD -DSCUMM_NEED_ALIGNMENT -DDISABLE_DOSBOX_OPL \
-DUSE_ZLIB -DUSE_SCALERS -DUSE_HQ_SCALERS -DUSE_RGB_COLOR"
for e in SCUMM SCUMM_7_8 HE AGI AGOS AGOS2 CINE CRUISE DRACI DRASCULA GOB GROOVIE KYRA LURE \
         MADE PARALLACTION QUEEN SAGA IHNM SCI SKY SWORD1 SWORD2 TEENAGENT TINSEL TOUCHE TUCKER; do
    defs="$defs -DENABLE_$e"
done
echo "$defs" > "$src/$o/defines.txt"

V='Z:\vc6\VC98'
docker run --rm -v "$src":/src -v "$sdl":/sdl -v "$vc6":/vc6:ro -w /src \
    -e "NTRUN_PATH=$V\\BIN;Z:\\vc6\\COMMON\\MSDEV98\\BIN" -e "INCLUDE=$V\\INCLUDE" -e "LIB=$V\\LIB" \
    tenox7/ntsdk nt40 alpha sh -c '
set -e
o='$o' jobs='$jobs' keep='$keep'
F="-nologo -c -O2 -W1 -MT"
CF="$F -DWIN32 -DNDEBUG -D_WINDOWS -I/sdl/include"
# -GX: ScummVM throws nothing, but VC6 makes temporaries only with it. -GR: dynamic_cast.
# -Zm500: with less, VC6 runs out of heap on the built-in GUI theme (gui/themes/default.inc).
SVM="$F -GX -GR -Zm500 -FI$o/svm_vc6.h -I. -Iengines -I/sdl/include -I$o/zlib $(cat $o/defines.txt)"
export CF SVM o keep
cat > $o/cc.sh <<"CC"
# cc.sh FLAGS NAME SOURCE: compile SOURCE to $o/obj/NAME.obj with the flags in variable FLAGS.
eval "flags=\$$1"
cl $flags "$3" -Fo$o/obj/$2.obj > $o/log/$2.log 2>&1 && exit 0
echo "COMPILE FAILED $3"
[ "$keep" = 1 ] && exit 0
cat $o/log/$2.log; exit 255
CC
{
    for f in $(cat $o/sdl.txt); do echo "CF sdl_$(echo $f | tr / _ | sed "s/\.c\$//") /sdl/$f"; done
    for f in $o/zlib/*.c; do echo "CF zlib_$(basename $f .c) $f"; done
    for f in $(cat $o/svm.txt); do echo "SVM $(echo $f | tr / _ | sed "s/\.[^.]*\$//") $f"; done
} > $o/list
xargs -P$jobs -L1 sh $o/cc.sh < $o/list
[ "$keep" = 1 ] && exit 0
# The RISC SDKs have no rc; the x86 SDK makes the same resources. HAS_INCLUDE_SET: the icon
# by its path from the include folder (.), as in the Visual Studio projects.
ntsdk nt40 x86 rc -i. -DHAS_INCLUDE_SET -fo$o/scummvm.res dists/scummvm.rc
ls $o/obj/*.obj > $o/link.rsp; echo $o/scummvm.res >> $o/link.rsp
# A console program, for its messages, that starts at the WinMain of ScummVM
# (backends/platform/sdl/main.cpp), as its Windows releases do: SDL renames main to SDL_main.
link -nologo -subsystem:console -entry:WinMainCRTStartup -OUT:$o/scummvm.exe @$o/link.rsp \
    kernel32.lib user32.lib gdi32.lib winmm.lib advapi32.lib shell32.lib > $o/link.log 2>&1 ||
    { tail -60 $o/link.log; exit 1; }
'
s=$?
docker run --rm -v "$src":/src tenox7/ntsdk chown -R "$(id -u):$(id -g)" /src/$o || true
[ "$keep" = 1 ] || ls -l "$src/$o/scummvm.exe"
exit $s
