#!/bin/sh
# Build Visual C++ 6.0's MFC (nafxcw.lib) for MIPS, PowerPC or ARM, which it never shipped
# for, from the MFC source on any Visual C++ 6.0 CD.
#
#   filezilla/nt4/mfc/build-mfc.sh mips|ppc|arm VC98 OUTDIR
#
# VC98: the CD's VC98 folder (nt4/get-vc6.sh unpacks it). OUTDIR gets MFC's headers,
# the CD's Win32 headers (sdk/) and lib/nafxcw.lib: give it to build.sh as MFC_MIPS,
# MFC_PPC or MFC_ARM. MIPS and PowerPC use tenox7/ntsdk's Visual C++ 4.1, ARM the PINT
# SDK (see ../sdkrun.sh). JOBS sets make -j (default 6).
set -eu
here=$(cd "$(dirname "$0")" && pwd)
fz=$(cd "$here/../.." && pwd)
cpu=${1:?usage: build-mfc.sh mips|ppc|arm VC98 OUTDIR}
vc98=$(cd "${2:?VC98 folder}" && pwd)
out=${3:?OUTDIR}
jobs=${JOBS:-6}
. "$fz/nt4/sdkrun.sh"

case $cpu in mips|ppc) flavor=vc4 ;; arm) flavor=arm ;; *) echo "build-mfc.sh: mips, ppc or arm" >&2; exit 2 ;; esac
python3 -I "$here/prepare.py" "$vc98" "$out" $flavor
out=$(cd "$out" && pwd)
mk="make -f /fz/nt4/mfc/Makefile CPU=$cpu M=/mfc"

case $cpu in
mips|ppc)
    # MFC's headers, then the CD's Win32 headers, then Visual C++ 4.1's C library and the
    # NT 4.0 SDK's own.
    ntsdk_run $cpu "$mk -k -j$jobs objs; $mk all" -v "$out":/mfc \
        -e 'INCLUDE=Z:\mfc\include;Z:\mfc\sdk;C:\msdev\include;C:\mstools\include'
    ;;
arm)
    # MFC's headers, then the CD's Internet Explorer 4 headers MFC 6 needs, then PINT's.
    # The SDK's cl sets INCLUDE itself, so they go in through CL.
    pint_run "export CL='-IZ:\\mfc\\include -IZ:\\mfc\\ie4'; $mk -k -j$jobs objs; $mk all" \
        "$out:/mfc"
    ;;
esac
ls -l "$out/lib/nafxcw.lib"
