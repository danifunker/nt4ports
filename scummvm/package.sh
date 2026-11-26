#!/bin/sh
# Make the release zips of ScummVM 0.6.1 for NT 4.0 from the builds of ntsdk/build.sh and
# pint/build.sh:
#   scummvm/package.sh /path/to/scummvm-0.6.1 /path/to/SDL-1.2.15 OUTDIR
# scummvm-0.6.1 is the patched source holding obj-alpha, obj-mips, obj-ppc and obj-arm.
# Makes scummvm-0.6.1-nt4-CPU.zip for each, and scummvm-0.6.1-nt4-src.zip: ScummVM's source
# as released, SDL's, and this folder (the patch, the SDL configuration and the builds).
set -eu
here=$(cd "$(dirname "$0")" && pwd)
usage="usage: package.sh scummvm-0.6.1 SDL-1.2.15 OUTDIR"
src=$(cd "${1:?$usage}" && pwd)
sdl=$(cd "${2:?$usage}" && pwd)
mkdir -p "${3:?$usage}"
out=$(cd "$3" && pwd)
v=scummvm-0.6.1-nt4
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

crlf() { sed 's/\r$//; s/$/\r/' "$1" > "$2"; }

for cpu in alpha mips ppc arm; do
    case $cpu in
    alpha) what="Alpha, Visual C++ 4.1 (tenox7/ntsdk)"; tested="Not yet run." ;;
    mips)  what="MIPS, Visual C++ 4.1 (tenox7/ntsdk)"; tested="Not yet run: there was no MIPS machine to test on." ;;
    ppc)   what="PowerPC, Visual C++ 4.1 (tenox7/ntsdk)"
           tested="Tested on NT 4.0 SP2 (QEMU 40P): the launcher, Day of the Tentacle (play, save,
load) and Flight of the Amazon Queen. That machine has no sound card." ;;
    arm)   what="ARM (PINT), Microsoft C for ARM (PINT 4.0 SDK)"
           tested="Tested on PINT 4.0 (QEMU raspi2b): the launcher, Day of the Tentacle and Flight
of the Amazon Queen. The sound mixer starts there, but nothing was listened to." ;;
    esac
    d=$tmp/$v-$cpu
    mkdir -p "$d"
    cp "$src/obj-$cpu/scummvm.exe" "$d/scummvm.exe"
    crlf "$src/COPYING" "$d/COPYING.TXT"
    crlf "$sdl/COPYING" "$d/COPYING-SDL.TXT"
    crlf "$src/README" "$d/README.TXT"
    cat > "$tmp/readme" <<EOF
ScummVM 0.6.1 for Windows NT 4.0 (unofficial port).
ScummVM runs LucasArts' SCUMM adventures, Simon the Sorcerer 1 and 2, Beneath a Steel Sky,
Flight of the Amazon Queen and Broken Sword 1 and 2 from their original data files. This is
ScummVM 0.6.1 with SDL 1.2.15 (GDI and waveOut, no DirectX), built on Linux by scummvm/ in
github.com/danifunker/nt4ports; the source is in $v-src.zip.

Run scummvm.exe and Add Game, or from a prompt: scummvm -p C:\\games\\tentacle tentacle
README.TXT is ScummVM's own, and lists the games, their files and the keys. No game is
included. Settings are kept in scummvm.ini in the Windows folder; saved games go in the
current folder unless savepath is set.

Not in this build: MP3, Ogg Vorbis, zlib or mpeg2. Speech, music and video compressed
with ScummVM's tools don't play, and nor do Broken Sword 1 and 2's cutscenes; those two
games are untested. The program opens a console window too: warnings and errors go there.
Run the desktop in 65536 colours or more: ScummVM draws in 16-bit colour, and at 256 or 16
colours Windows maps it to the screen's palette badly.

This build: $what.
$tested
No one has listened to the sound on any CPU yet.
Licences: COPYING.TXT (ScummVM, GPL v2), COPYING-SDL.TXT (SDL, LGPL v2.1). SDL is linked
statically; its source and ScummVM's are in $v-src.zip.
EOF
    crlf "$tmp/readme" "$d/README-PORT.TXT"
    (cd "$tmp" && rm -f "$out/$v-$cpu.zip" && zip -qrX "$out/$v-$cpu.zip" "$v-$cpu")
done

s=$tmp/$v-src
mkdir -p "$s"
# ScummVM as released: the source without the patch, the build's objects or its lists.
mkdir -p "$s/scummvm-0.6.1"
(cd "$src" && tar -c --exclude='./obj-*' --exclude='./.ntsdk-*' --exclude='./.pint-*' \
    --exclude=./.git --exclude=./.git-ignore-local .) | tar -x -C "$s/scummvm-0.6.1"
(cd "$s/scummvm-0.6.1" && patch -R -p1 --binary -s < "$here/patches/scummvm-0.6.1-nt4.diff")
mkdir -p "$s/SDL-1.2.15"
(cd "$sdl" && tar -c --exclude=./include/SDL_config.h .) | tar -x -C "$s/SDL-1.2.15"
mkdir -p "$s/nt4ports-scummvm"
cp -R "$here"/. "$s/nt4ports-scummvm/"
cat > "$s/BUILDING.TXT" <<'EOF'
ScummVM 0.6.1 and SDL 1.2.15 as released, and nt4ports-scummvm: the patch, the SDL
configuration and the build scripts (Linux, Docker). In this folder:

  cd scummvm-0.6.1 && patch -p1 --binary < ../nt4ports-scummvm/patches/scummvm-0.6.1-nt4.diff && cd ..
  nt4ports-scummvm/ntsdk/build.sh alpha|mips|ppc scummvm-0.6.1 SDL-1.2.15
  nt4ports-scummvm/pint/build.sh scummvm-0.6.1 SDL-1.2.15

pint/build.sh needs filezilla/nt4/sdkrun.sh from github.com/danifunker/nt4ports.
More in nt4ports-scummvm/README.md.
EOF
crlf "$s/BUILDING.TXT" "$s/BUILDING.TMP" && mv "$s/BUILDING.TMP" "$s/BUILDING.TXT"
(cd "$tmp" && rm -f "$out/$v-src.zip" && zip -qrX "$out/$v-src.zip" "$v-src")
ls -l "$out"/$v-*.zip
