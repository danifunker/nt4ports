#!/bin/sh
# Make the release zips from the builds of nt4/build.sh.
#
#   filezilla/nt4/package.sh OUTROOT DISTDIR
#
# OUTROOT holds a folder per CPU (alpha, mips, ppc, arm) as build.sh made them. DISTDIR
# gets filezilla-2.2.32-nt4-CPU.zip for each, filezilla-2.2.32-nt4-src.zip (this folder of
# the repository at HEAD, as git has it) and SHA256SUMS.txt. Needs zip.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
fz=$(cd "$here/.." && pwd)
root=$(cd "${1:?usage: package.sh OUTROOT DISTDIR}" && pwd)
mkdir -p "${2:?usage: package.sh OUTROOT DISTDIR}"
dist=$(cd "$2" && pwd)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
crlf() { sed 's/\r*$/\r/' "$1" > "$2"; }

for cpu in alpha mips ppc arm; do
    [ -f "$root/$cpu/FileZilla.exe" ] || { echo "package.sh: no $root/$cpu/FileZilla.exe" >&2; exit 1; }
    d=$tmp/filezilla-2.2.32-nt4-$cpu
    mkdir -p "$d"
    cp "$root/$cpu/FileZilla.exe" "$root/$cpu/FzSFtp.exe" "$d/"
    crlf "$root/$cpu/FileZilla.map" "$d/FileZilla.map"
    crlf "$fz/LICENSE" "$d/LICENSE.TXT"
    crlf "$fz/zlib/LICENSE" "$d/ZLIB.TXT"
    crlf "$here/README-PORT.TXT" "$d/README-PORT.TXT"
    rm -f "$dist/filezilla-2.2.32-nt4-$cpu.zip"
    (cd "$tmp" && zip -qr -9 "$dist/filezilla-2.2.32-nt4-$cpu.zip" "filezilla-2.2.32-nt4-$cpu")
done
rm -f "$dist/filezilla-2.2.32-nt4-src.zip"
(cd "$fz" && git archive --format=zip --prefix=filezilla-2.2.32-nt4-src/ -o "$dist/filezilla-2.2.32-nt4-src.zip" HEAD .)
(cd "$dist" && sha256sum filezilla-2.2.32-nt4-*.zip > SHA256SUMS.txt)
ls -l "$dist"
