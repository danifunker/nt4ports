# ScummVM 1.2.1 for NT 4.0 on Alpha

Licence: GPL v2 or later, see `LICENSE`. SDL is LGPL v2.1 (`../scummvm/licenses`), zlib is
in `../filezilla/zlib`. ScummVM is by the ScummVM team; this port is unofficial.

ScummVM 1.2.1 (December 2010) is the first ScummVM with Sierra's SCI games: King's Quest IV
to VI, Space Quest III to V, Leisure Suit Larry 2 to 6, Quest for Glory I to III, Police
Quest II and III, Conquests of the Longbow and the rest of SCI0 to SCI1.1. It also has the
AGI games (since 0.10, 2007) and every engine of 0.6.1 and many more. The later SCI games
(Gabriel Knight, King's Quest VII, Phantasmagoria) came only in 2.0.

It's built for Alpha only, with Visual C++ 6.0 for Alpha (the Visual C++ 6.0 CD for Alpha,
run by [tenox7/ntsdk](https://github.com/tenox7/ntsdk)). Visual C++ 4.1, the only compiler
here for MIPS and PowerPC, stops at its limit of 100 errors in ScummVM's shared headers, which
every file includes (`../scummvm` is 0.6.1 for those).
SDL 1.2.15 is built as for `../scummvm` (GDI video, `waveOut` sound, no DirectX), and zlib
for the GUI's themes and compressed saves.

- `patches/scummvm-1.2.1-nt4.diff`: every change to ScummVM, against its `v1.2.1` tag.
- `compat/svm_vc6.h`: the forced header (`-FI`).
- `csrc.txt`: the sources, from ScummVM's own Visual Studio projects (`dists/msvc9`), less
  the MT-32 emulator.
- `ntsdk/build.sh`: the build. `package.sh`: the release zips.
- `test/`: running the build in ntsdk's Alpha emulator (below).

## Building

    curl -L https://github.com/scummvm/scummvm/archive/refs/tags/v1.2.1.tar.gz | tar -xz
    curl -L https://github.com/libsdl-org/SDL-1.2/archive/refs/tags/release-1.2.15.tar.gz | tar -xz
    mv SDL-1.2-release-1.2.15 SDL-1.2.15
    cd scummvm-1.2.1 && patch -p1 --binary < ../scummvm-1.2/patches/scummvm-1.2.1-nt4.diff && cd ..
    VC6=/path/to/vc6 scummvm-1.2/ntsdk/build.sh scummvm-1.2.1 SDL-1.2.15   # obj-alpha/scummvm.exe
    scummvm-1.2/package.sh scummvm-1.2.1 SDL-1.2.15 OUTDIR

`VC6` is the folder with the CD's `VC98` and `COMMON` folders; `../filezilla/nt4/get-vc6.sh`
unpacks them from the ISO. On 10 cores the build takes about 25 minutes.

The engines are those of ScummVM's Visual Studio projects: the ones 1.2.1 builds by default
(not Hugo, M4, Mohawk, TestBed, Lands of Lore, SAGA 2, Groovie 2 or SCI32). MP3, Ogg Vorbis, FLAC,
translations and the assembly scalers are off. So is DOSBox's OPL emulator
(`DISABLE_DOSBOX_OPL`): it takes the addresses of function templates that differ only in
their template arguments, which VC6 can't tell apart (below). AdLib music plays through
MAME's emulator instead.

## What Visual C++ 6.0 needed

VC6 (1998) predates the C++ standard in places. The patch keeps to what it can do; most of
it is in `common/` and repeats across the engines.

- **Template functions merged at link time.** VC6 gives every instantiation of a function
  template the same name when the template arguments aren't in its parameter types, and the
  linker keeps one: `f<1>()` and `f<2>()` both run `f<2>`. The scalers (`ColorMask`), the
  colour masks, Kyra's and the SCUMM HE blitters, the rate converters and Paula all have
  such templates. The patch gives each an unused last parameter of type `VC6Int<N>` or
  `VC6Type<T>` (from the forced header), defaulted to 0, which puts the arguments in the
  name; the object files show one name per instantiation. The SCUMM HE (Wiz) blitters,
  where VC6 crashed on that, take their `type` as an ordinary argument instead.
- **No `return` of a void expression in a template**: void versions of
  `Common::Functor0Mem` and `Functor1Mem` (`Functor0MemVoid`, `Functor1MemVoid`), used by
  the engines whose opcodes return nothing.
- **No in-class initialisers for `static const` integers**: enums.
- **No partial ordering of function templates**: one `Common::sort`, `set_to` and
  `distance` for pointers and iterators alike.
- **No argument-dependent lookup**: `using` declarations at global scope for the
  `Common::` and `Audio::` functions engines call unqualified.
- **No template friends, and no nested class as a friend of another**: every member is
  public (`#define private public` in the forced header). Access is only checked, and
  every file is built the same way, so it changes no code.
- **No member templates defined outside their class**: Tucker's savegame sync, the SJIS
  font's blitter and Parallaction's `freeList` became functions outside their classes or
  were defined in them.
- **No aggregate initialisers for structures with `Common::Point` or `Common::Rect`
  members, or with references, and no empty `{ }`**: set field by field, pointers for
  references, `{ 0 }`.
- **No `delete` of a pointer to const**: casts.
- **Covariant return types, namespace-qualified calls of a base class's members
  (`GUI::Dialog::open()`), and a base class's own namespace found through the base** are
  written the way VC6 reads them.
- **Compiler crashes**: VC6 fails (C1001) on a class template with a non-type parameter
  after a global function declared with a namespace's type. `engines/util.h`'s
  `initGraphics` names its types through typedefs, and `engines/agi/detection.cpp` includes
  `common/stack.h` first. `-Zm500` gives the compiler the heap the built-in theme needs.
- Alpha faults on unaligned reads: `SCUMM_NEED_ALIGNMENT`, and `main` asks NT to fix up any
  that are left (`SEM_NOALIGNMENTFAULTEXCEPT`), as in `../scummvm`.
- It links as a console program (warnings and errors go to its window) that starts at
  ScummVM's own `WinMain`, as SDL renames `main`.

## Testing without an Alpha

    VC6=/path/to/vc6 scummvm-1.2/test/ntrun-test.sh scummvm-1.2.1 GAMES longbow:sci goldrush:agi

ntsdk's `ntrun` runs Alpha programs on Linux, emulating the CPU and NT's system DLLs, but
with no screen or sound and without NT's `winmm.dll`. `test/winmm.c` is a stand-in for
that (no devices; a timer for SDL). The script runs ScummVM's own detector test over the
games and each game for two minutes with SDL's dummy video and sound, writing ScummVM's
debug output.

## Status

On an AlphaStation 200 FPGA core on a MiSTer (NT 4.0 Terminal Server Edition, 640x480 in
256 colours, the most its TGA card offers):

- The launcher, with the modern theme and its game list. Day of the Tentacle's intro plays
  through, and its autosave works. Conquests of the Longbow (SCI) gets from Sierra's logo to
  its title screen in about 30 s and its main menu (Start, Restore, Introduction) within
  45 s, with the default settings.
- At 22 kHz that core can't emulate AdLib music in real time, and SCI games, which time
  their scenes by their music, crawl: Longbow sat on its logo for minutes at 100% CPU. At
  11 kHz, or with no music, it keeps up, so 11 kHz is this build's default output rate
  (`backends/platform/sdl/sdl.cpp`; Options > Audio or `output_rate` still change it).
- `waveOut` plays there in real time (a test program had 39 of 40 quarter-second buffers
  back on time), but the sound wasn't listened to: the MiSTer's video capture records none.
- ScummVM draws in 16-bit colour and SDL maps each frame to the 256-colour screen, which
  costs time and makes 256-colour games look nearly 16-colour.
- The window's close button quits; Alt+F4 doesn't reach it.

In ntrun: `--version`, `--list-games`, and the detector test, which finds Gold Rush! (AGI),
Conquests of the Longbow (SCI), Gobliiins, Flight of the Amazon Queen, Simon the Sorcerer
and Day of the Tentacle. Gold Rush!, Gobliiins, Flight of the Amazon Queen and Simon the
Sorcerer weren't started on the MiSTer.
