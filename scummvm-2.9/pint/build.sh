#!/bin/sh
# Build ScummVM 2.9.1 for PINT (Windows NT 4.0 on ARM) with the PINT 4.0 SDK, with
# SDL 1.2.15 and zlib, on Linux:
#   build.sh /path/to/scummvm-2.9.1 /path/to/SDL-1.2.15
# ScummVM needs patches/scummvm-2.9.1-nt4.diff. Output: obj-arm/scummvm.exe in its folder.
# ENGINES: the engines to build, comma separated (default below). JOBS: compiles at a time.
#
# The source list and defines come from ScummVM's own devtools/create_project, as for a
# Visual Studio build: it's built with the host's g++ and run into obj-arm/proj.
# The SDK runs as filezilla/nt4/sdkrun.sh does: Docker's tenox7/pint40sdk, or an unpacked
# copy (PINT_SDK, PINT_MSVC).
set -eu
here=$(cd "$(dirname "$0")" && pwd)
top=$(cd "$here/../.." && pwd)
usage="usage: build.sh scummvm-2.9.1 SDL-1.2.15"
fz=$(cd "${1:?$usage}" && pwd)
sdl=$(cd "${2:?$usage}" && pwd)
# The engines create_project accepts with no external library but zlib, except four too big
# or slow for a Raspberry Pi under NT 4.0: Blade Runner, Ultima, mTropolis and Director.
engines=${ENGINES:-scumm,scumm_7_8,he,access,adl,agi,agos,agos2,bbvs,cge,cge2,chewy,cine,\
composer,cruise,dgds,draci,drascula,dreamweb,efh,gnap,gob,griffon,groovie,hopkins,hugo,\
illusions,kingdom,kyra,lol,eob,lab,lure,made,mads,mm,mm1,xeen,mohawk,riven,myst,mortevielle,\
neverhood,parallaction,pegasus,pink,plumbers,prince,private,queen,saga,ihnm,sci,sci32,\
sherlock,sky,startrek,supernova,sword1,sword2,teenagent,tinsel,toltecs,tony,toon,touche,\
trecision,tsage,tucker,twine,voyeur}
jobs=${JOBS:-6}
o=$fz/obj-arm
. "$top/filezilla/nt4/sdkrun.sh"

rm -rf "$o"; mkdir -p "$o/proj" "$o/tools"
# ScummVM's project generator, for the source list, the defines and the engine tables.
(cd "$fz/devtools/create_project" && g++ -O1 -o "$o/tools/create_project" \
    create_project.cpp cmake.cpp codeblocks.cpp msvc.cpp msbuild.cpp xcode.cpp)
off=""
# No external libraries but zlib, and none of the features NT 4.0 or PINT can't have
# (OpenGL, the system dialogs of Vista, text to speech, cloud and network, language
# detection of Windows 2000) or that need data files not included (translations, MT-32).
for f in mad fribidi ogg vorbis flac png mpeg2 theoradec freetype2 jpeg fluidsynth libcurl \
         sdlnet hqscalers edgescalers 3d imgui mt32emu lua tinygl opengl opengl_game_classic \
         opengl_game_shaders taskbar cloud enet translation dialogs langdetect tts nasm; do
    off="$off --disable-$f"
done
(cd "$o/proj" && "$o/tools/create_project" "$fz" --msvc --msvc-version 17 --sdl1 \
    --use-windows-ansi --disable-all-engines --enable-engine="$engines" $off > create_project.log)
defs=$(sed -n 's/.*<PreprocessorDefinitions>\([^<]*\)<.*/\1/p' "$o/proj/ScummVM_Globalx86.props" |
    head -1 | tr ';' '\n' | grep -v '^%' | sed 's/^/-D/' | tr '\n' ' ')
for p in "$o"/proj/*.vcxproj; do
    sed -n 's/.*<ClCompile Include="\([^"]*\)".*/\1/p' "$p"
done | sed 's,\\,/,g; s,^'"$fz"'/,,' > "$o/svm.txt"
echo "$defs" > "$o/defines.txt"
echo "$(wc -l < "$o/svm.txt") ScummVM sources; $defs"

cp "$top/scummvm/sdl/SDL_config.h" "$sdl/include/SDL_config.h"
cp "$top/scummvm/sdl/csrc.txt" "$o/sdl.txt"
rm -rf "$o/zlib" "$o/inc"; cp -R "$top/filezilla/zlib" "$o/zlib"
cp -R "$here/include" "$o/inc"; cp "$here/src/pint_crt.cpp" "$o/pint_crt.cpp"
cat > "$o/cc.sh" <<'CC'
# cc.sh FLAGS NAME SOURCE: compile SOURCE to $O/obj/NAME.obj with the flags in variable FLAGS.
eval "flags=\$$1"
cl $flags "$3" -Fo$O/obj/$2.obj > $O/log/$2.log 2>&1 ||
    { cat $O/log/$2.log; echo "COMPILE FAILED $3"; exit 255; }
CC

pint_run '
set -e
cd /svm
o=obj-arm jobs='"$jobs"'
D="-DWIN32 -D_WINDOWS -DNDEBUG -DRELEASE_BUILD '"$defs"'"
# The C++ runtime helpers of MSVC 14 that the NT 4.0 runtime lacks are switched off.
F="-nologo -c -MT -W1 -GS- -Zc:sizedDealloc- -Zc:threadSafeInit- -Zc:tlsGuards-"
# ScummVM uses no exceptions: without them, MSVC 14 needs fewer helpers from the runtime.
X="$F -O2 -EHs-c- -GR -Zc:__cplusplus -utf-8 -std:c++17 -FIpint_svm.h"
I="-I$o/inc -I$o/proj -I. -Iengines -I/sdl/include -I$o/zlib"
# One line per file: its flags (CF for SDL and zlib, CRT, SVM), object name and source.
# pint_crt.cpp without pint_svm.h, whose C++ float overloads would make its float functions
# call themselves, and unoptimised: the optimiser turns (float)tan((double)x) into tanf(x).
CF="$F -O2 -I/sdl/include"; CRT="$F -EHs-c- -Od -Oi-"; SVM="$X $I $D"; O=$o
export CF CRT SVM O
{
    for f in $(cat $o/sdl.txt); do echo "CF sdl_$(echo $f | tr / _ | sed "s/\.c\$//") /sdl/$f"; done
    for f in $o/zlib/*.c; do echo "CF zlib_$(basename $f .c) $f"; done
    echo "CRT pint_crt $o/pint_crt.cpp"
    for f in $(cat $o/svm.txt); do echo "SVM $(echo $f | tr / _ | sed "s/\.cpp\$//") $f"; done
} > $o/list
mkdir -p $o/obj $o/log
# One cl per file, several at a time; a failure prints its log and stops the build.
xargs -P$jobs -L1 sh $o/cc.sh < $o/list
rc -nologo -i $o/inc -i $o/proj -i . $D -fo $o/scummvm.res dists/scummvm.rc
# A response file: the object list is longer than a Windows command line. ScummVM brings
# its own WinMain (backends/platform/sdl/win32/win32-main.cpp), as in its Windows releases.
ls $o/obj/*.obj > $o/link.rsp; echo $o/scummvm.res >> $o/link.rsp
link -nologo -subsystem:windows -OUT:$o/scummvm.exe @$o/link.rsp \
    kernel32.lib user32.lib gdi32.lib winmm.lib advapi32.lib shell32.lib ole32.lib imm32.lib \
    version.lib setupapi.lib > $o/link.log 2>&1 || { tail -60 $o/link.log; exit 1; }
' "$fz:/svm" "$sdl:/sdl"
ls -l "$o/scummvm.exe"
