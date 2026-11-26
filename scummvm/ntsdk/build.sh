#!/bin/sh
# Build ScummVM 0.6.1 with SDL 1.2.15 for NT 4.0 on Alpha, MIPS or PowerPC with
# tenox7/ntsdk (Visual C++ 4.1): no NT machine.
#   build.sh alpha|mips|ppc /path/to/scummvm-0.6.1 /path/to/SDL-1.2.15
# ScummVM needs patches/scummvm-0.6.1-nt4.diff. The script puts sdl/SDL_config.h in SDL's
# include folder. Output: obj-CPU/scummvm.exe in the ScummVM folder.
# JOBS: compiles at a time (default 6).
set -eu
here=$(cd "$(dirname "$0")/.." && pwd)
usage="usage: build.sh alpha|mips|ppc scummvm-0.6.1 SDL-1.2.15"
cpu=${1:?$usage}
src=$(cd "${2:?$usage}" && pwd)
sdl=$(cd "${3:?$usage}" && pwd)
case $cpu in alpha|mips|ppc) ;; *) echo "build.sh: alpha, mips or ppc" >&2; exit 2 ;; esac
jobs=${JOBS:-6}
cp "$here/sdl/SDL_config.h" "$sdl/include/SDL_config.h"
cp "$here/sdl/csrc.txt" "$src/.ntsdk-sdl-$cpu.txt"
cp "$here/csrc.txt" "$src/.ntsdk-svm-$cpu.txt"
# Shared by the CPUs' builds and left in place: its name changes the code layout VC4
# makes (not the code), so the builds stay the same as the tested ones.
cp "$here/compat/svm_vc4.h" "$src/.ntsdk-vc4.h"

docker run --rm -v "$src":/src -v "$sdl":/sdl -w /src tenox7/ntsdk nt40 $cpu sh -c '
set -e
cpu='$cpu' jobs='$jobs'
o=obj-$cpu
rm -rf $o; mkdir $o
# Object names: the path with / as _, so that two sources of one name stay apart.
cc() {
    xargs -n1 -P$jobs sh -c "n=\$(echo \$0 | sed \"s,/,_,g; s,\\.[^.]*\$,,\"); \
        cl -nologo -c -O2 -W1 -MT -DWIN32 -DNDEBUG -D_WINDOWS $1 -Fo/src/$o/$2\$n.obj $3\$0 \
        >/tmp/cl.\$\$ 2>&1 || { cat /tmp/cl.\$\$; echo COMPILE FAILED \$0; exit 255; }"
}
cc "-I/sdl/include" sdl_ /sdl/ < .ntsdk-sdl-$cpu.txt
# -GX: without it, Visual C++ 4 makes no temporary String from a char array.
# -GR: sound/mixer.cpp uses dynamic_cast.
# -vmg: Common::Debugger<T> takes pointers to members of T before T is complete.
cc "-GX -GR -vmg -FI.ntsdk-vc4.h -I. -Icommon -I/sdl/include" "" "" < .ntsdk-svm-$cpu.txt
# The RISC SDKs have no rc; the x86 SDK makes the same resources.
ntsdk nt40 x86 rc -fo$o/scummvm.res scummvm.rc
link -nologo -subsystem:console -OUT:$o/scummvm.exe $o/*.obj $o/scummvm.res \
    kernel32.lib user32.lib gdi32.lib winmm.lib advapi32.lib shell32.lib
'
s=$?
docker run --rm -v "$src":/src tenox7/ntsdk sh -c "chown -R $(id -u):$(id -g) /src/obj-$cpu; rm -f /src/.ntsdk-*-$cpu.txt" || true
ls -l "$src/obj-$cpu/scummvm.exe"
exit $s
