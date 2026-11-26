#!/bin/sh
# Build np2_gdi (Neko Project II, GDI and waveOut) for PINT (NT 4.0 on ARM) with the
# PINT 4.0 SDK.   build.sh /path/to/np2_gdi     Output: obj-arm/np2.exe in that folder.
# Runs the SDK as filezilla/nt4/sdkrun.sh does: Docker's tenox7/pint40sdk, or an unpacked
# copy (PINT_SDK, PINT_MSVC).
set -eu
here=$(cd "$(dirname "$0")" && pwd)
fz=$(cd "${1:?usage: build.sh np2_gdi}" && pwd)
. "$here/../../filezilla/nt4/sdkrun.sh"
cp "$here/../ntsdk/csrc.txt" "$fz/.pint-csrc.txt"
rm -rf "$fz/.pint-inc"; cp -R "$here/include" "$fz/.pint-inc"
pint_run '
set -e
rm -rf obj-arm; mkdir obj-arm
I="-Idialog -Icommon -Ii286c -Imem -Iio -Icbus -Ibios -Ilio -Ivram -Isound -Isound/vermouth"
I="$I -Isound/getsnd -Ifdd -Ifont -Igeneric -Itrap -Iwin9xc -Iwin9xc/dialog -I."
for f in $(cat .pint-csrc.txt); do
    n=$(echo $f | sed "s,/,_,g; s,\.[^.]*\$,,")
    echo "$f -Foobj-arm/$n.obj"
done > obj-arm/list
# One cl per file (its own -Fo), several at a time.
xargs -P6 -L1 sh -c "cl -nologo -c -O2 -MT -W1 -EHs-c- -GR- -DWIN32 -DNDEBUG -D_WINDOWS $I \$0 \$1 >obj-arm/\$(basename \$0).log 2>&1 || { cat obj-arm/\$(basename \$0).log; echo COMPILE FAILED \$0; exit 255; }" < obj-arm/list
cd win9xc
rc -l 0x411 -d NDEBUG -i ../.pint-inc -fo../obj-arm/np2.res np2.rc
cd ..
link -nologo -subsystem:windows -OUT:obj-arm/np2.exe obj-arm/*.obj obj-arm/np2.res \
    kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib \
    ole32.lib winmm.lib comctl32.lib
rm -rf .pint-csrc.txt .pint-inc
'
ls -l "$fz/obj-arm/np2.exe"
