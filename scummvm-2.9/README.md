# ScummVM 2.9.1 for NT 4.0 on ARM (PINT)

Licence: GPL v3 or later, see `LICENSE`. SDL is LGPL v2.1 (`../scummvm/licenses`), zlib is
in `../filezilla/zlib`. ScummVM is by the ScummVM team; this port is unofficial.

ScummVM 2.9.1 (2025) for [PINT](https://github.com/tenox7/PINT), Windows NT 4.0 on the
Raspberry Pi, built on Linux with the PINT 4.0 SDK, whose compiler is Microsoft C for ARM
14.44: modern enough for ScummVM's C++. It's ARM only: Visual C++ 4.1, the compiler for
Alpha, MIPS and PowerPC, can't build anything this new (`../scummvm` is 0.6.1 for those).

ScummVM draws and plays sound through SDL 1.2.15, built as for `../scummvm` (GDI video,
`waveOut` sound, no DirectX), and uses zlib for its themes and compressed files. Almost all
of the port is outside ScummVM's source: what NT 4.0's C run-time and headers lack, and the
build.

- `patches/scummvm-2.9.1-nt4.diff`: the one change to ScummVM. CD audio falls back to
  `IOCTL_CDROM_READ_TOC` when NT 4.0 has no `IOCTL_CDROM_READ_TOC_EX`.
- `pint/include`: headers for the SDK, which has NT 4.0's.
  - `pint_svm.h`, included first in every ScummVM file: SAL annotations, the byte-swap
    intrinsics, C99's `snprintf`, `strtoll` and math functions, the float math functions
    (NT 4.0 has them only on MIPS) and C++11's `<cmath>` overloads.
  - `stdint.h`, `inttypes.h`, `<new>`, `<limits>`, `<initializer_list>`, `<cstddef>` and
    the other `<cxxx>` headers, `intrin.h`, `sdkddkver.h`.
  - `windows.h`: the SDK's, then the Windows 2000 types and constants ScummVM's Win32
    code names (`ULONG_PTR`, `OSVERSIONINFOEXA`, `SetClassLongPtr`, `CSIDL_*`). That code
    already looks up the Windows 2000 functions at run time.
  - `ntddcdrm.h`: the copy of MinGW's that ScummVM carries for older Visual C++.
- `pint/src/pint_crt.cpp`: the functions `pint_svm.h` declares.
- `pint/build.sh`: the build. `package.sh`: the release zips.

## Building

    curl -L https://github.com/scummvm/scummvm/archive/refs/tags/v2.9.1.tar.gz | tar -xz
    curl -L https://github.com/libsdl-org/SDL-1.2/archive/refs/tags/release-1.2.15.tar.gz | tar -xz
    mv SDL-1.2-release-1.2.15 SDL-1.2.15
    cd scummvm-2.9.1 && patch -p1 --binary < ../scummvm-2.9/patches/scummvm-2.9.1-nt4.diff && cd ..
    scummvm-2.9/pint/build.sh scummvm-2.9.1 SDL-1.2.15        # obj-arm/scummvm.exe
    scummvm-2.9/package.sh scummvm-2.9.1 SDL-1.2.15 OUTDIR

`build.sh` needs g++ on the host: it builds ScummVM's `devtools/create_project`, which
writes a Visual Studio project, and takes the source list, the defines and the engine
tables from that. `ENGINES` picks the engines; the default is every one that needs no
library but zlib, except Blade Runner, Ultima, mTropolis and Director. The SDK runs as
`../filezilla/nt4/sdkrun.sh` does. On 8 cores the default build takes about 12 minutes.

## Notes

- Keep an empty `scummvm.ini` beside `scummvm.exe` (the zip has one). That's ScummVM's
  portable mode: settings, saves and `scummvm.log` stay in that folder. Otherwise it wants
  the user's Application Data folder, which NT 4.0 without Internet Explorer 5 can't give
  it, and the settings go in the Windows folder.
- It's a Windows program with ScummVM's own `WinMain`, as in ScummVM's Windows releases.
- No exceptions (`-EHs-c-`): ScummVM throws none, and the old runtime lacks the helpers.
  The thread-safe statics, sized delete and security cookie of MSVC 14 are off for the
  same reason.
- `pint_crt.cpp` is built without `pint_svm.h` and unoptimised. With the header, its
  `tanf` would call the header's C++ `tan(float)`, which calls `tanf`; optimised, MSVC
  turns `(float)tan((double)x)` back into `tanf(x)`. Either way `tanf` called itself for
  ever, and the launcher hung on its first SVG icon.
- NT 4.0's C run-time has `_ftelli64` and `_fseeki64` (its `fgetpos` uses them), though
  its `stdio.h` doesn't declare them.
- The object list is longer than a Windows command line, so the link takes a response
  file.
- `pint/include/windows.h` includes the SDK's by its path, `Z:\opt\pint40sdk\include`, which
  is where the SDK is in its Docker image and in `sdkrun.sh`'s bubblewrap copy. The SDK
  installed on Windows would need that path changed.

## Status

PINT 4.0 in QEMU (raspi2b): the launcher draws with the current theme; Day of the Tentacle
(SCUMM), Flight of the Amazon Queen, Gold Rush! (AGI), Simon the Sorcerer (AGOS) and
Gobliiins (Gob) start and play. Not tried on a real Pi, and saving and loading weren't
tried (PINT's mouse in QEMU is too erratic for the menus).
Conquests of the Longbow (SCI) waited for ever on the Sierra logo with sound on: QEMU's Pi
has no sound device, so `waveOut` takes the sound and never plays it, and SCI games time
their scenes by their music. With `SDL_AUDIODRIVER=dummy` (the zip's
`scummvm-nosound.bat`) it reaches its menu. Nobody has listened to the sound yet. Ctrl+Q
doesn't quit a game; the window's close button and Alt+F4 do.

Not built in: MP3, Ogg Vorbis, FLAC, MPEG-2, Theora, JPEG and PNG, FreeType, OpenGL and 3D,
the MT-32 emulator, translations, text to speech and networking, and the engines that need
them (AGS, Wintermute, The Longest Journey, Grim Fandango and others).
