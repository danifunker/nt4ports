#!/bin/sh
# Build FileZilla 2.2.32 for Windows NT 4.0 on Linux, with Microsoft's own compilers in
# containers: no Windows, no VM.
#
#   filezilla/nt4/build.sh alpha|mips|ppc|arm [OUTDIR]     (OUTDIR: default filezilla/out/CPU)
#
# Every CPU needs a Visual C++ 6.0 CD, which nothing here carries: VC6 is a folder with its
# VC98 and COMMON folders (nt4/get-vc6.sh unpacks them from the ISO). The Alpha CD is the
# one this was made with; for the other CPUs only its MFC source, headers and resource
# compiler are used, and any CPU's CD has those.
#
#   alpha       tenox7/ntsdk runs the CD's own Visual C++ 6.0 for Alpha, with its MFC and STL.
#   mips, ppc   tenox7/ntsdk's Visual C++ 4.1, with MFC 6 built for it: MFC is the output
#               folder of nt4/mfc/build-mfc.sh for that CPU.
#   arm         the PINT 4.0 SDK (see nt4/sdkrun.sh), with MFC 6 built for it, as above.
#
# The output folder gets FileZilla.exe and FzSFtp.exe. JOBS sets make -j (default 6);
# MAKEARGS adds make arguments (targets, -k).
set -eu
here=$(cd "$(dirname "$0")" && pwd)
fz=$(cd "$here/.." && pwd)
cpu=${1:?usage: build.sh alpha|mips|ppc|arm [OUTDIR]}
out=${2:-$fz/out/$cpu}
mkdir -p "$out"
out=$(cd "$out" && pwd)
jobs=${JOBS:-6}
vc6=$(cd "${VC6:?set VC6 to the folder with VC98 and COMMON of a Visual C++ 6.0 CD}" && pwd)
. "$here/sdkrun.sh"
mk="make -f /fz/nt4/Makefile CPU=$cpu O=/out -j$jobs ${MAKEARGS:-}"
# The resource compiler: Visual C++ 6.0's, which ntrun runs whatever the CD's CPU, with
# the system DLLs of that CPU.
rccpu=$(python3 -I -c 'import struct,sys
d=open(sys.argv[1],"rb").read(4096); pe=struct.unpack_from("<I",d,0x3c)[0]
print({0x14c:"x86",0x184:"alpha",0x166:"mips",0x1f0:"ppc"}[struct.unpack_from("<H",d,pe+4)[0]])' \
    "$vc6/COMMON/MSDEV98/BIN/RC.EXE")
rc="RC='env NTRUN_DLLPATH=/opt/sys/$rccpu ntrun /vc6/COMMON/MSDEV98/BIN/RC.EXE'"

case $cpu in
alpha)
    V='Z:\vc6\VC98'
    ntsdk_run alpha "$mk" -v "$out":/out -v "$vc6":/vc6:ro \
        -e "NTRUN_PATH=$V\\BIN;Z:\\vc6\\COMMON\\MSDEV98\\BIN" \
        -e "INCLUDE=$V\\INCLUDE;$V\\MFC\\INCLUDE;$V\\ATL\\INCLUDE" \
        -e "LIB=$V\\LIB;$V\\MFC\\LIB"
    ;;
mips|ppc)
    mfc=$(cd "${MFC:?set MFC to the output folder of nt4/mfc/build-mfc.sh $cpu}" && pwd)
    # MFC 6 and the CD's Win32 headers before Visual C++ 4.1's C library and the NT 4.0
    # SDK; MFC 6's library, never ntsdk's MFC 4.1.
    ntsdk_run $cpu "$mk $rc" -v "$out":/out -v "$vc6":/vc6:ro -v "$mfc":/mfc:ro \
        -e 'INCLUDE=Z:\mfc\include;Z:\mfc\sdk;C:\msdev\include;C:\mstools\include' \
        -e 'LIB=Z:\mfc\lib;C:\msdev\lib;C:\mstools\lib'
    ;;
arm)
    mfc=$(cd "${MFC:?set MFC to the output folder of nt4/mfc/build-mfc.sh arm}" && pwd)
    # The SDK's cl and link set INCLUDE and LIB themselves: MFC goes in through CL and LINK.
    # rc is the SDK's own.
    pint_run "export CL='-IZ:\\mfc\\include -IZ:\\mfc\\ie4' LINK='-libpath:Z:\\mfc\\lib'; $mk RCFLAGS='-i/mfc/include -i/mfc/sdk'" \
        "$out:/out" "$mfc:/mfc"
    ;;
*) echo "build.sh: CPU is alpha, mips, ppc or arm" >&2; exit 2 ;;
esac
ls -l "$out"/*.exe
