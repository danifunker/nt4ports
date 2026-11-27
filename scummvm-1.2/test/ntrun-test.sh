#!/bin/sh
# Run the Alpha build in ntsdk's Alpha emulator (ntrun), which has no screen or sound:
#   VC6=/path/to/vc6 test/ntrun-test.sh /path/to/scummvm-1.2.1 GAMES TARGET:GAMEID...
# GAMES holds a folder per game; each TARGET is one of them, run as GAMEID (1.2.1 calls SCI
# games "sci", AGI games "agi" and Gobliiins "gob"). Example:
#   test/ntrun-test.sh scummvm-1.2.1 ~/games longbow:sci goldrush:agi queen:queen
# It builds test/winmm.c (ntrun can't start NT's winmm.dll), runs ScummVM's detector over the
# games, then each game for SECONDS (default 120) with SDL's dummy video and sound and
# ScummVM's debug output. Output: obj-alpha/ntrun-test/.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
usage="usage: ntrun-test.sh scummvm-1.2.1 GAMES TARGET:GAMEID..."
src=$(cd "${1:?$usage}" && pwd)
games=$(cd "${2:?$usage}" && pwd)
shift 2
[ $# -gt 0 ] || { echo "$usage" >&2; exit 2; }
vc6=$(cd "${VC6:?set VC6 to the folder with VC98 and COMMON of the Visual C++ 6.0 for Alpha CD}" && pwd)
secs=${SECONDS_EACH:-120}
t=$src/obj-alpha/ntrun-test
mkdir -p "$t/rt" "$t/prof"
cp "$here/winmm.c" "$here/winmm.def" "$t/"
V='Z:\vc6\VC98'
docker run --rm -v "$t":/t -w /t -v "$vc6":/vc6:ro \
    -e "NTRUN_PATH=$V\\BIN;Z:\\vc6\\COMMON\\MSDEV98\\BIN" -e "INCLUDE=$V\\INCLUDE" -e "LIB=$V\\LIB" \
    tenox7/ntsdk nt40 alpha sh -c 'cl -nologo -O2 -MT -LD winmm.c -Fowinmm.obj \
        -link -def:winmm.def -out:rt/winmm.dll > winmm.log 2>&1 || { cat winmm.log; exit 1; }'
: > "$t/test.ini"
list=
for a in "$@"; do
    tgt=${a%%:*} id=${a#*:}
    printf '[%s]\r\ngameid=%s\r\npath=Z:\\g\\%s\r\n\r\n' "$tgt" "$id" "$tgt" >> "$t/test.ini"
    list="$list $tgt"
done
# NT 4.0 always sets USERPROFILE, which ScummVM keeps its settings under; ntrun doesn't.
docker run --rm -v "$src/obj-alpha":/w -v "$games":/g:ro -w /w tenox7/ntsdk nt40 alpha sh -c '
export NTRUN_DLLPATH="/w/ntrun-test/rt:/opt/sys/alpha" USERPROFILE="Z:\\w\\ntrun-test\\prof"
ini="Z:\\w\\ntrun-test\\test.ini"
ntrun scummvm.exe --version
ntrun scummvm.exe --config="$ini" --test-detector 2>&1 | tee ntrun-test/detector.txt
export SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
for g in '"$list"'; do
    timeout '"$secs"' ntrun scummvm.exe --config="$ini" -d2 \
        --debugflags=Room,Scripts,Main,Resources $g > ntrun-test/run-$g.txt 2>&1 || true
    echo "$g: $(wc -l < ntrun-test/run-$g.txt) lines of output in '"$secs"' s"
done'
