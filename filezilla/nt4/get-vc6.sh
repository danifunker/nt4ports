#!/bin/sh
# Unpack what the build takes from a Visual C++ 6.0 CD: the compiler, headers, libraries,
# MFC (with its source) and ATL of VC98, and COMMON\MSDEV98\BIN (the resource compiler
# and mspdb60.dll).
#
#   filezilla/nt4/get-vc6.sh VC6.iso OUTDIR       (then VC6=OUTDIR for build.sh)
#
# The Alpha CD (msvc60pro, Visual C++ 6.0 for Alpha) is the one this was made with, and
# the only one that builds the Alpha version; for the other CPUs any CPU's CD will do.
# Needs 7z. Nothing of it goes into the repository.
set -eu
iso=${1:?usage: get-vc6.sh VC6.iso OUTDIR}
out=${2:?usage: get-vc6.sh VC6.iso OUTDIR}
mkdir -p "$out"
7z x -y -o"$out" "$iso" 'VC98/BIN/*' 'VC98/INCLUDE/*' 'VC98/LIB/*' 'VC98/MFC/*' 'VC98/ATL/*' \
    'COMMON/MSDEV98/BIN/*' > /dev/null
# The Alpha CD is plain ISO 9660: six of the STL's header names are cut to eight letters.
cd "$out/VC98/INCLUDE"
for p in FCTIONAL:FUNCTIONAL ALGRITHM:ALGORITHM STDXCEPT:STDEXCEPT STREAMBF:STREAMBUF \
         STRSTREM:STRSTREAM XCEPTION:EXCEPTION; do
    if [ -f "${p%%:*}" ] && [ ! -f "${p##*:}" ]; then cp "${p%%:*}" "${p##*:}"; fi
done
echo "get-vc6.sh: $out"
