#!/bin/sh
# Make the release zips of ScummVM 1.2.1 for NT 4.0 on Alpha from ntsdk/build.sh's build:
#   scummvm-1.2/package.sh /path/to/scummvm-1.2.1 /path/to/SDL-1.2.15 OUTDIR
# scummvm-1.2.1 is the patched source holding obj-alpha. Makes scummvm-1.2.1-nt4-alpha.zip and
# scummvm-1.2.1-nt4-src.zip: ScummVM's and SDL's source as released, and the port's folders
# (this one, scummvm/sdl for SDL's configuration, filezilla/zlib).
set -eu
here=$(cd "$(dirname "$0")" && pwd)
top=$(cd "$here/.." && pwd)
usage="usage: package.sh scummvm-1.2.1 SDL-1.2.15 OUTDIR"
src=$(cd "${1:?$usage}" && pwd)
sdl=$(cd "${2:?$usage}" && pwd)
mkdir -p "${3:?$usage}"
out=$(cd "$3" && pwd)
v=scummvm-1.2.1-nt4
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

crlf() { sed 's/\r$//; s/$/\r/' "$1" > "$2"; }

d=$tmp/$v-alpha
mkdir -p "$d"
cp "$src/obj-alpha/scummvm.exe" "$d/scummvm.exe"
# The engines' data files and the two GUI themes, which ScummVM finds in the current folder.
for f in drascula.dat kyra.dat lure.dat queen.tbl sky.cpt teenagent.dat; do
    cp "$src/dists/engine-data/$f" "$d/"
done
cp "$src/gui/themes/scummclassic.zip" "$src/gui/themes/scummmodern.zip" "$d/"
crlf "$src/README" "$d/README.TXT"
crlf "$src/COPYING" "$d/COPYING.TXT"
crlf "$src/COPYING.LGPL" "$d/COPYING-LGPL.TXT"
crlf "$src/COPYING.BSD" "$d/COPYING-BSD.TXT"
crlf "$src/COPYRIGHT" "$d/COPYRIGHT.TXT"
crlf "$sdl/COPYING" "$d/COPYING-SDL.TXT"
crlf "$top/filezilla/zlib/LICENSE" "$d/COPYING-ZLIB.TXT"
cat > "$tmp/readme" <<EOF
ScummVM 1.2.1 for Windows NT 4.0 on Alpha (unofficial port).
ScummVM runs classic adventure games from their original data files. 1.2.1 is the first
ScummVM with Sierra's SCI games (King's Quest IV to VI, Space Quest III to V, Leisure Suit
Larry 2 to 6, Quest for Glory I to III, Police Quest II and III and others), and has its AGI
ones (King's Quest I to III, Space Quest I and II, Gold Rush! and others), with LucasArts',
Simon the Sorcerer, Kyrandia, Gobliiins and more: README.TXT, ScummVM's own, lists them.
It is ScummVM 1.2.1 with SDL 1.2.15 (GDI and waveOut, no DirectX) and zlib, built on Linux
with Visual C++ 6.0 for Alpha by scummvm-1.2/ in github.com/danifunker/nt4ports; the source
is in $v-src.zip.

Unzip it and run scummvm.exe from its own folder (the engines' data files and the themes
are found there), then Add Game; or from a prompt there:
  scummvm --path=C:\\games\\longbow sci
No game is included. Settings are kept in %USERPROFILE%\\Application Data\\ScummVM\\scummvm.ini;
saved games go in the current folder unless a save path is set in the options.

Not in this build: MP3, Ogg Vorbis and FLAC, the MT-32 emulator, the assembly scalers and
translations; nor DOSBox's OPL emulator, so AdLib music plays through MAME's. Speech, music
and video compressed with ScummVM's tools don't play. The program opens a console window
too: warnings and errors go there. Run the desktop in 65536 colours or more.

NOT YET RUN ON AN ALPHA. It was run only in ntsdk's Alpha emulator (ntrun), with no screen
or sound: there ScummVM detects Gold Rush!, Conquests of the Longbow, Gobliiins, Flight of
the Amazon Queen, Simon the Sorcerer and Day of the Tentacle, and Longbow (SCI) and Gold
Rush! (AGI) run their scripts. Reports from a real Alpha are welcome.
Licences: COPYING.TXT (ScummVM, GPL v2 or later), COPYRIGHT.TXT, COPYING-LGPL.TXT and
COPYING-BSD.TXT (parts of ScummVM), COPYING-SDL.TXT (SDL, LGPL v2.1), COPYING-ZLIB.TXT (zlib).
SDL and zlib are linked statically; their source and ScummVM's are in $v-src.zip.
EOF
crlf "$tmp/readme" "$d/README-PORT.TXT"
(cd "$tmp" && rm -f "$out/$v-alpha.zip" && zip -qrX "$out/$v-alpha.zip" "$v-alpha")

s=$tmp/$v-src
mkdir -p "$s/scummvm-1.2.1"
# ScummVM as released: the source without the patch or the build.
(cd "$src" && tar -c --exclude=./obj-alpha --exclude=./.git .) | tar -x -C "$s/scummvm-1.2.1"
(cd "$s/scummvm-1.2.1" && patch -R -p1 --binary -s < "$here/patches/scummvm-1.2.1-nt4.diff")
mkdir -p "$s/SDL-1.2.15"
(cd "$sdl" && tar -c --exclude=./include/SDL_config.h .) | tar -x -C "$s/SDL-1.2.15"
mkdir -p "$s/nt4ports/scummvm" "$s/nt4ports/filezilla"
cp -R "$here" "$s/nt4ports/scummvm-1.2"
cp -R "$top/scummvm/sdl" "$s/nt4ports/scummvm/sdl"
cp -R "$top/filezilla/zlib" "$s/nt4ports/filezilla/zlib"
cat > "$tmp/building" <<'EOF'
ScummVM 1.2.1 and SDL 1.2.15 as released, and nt4ports: the port's folders from
github.com/danifunker/nt4ports (scummvm-1.2: the patch, forced header and build; scummvm/sdl:
SDL's configuration; filezilla/zlib: zlib 1.3.1).
On Linux with Docker (tenox7/ntsdk) and the Visual C++ 6.0 for Alpha CD unpacked to a
folder VC6 (its VC98 and COMMON folders), in this folder:

  cd scummvm-1.2.1 && patch -p1 --binary < ../nt4ports/scummvm-1.2/patches/scummvm-1.2.1-nt4.diff && cd ..
  VC6=/path/to/vc6 nt4ports/scummvm-1.2/ntsdk/build.sh scummvm-1.2.1 SDL-1.2.15

More in nt4ports/scummvm-1.2/README.md.
EOF
crlf "$tmp/building" "$s/BUILDING.TXT"
(cd "$tmp" && rm -f "$out/$v-src.zip" && zip -qrX "$out/$v-src.zip" "$v-src")
ls -l "$out"/$v-*.zip
