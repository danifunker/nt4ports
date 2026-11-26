#!/bin/sh
# Build ScummVM 0.6.1 with SDL 1.2.15 for PINT (NT 4.0 on ARM) with the PINT 4.0 SDK.
#   build.sh /path/to/scummvm-0.6.1 /path/to/SDL-1.2.15
# ScummVM needs patches/scummvm-0.6.1-nt4.diff. The script puts sdl/SDL_config.h in SDL's
# include folder. Output: obj-arm/scummvm.exe in the ScummVM folder.
# Runs the SDK as filezilla/nt4/sdkrun.sh does: Docker's tenox7/pint40sdk, or an unpacked
# copy (PINT_SDK, PINT_MSVC).
set -eu
here=$(cd "$(dirname "$0")/.." && pwd)
usage="usage: build.sh scummvm-0.6.1 SDL-1.2.15"
fz=$(cd "${1:?$usage}" && pwd)
sdl=$(cd "${2:?$usage}" && pwd)
. "$here/../filezilla/nt4/sdkrun.sh"
cp "$here/sdl/SDL_config.h" "$sdl/include/SDL_config.h"
cp "$here/sdl/csrc.txt" "$fz/.pint-sdl.txt"
cp "$here/csrc.txt" "$fz/.pint-svm.txt"
rm -rf "$fz/.pint-inc"; cp -R "$here/pint/include" "$fz/.pint-inc"
pint_run '
set -e
rm -rf obj-arm; mkdir obj-arm
# -GR: sound/mixer.cpp uses dynamic_cast.
F="-nologo -c -O2 -MT -W1 -EHs-c- -GR -DWIN32 -DNDEBUG -D_WINDOWS"
# One cl per file (its own -Fo), several at a time. Object names: the path with / as _.
for f in $(cat .pint-sdl.txt); do
    echo "/sdl/$f -Foobj-arm/sdl_$(echo $f | sed "s,/,_,g; s,\.[^.]*\$,,").obj -I/sdl/include"
done > obj-arm/list
for f in $(cat .pint-svm.txt); do
    echo "$f -Foobj-arm/$(echo $f | sed "s,/,_,g; s,\.[^.]*\$,,").obj -I. -Icommon -I/sdl/include"
done >> obj-arm/list
xargs -P6 -L1 sh -c "cl $F \$0 \$* >obj-arm/\$(basename \$0).log 2>&1 || { cat obj-arm/\$(basename \$0).log; echo COMPILE FAILED \$0; exit 255; }" < obj-arm/list
# include/winresrc.h stands in for the one the SDK lacks.
rc -i .pint-inc -fo obj-arm/scummvm.res scummvm.rc
link -nologo -subsystem:console -OUT:obj-arm/scummvm.exe obj-arm/*.obj obj-arm/scummvm.res \
    kernel32.lib user32.lib gdi32.lib winmm.lib advapi32.lib shell32.lib
rm -rf .pint-sdl.txt .pint-svm.txt .pint-inc
' "$sdl:/sdl"
ls -l "$fz/obj-arm/scummvm.exe"
