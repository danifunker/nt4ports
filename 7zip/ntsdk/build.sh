#!/bin/sh
# Build 7-Zip 9.20 for NT 4.0 on Alpha, MIPS or PowerPC with tenox7/ntsdk (Visual C++ 4.1):
# no NT machine.
#   7zip/ntsdk/build.sh alpha|mips|ppc /path/to/7z920 [component...]
# 7z920 is a copy with the patch applied. Components are folders under CPP/7zip (default:
# all nine). Each one's result is in its folder for the CPU (ALPHA, MIPS or PPC), as on NT.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
cpu=${1:?usage: build.sh alpha|mips|ppc 7z920}
src=$(cd "${2:?usage: build.sh alpha|mips|ppc 7z920}" && pwd)
case $cpu in
alpha) CPU=ALPHA ;; mips) CPU=MIPS ;; ppc) CPU=PPC ;;
*) echo "build.sh: alpha, mips or ppc" >&2; exit 2 ;;
esac

mkdir -p "$src/ntsdk" "$src/CPP/7zip/Archive/Rar5"
cp "$here/../ppc/p7_vc4.h" "$here"/include/*.h "$src/ntsdk/"
cp "$here"/../rar5/* "$src/CPP/7zip/Archive/Rar5/"
shift 2
dirs=${*:-Bundles/Alone Bundles/Format7zF UI/Console Archive/Rar5 UI/FileManager UI/GUI \
UI/Explorer Bundles/SFXCon Bundles/SFXWin}

# The forced include and stubs come before the NT 4.0 SDK's headers, then three newer
# headers from the same image, all CPU-neutral: the x86 SDK's commctrl.h and prsht.h
# (Visual C++ 4.2's), with the common controls of Internet Explorer 3 that the RISC SDKs'
# lack, and the Windows 2000 SDK's NTSecAPI.h (the file manager's large pages). VC4=1:
# every compiler here is Visual C++ 4.1. The RISC SDKs have no rc, and nmake can't start ntsdk, so each resource.rc is
# compiled first with the x86 SDK's rc: resources are the same for every CPU. STRICT, as
# in later SDKs, where it's the default: the file manager passes WNDPROCs.
docker run --rm -v "$src":/src tenox7/ntsdk nt40 $cpu sh -c "
    mkdir -p /src/ntsdk/ie3
    cp /opt/nt40/x86/msdev/include/COMMCTRL.H /opt/nt40/x86/msdev/include/PRSHT.H \
       /opt/nt50/axp64/mssdk/include/NTSecAPI.h /src/ntsdk/ie3/
    export INCLUDE=\"Z:\\\\src\\\\ntsdk;Z:\\\\src\\\\ntsdk\\\\ie3;\$INCLUDE\"
    export CL=-DSTRICT
    s=0
    for d in $dirs; do
        cd /src/CPP/7zip/\$d
        rm -rf $CPU; mkdir $CPU
        if [ -f resource.rc ] && ! ntsdk nt40 x86 rc -i /src/ntsdk/ie3 -fo$CPU/resource.res resource.rc; then
            echo \"FAILED \$d (rc)\"; s=1; continue
        fi
        if nmake /nologo CPU=$CPU VC4=1 MY_STATIC_LINK=1; then
            echo \"BUILT \$d\"
        else
            echo \"FAILED \$d\"; s=1
        fi
    done
    chown -R $(id -u):$(id -g) /src
    exit \$s"
