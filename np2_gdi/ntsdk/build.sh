#!/bin/sh
# Build np2_gdi (Neko Project II, GDI and waveOut) for NT 4.0 on Alpha, MIPS or PowerPC
# with tenox7/ntsdk (Visual C++ 4.1): no NT machine.
#   build.sh alpha|mips|ppc /path/to/np2_gdi
# Output: obj-CPU/np2.exe in the source folder. JOBS: compiles at a time (default 6).
# The file list is np2cmips.mak's (the Visual C++ 4 MIPS project), in csrc.txt.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
cpu=${1:?usage: build.sh alpha|mips|ppc np2_gdi}
src=$(cd "${2:?usage: build.sh alpha|mips|ppc np2_gdi}" && pwd)
case $cpu in alpha|mips|ppc) ;; *) echo "build.sh: alpha, mips or ppc" >&2; exit 2 ;; esac
jobs=${JOBS:-6}
cp "$here/csrc.txt" "$src/.ntsdk-csrc-$cpu.txt"

docker run --rm -v "$src":/src -w /src tenox7/ntsdk nt40 $cpu sh -c '
set -e
cpu='$cpu' jobs='$jobs'
o=obj-$cpu
rm -rf $o; mkdir $o
I="-Idialog -Icommon -Ii286c -Imem -Iio -Icbus -Ibios -Ilio -Ivram -Isound -Isound/vermouth
   -Isound/getsnd -Ifdd -Ifont -Igeneric -Itrap -Iwin9xc -Iwin9xc/dialog -I."
export I
# Object names: the path with / as _, so that two sources of one name stay apart.
xargs -n1 -P$jobs sh -c "n=\$(echo \$0 | sed \"s,/,_,g; s,\\.[^.]*\$,,\"); \
    cl -nologo -c -O2 -W1 -ML -DWIN32 -DNDEBUG -D_WINDOWS \$I -Fo$o/\$n.obj \$0 \
    >/tmp/cl.\$\$ 2>&1 || { cat /tmp/cl.\$\$; echo COMPILE FAILED \$0; exit 255; }" \
    < .ntsdk-csrc-$cpu.txt
# The RISC SDKs have no rc; the x86 SDK makes the same resources.
cd win9xc
ntsdk nt40 x86 rc -l 0x411 -d NDEBUG -fo../$o/np2.res np2.rc
cd ..
link -nologo -subsystem:windows -OUT:$o/np2.exe $o/*.obj $o/np2.res \
    kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib \
    ole32.lib winmm.lib comctl32.lib
'
s=$?
docker run --rm -v "$src":/src tenox7/ntsdk sh -c "chown -R $(id -u):$(id -g) /src/obj-$cpu; rm -f /src/.ntsdk-csrc-$cpu.txt" || true
ls -l "$src/obj-$cpu/np2.exe"
exit $s
