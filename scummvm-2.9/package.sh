#!/bin/sh
# Make the release zips of ScummVM 2.9.1 for PINT (NT 4.0 on ARM) from pint/build.sh's build:
#   scummvm-2.9/package.sh /path/to/scummvm-2.9.1 /path/to/SDL-1.2.15 OUTDIR
# scummvm-2.9.1 is the patched source holding obj-arm. Makes scummvm-2.9.1-nt4-arm.zip and
# scummvm-2.9.1-nt4-src.zip: ScummVM's and SDL's source as released, zlib, and the port's
# folders (this one, scummvm/sdl for SDL's configuration, and filezilla/nt4/sdkrun.sh).
set -eu
here=$(cd "$(dirname "$0")" && pwd)
top=$(cd "$here/.." && pwd)
usage="usage: package.sh scummvm-2.9.1 SDL-1.2.15 OUTDIR"
src=$(cd "${1:?$usage}" && pwd)
sdl=$(cd "${2:?$usage}" && pwd)
mkdir -p "${3:?$usage}"
out=$(cd "$3" && pwd)
v=scummvm-2.9.1-nt4
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

crlf() { sed 's/\r$//; s/$/\r/' "$1" > "$2"; }

d=$tmp/$v-arm
mkdir -p "$d"
cp "$src/obj-arm/scummvm.exe" "$d/scummvm.exe"
# An empty scummvm.ini beside the exe: ScummVM's portable mode, which keeps its settings,
# saved games and log in this folder. NT 4.0 without Internet Explorer 5 has no
# per-user Application Data folder for ScummVM to find.
: > "$tmp/ini"; crlf "$tmp/ini" "$d/scummvm.ini"
cat > "$tmp/bat" <<'EOF'
@echo off
rem ScummVM with no sound, for a machine whose sound device takes sound but never plays
rem it (QEMU's Raspberry Pi): games that time themselves by their music wait for ever there.
set SDL_AUDIODRIVER=dummy
start scummvm.exe %1 %2 %3 %4 %5 %6 %7 %8 %9
EOF
crlf "$tmp/bat" "$d/scummvm-nosound.bat"
crlf "$src/COPYING" "$d/COPYING.TXT"
crlf "$src/COPYRIGHT" "$d/COPYRIGHT.TXT"
crlf "$sdl/COPYING" "$d/COPYING-SDL.TXT"
crlf "$top/filezilla/zlib/LICENSE" "$d/COPYING-ZLIB.TXT"
mkdir -p "$d/LICENSES"
for f in "$src"/LICENSES/*; do
    n=$(basename "$f"); case $n in *.txt) ;; *) n=$n.TXT ;; esac
    crlf "$f" "$d/LICENSES/$n"
done
engines=$(tr ' ' '\n' < "$src/obj-arm/defines.txt" | sed -n 's/^-DENABLE_//p' | tr 'A-Z' 'a-z' |
    tr '\n' ' ' | fold -s -w 88)
cat > "$tmp/readme" <<EOF
ScummVM 2.9.1 for Windows NT 4.0 on ARM (PINT) (unofficial port).
ScummVM runs classic adventure and role-playing games from their original data files.
This is ScummVM 2.9.1 built with SDL 1.2.15 (GDI and waveOut, no DirectX) and zlib by
scummvm-2.9/ in github.com/danifunker/nt4ports; the source is in $v-src.zip.

Run scummvm.exe and use Add Game, or from a prompt:
  scummvm --path=C:\\games\\tentacle --auto-detect
No game is included. Keep scummvm.ini beside scummvm.exe: it makes ScummVM keep its
settings, saved games and log (scummvm.log) in this folder.

Engines built in:
$engines
Not built in: MP3, Ogg Vorbis, FLAC, MPEG-2, Theora, JPEG and PNG decoding, FreeType,
OpenGL and 3D, the MT-32 emulator, translations, text to speech and networking; nor the
engines that need them. Games or files that need those won't run or play those parts.

If a game stops on its first screen (Sierra's games do this in QEMU, whose Raspberry Pi has
no sound), the sound device takes sound but doesn't play it: run scummvm-nosound.bat.
Ctrl+Q doesn't quit a game; the window's close button and Alt+F4 do.

Tested on PINT 4.0 in QEMU (raspi2b): the launcher; Day of the Tentacle (SCUMM), Flight of
the Amazon Queen, Simon the Sorcerer (AGOS), Gobliiins (Gob) and Gold Rush! (AGI) start and
play; Conquests of the Longbow (SCI) reaches its menu with no sound. Not tried on a real
Raspberry Pi yet, and nobody has listened to the sound.
Licences: COPYING.TXT (ScummVM, GPL v3 or later), COPYRIGHT.TXT, LICENSES\\ (code in
ScummVM under other licences), COPYING-SDL.TXT (SDL, LGPL v2.1), COPYING-ZLIB.TXT (zlib).
SDL and zlib are linked statically; their source and ScummVM's are in $v-src.zip.
EOF
crlf "$tmp/readme" "$d/README-PORT.TXT"
(cd "$tmp" && rm -f "$out/$v-arm.zip" && zip -qrX "$out/$v-arm.zip" "$v-arm")

s=$tmp/$v-src
mkdir -p "$s/scummvm-2.9.1"
# ScummVM as released: the source without the patch, the build or its git folder, nor the
# engine data lists create_project writes into dists.
(cd "$src" && tar -c --exclude=./obj-arm --exclude=./.git \
    --exclude='./dists/scummvm_rc_engine_data*.rh' .) | tar -x -C "$s/scummvm-2.9.1"
(cd "$s/scummvm-2.9.1" && patch -R -p1 --binary -s < "$here/patches/scummvm-2.9.1-nt4.diff")
mkdir -p "$s/SDL-1.2.15"
(cd "$sdl" && tar -c --exclude=./include/SDL_config.h .) | tar -x -C "$s/SDL-1.2.15"
mkdir -p "$s/nt4ports/scummvm" "$s/nt4ports/filezilla/nt4"
cp -R "$here" "$s/nt4ports/scummvm-2.9"
cp -R "$top/scummvm/sdl" "$s/nt4ports/scummvm/sdl"
cp -R "$top/filezilla/zlib" "$s/nt4ports/filezilla/zlib"
cp "$top/filezilla/nt4/sdkrun.sh" "$s/nt4ports/filezilla/nt4/"
cat > "$tmp/building" <<'EOF'
ScummVM 2.9.1 and SDL 1.2.15 as released, and nt4ports: the port's folders from
github.com/danifunker/nt4ports (scummvm-2.9: the patch, headers and build; scummvm/sdl: SDL's
configuration; filezilla/zlib: zlib 1.3.1; filezilla/nt4/sdkrun.sh: runs the PINT SDK).
On Linux with the PINT 4.0 SDK (Docker's tenox7/pint40sdk) and g++, in this folder:

  cd scummvm-2.9.1 && patch -p1 --binary < ../nt4ports/scummvm-2.9/patches/scummvm-2.9.1-nt4.diff && cd ..
  nt4ports/scummvm-2.9/pint/build.sh scummvm-2.9.1 SDL-1.2.15

More in nt4ports/scummvm-2.9/README.md.
EOF
crlf "$tmp/building" "$s/BUILDING.TXT"
(cd "$tmp" && rm -f "$out/$v-src.zip" && zip -qrX "$out/$v-src.zip" "$v-src")
ls -l "$out"/$v-*.zip
