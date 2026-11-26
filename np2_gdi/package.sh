#!/bin/sh
# Make the release zips of np2_gdi for NT 4.0 from the builds of ntsdk/build.sh and
# pint/build.sh:
#   np2_gdi/package.sh /path/to/np2_gdi OUTDIR
# np2_gdi is the upstream clone at ac36bf2 holding obj-alpha, obj-mips, obj-ppc and obj-arm.
# Makes np2_gdi-0.81a-nt4-CPU.zip for each, and np2_gdi-0.81a-nt4-src.zip: upstream's
# source without its bin/ (commercial disk images, a font ROM, prebuilt binaries), obj/,
# IDE caches and np2tool's disk image, plus this folder.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
src=$(cd "${1:?usage: package.sh np2_gdi OUTDIR}" && pwd)
mkdir -p "${2:?usage: package.sh np2_gdi OUTDIR}"
out=$(cd "$2" && pwd)
v=np2_gdi-0.81a-nt4
rev=$(git -C "$src" rev-parse --short HEAD)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

crlf() { sed 's/$/\r/' "$1" > "$2"; }

for cpu in alpha mips ppc arm; do
    case $cpu in
    alpha) what="Alpha, Visual C++ 4.1 (tenox7/ntsdk)"; tested="Not yet run." ;;
    mips)  what="MIPS, Visual C++ 4.1 (tenox7/ntsdk)"; tested="Not yet run: there was no MIPS machine to test on." ;;
    ppc)   what="PowerPC, Visual C++ 4.1 (tenox7/ntsdk)"; tested="Tested on NT 4.0 SP2 (QEMU 40P): boots a floppy image and runs, at 36 FPS." ;;
    arm)   what="ARM (PINT), Microsoft C for ARM (PINT 4.0 SDK)"; tested="Tested on PINT 4.0 (QEMU raspi2b): boots a floppy image and runs, at 11 FPS." ;;
    esac
    d=$tmp/$v-$cpu
    mkdir -p "$d"
    cp "$src/obj-$cpu/np2.exe" "$d/np2.exe"
    crlf "$here/LICENSE" "$d/LICENSE.TXT"
    cat > "$tmp/readme" <<EOF
Neko Project II (C version, GDI) for Windows NT 4.0 (unofficial port).
A PC-9801 emulator by Yui and the NP2 developer team, version 0.81a, from spaztron64's
np2_gdi (github.com/spaztron64/np2_gdi, commit $rev), which draws with GDI and plays sound
with waveOut, so it needs no DirectX. Built on Linux from that source, unchanged, by
np2_gdi/ in github.com/danifunker/nt4ports; the source is in $v-src.zip.

Run np2.exe, then FDD1 > Open to insert a disk image (.d88, .d98, .fdi, or raw: .xdf, .hdm)
and Emulate > Reset. No disk images or ROMs are included. Without FONT.ROM, np2 makes its
font from the Windows fonts on first run (font.tmp). Settings are kept in np2.cfg beside it.

This build: $what.
$tested
Sound, the settings dialogs and hard disk images have not been tested on any CPU.
Licence: LICENSE.TXT (Neko Project II's terms of use).
EOF
    crlf "$tmp/readme" "$d/README-PORT.TXT"
    (cd "$tmp" && rm -f "$out/$v-$cpu.zip" && zip -qrX "$out/$v-$cpu.zip" "$v-$cpu")
done

s=$tmp/$v-src
mkdir -p "$s"
git -C "$src" archive --prefix=np2_gdi/ HEAD -- . ':!bin' ':!obj' ':!np2tool/np2tool.zip' \
    ':!*.ncb' ':!*.opt' | tar -x -C "$s"
mkdir -p "$s/nt4ports-np2_gdi"
cp -R "$here"/. "$s/nt4ports-np2_gdi/"
(cd "$tmp" && rm -f "$out/$v-src.zip" && zip -qrX "$out/$v-src.zip" "$v-src")
ls -l "$out"/$v-*.zip
