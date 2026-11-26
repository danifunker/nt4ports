# ScummVM 0.6.1 for NT 4.0

Licence: GPL v2, see `LICENSE`. SDL is LGPL v2.1, in `licenses/`. ScummVM is by the ScummVM
team; this port is unofficial.

ScummVM 0.6.1 (2004) runs LucasArts' SCUMM games, Simon the Sorcerer 1 and 2, Beneath a
Steel Sky, Flight of the Amazon Queen and Broken Sword 1 and 2. Visual C++ 4.1, ntsdk's
compiler for Alpha, MIPS and PowerPC, compiles it with the patch here; later versions are
larger and use more of the C++ that compiler lacks, and weren't tried. ScummVM draws and
plays sound through SDL 1.2.15 (SDL 2 needs Windows XP), built here with GDI video and
`waveOut` sound and no DirectX.

- `patches/scummvm-0.6.1-nt4.diff`: every change to ScummVM, against its `v0.6.1` tag
- `compat/svm_vc4.h`: a forced header for Visual C++ 4.1
- `sdl/SDL_config.h`: SDL's configuration for NT 4.0; SDL itself is unchanged.
  `sdl/csrc.txt` lists the SDL sources to build.
- `csrc.txt`: the ScummVM sources, from its `module.mk` files, for the SDL backend on Windows
- `ntsdk/build.sh`: Alpha, MIPS and PowerPC, with [tenox7/ntsdk](https://github.com/tenox7/ntsdk)
- `pint/build.sh`: ARM, with the PINT 4.0 SDK. `pint/include/winresrc.h` stands in for the
  one the SDK lacks.
- `package.sh`: the release zips. `scummvm-0.6.1-nt4-src.zip` holds ScummVM and SDL as
  released, plus this folder.

## Building

    curl -L https://github.com/scummvm/scummvm/archive/refs/tags/v0.6.1.tar.gz | tar -xz
    curl -L https://github.com/libsdl-org/SDL-1.2/archive/refs/tags/release-1.2.15.tar.gz | tar -xz
    mv SDL-1.2-release-1.2.15 SDL-1.2.15
    cd scummvm-0.6.1 && patch -p1 --binary < ../scummvm/patches/scummvm-0.6.1-nt4.diff && cd ..
    scummvm/ntsdk/build.sh alpha|mips|ppc scummvm-0.6.1 SDL-1.2.15   # obj-CPU/scummvm.exe
    scummvm/pint/build.sh scummvm-0.6.1 SDL-1.2.15                   # obj-arm/scummvm.exe
    scummvm/package.sh scummvm-0.6.1 SDL-1.2.15 OUTDIR               # after all four

The build scripts copy
`SDL_config.h` into SDL's `include` folder. `pint/build.sh` uses `filezilla/nt4/sdkrun.sh`
to run the SDK.

It's a console program, as ScummVM's own Windows builds were: warnings show in its window.
There's no MP3, Ogg Vorbis, zlib or mpeg2: speech, music and video compressed with
ScummVM's tools don't play, and nor do Broken Sword 1 and 2's cutscenes.

## Status

| | Alpha | MIPS | PowerPC | ARM (PINT) |
|---|---|---|---|---|
| Builds | Yes | Yes | Yes | Yes |
| Tested on | | | NT 4.0 SP2, QEMU 40P | PINT 4.0, QEMU raspi2b |
| Launcher, add-game browser | Not run | Not run | Yes | Launcher yes |
| Day of the Tentacle (SCUMM v6) | Not run | Not run | Plays, saves and loads | Plays |
| Flight of the Amazon Queen | Not run | Not run | Intro and game | Intro and game |
| Sound | Not run | Not run | No sound card in QEMU | Mixer starts; not heard |

Simon the Sorcerer, Beneath a Steel Sky and Broken Sword haven't been tried. At 16 or 256
colours, as the PowerPC VM's desktop is, Windows maps ScummVM's 16-bit picture to the
palette and the colours band; at 65536 colours or more it looks right, as on PINT.

## What the patch changes

Visual C++ 4.1 predates standard C++, so most of the patch works around the compiler.
Only the last two items change what ScummVM does with another compiler.

- No `bool`, `explicit`, `mutable` or `typename`, and the old scope of `for`:
  `compat/svm_vc4.h` defines them. With `bool` an `int`, `ConfigManager`'s overloads on
  `bool` and `int` clash, so the `bool` ones go under VC4. `getBool` already accepts `1`.
- No template default argument that uses an earlier parameter: `Common::Map` takes its
  comparator explicitly.
- Every member of a class template is compiled, so `Map` needs an assignment operator.
  `Map`'s copy assignment was private and never defined.
- Function template parameters must be deducible from the arguments: the scalers
  (`common/scaler`) took the pixel format, 565 or 555, as a template parameter. It's now a
  function parameter or a macro, and the colour masks are expressions on it.
- No declarations in `if` or `while` conditions (Sky, Broken Sword 1), and `error()`
  isn't known not to return, so functions that end in it get a `return`.
- `friend class String;` before `String` was declared made VC4 take it as a global class,
  in some files only, so callers and definitions mangled `Common::String` differently and
  the link failed. `str.h` now declares `String` first. Sky's `text.h` had the same problem
  with `struct displayText_t`.
- `-vmg` (pointers to members of incomplete classes, for `Common::Debugger<T>`), `-GX` (or
  VC4 won't make a temporary `String` from a string literal) and `-GR` (the mixer uses
  `dynamic_cast`) are needed.
- Alpha, MIPS, PowerPC and ARM fault on unaligned reads: `SCUMM_NEED_ALIGNMENT` is set for
  MSVC on anything but x86, and `main` asks NT to fix up any others
  (`SEM_NOALIGNMENTFAULTEXCEPT`).
- With no sound device, ScummVM's mixer never runs, so a sound never ends, and Flight of
  the Amazon Queen's intro waited for ever on its first sound effect. The mixer now drops
  sounds when it has no device. That bug is upstream's too.
