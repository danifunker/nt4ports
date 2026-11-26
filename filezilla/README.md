# FileZilla 2.2.32 for NT 4.0

Licence: GPL v2 or later, see `LICENSE`. zlib has its own, in `zlib/LICENSE`.
FileZilla is by Tim Kosse; this port is unofficial.

Unlike the other folders here, this one has FileZilla's source itself:

- `src/`: FileZilla 2.2.32 (`FileZilla_2_2_32_src.zip`) with the port's changes. The
  commit that added it has it unchanged, so `git diff` against that commit shows every
  change. Each is marked `nt4`, or under `#ifdef FZ_NT4`.
- `zlib/`: the zlib 1.3.1 files FileZilla links.
- `nt4/`: the build and what the NT 4.0 compilers lack:
  - `build.sh`, `Makefile`, `sdkrun.sh`, `get-vc6.sh`: the build, on Linux.
  - `include/`: headers in place of the Platform SDK's (`fz_nt4.h`, IPv6 types, themes,
    IDN) and `fz_vc4.h`, the forced include for Visual C++ 4.1.
  - `stl/`: a small STL (vector, deque, list, map, set) for the compilers without one,
    with a test, `stltest.cpp`.
  - `mfc/`: builds Visual C++ 6.0's MFC for MIPS, PowerPC and ARM, which it never shipped
    for, from the MFC source on the CD.
  - `imports.c`: comctl32 functions of Internet Explorer 3 and 4, falling back to what
    NT 4.0 has. `crashlog.c`: writes `FileZilla-crash.txt` on a fault.
    `debughooks.c`: a debugging aid, not in releases.

## Status

| CPU | Compiler | MFC | State |
|---|---|---|---|
| Alpha | Visual C++ 6.0 for Alpha | its own, 6.0 | Built, not run yet |
| MIPS | Visual C++ 4.1 | 6.0, built by `nt4/mfc` | Built, not run yet |
| PowerPC | Visual C++ 4.1 | 6.0, built by `nt4/mfc` | Built, tested in QEMU |
| ARM (PINT) | Microsoft C 19 for ARM, PINT SDK | 6.0, built by `nt4/mfc` | Built, tested in QEMU |

PowerPC was tested on NT 4.0 SP2 in QEMU's IBM 40P (with its own comctl32 of 1996), and
ARM on PINT 4.0 in QEMU raspi2b: each starts, browses local folders, connects to an FTP
server, lists it, and downloads and uploads files, which arrived unchanged (a 1 MB
download, uploads of 214 and 365 KB; on ARM over an existing file, through the "File
already exists" box). SFTP, the Site Manager and the settings dialogs have not been
tried. Alpha and MIPS have not been run: their imports are all NT 4.0's.

## Limits

- No FTP over SSL/TLS: FileZilla loads `ssleay32.dll` and `libeay32.dll` when it needs
  them, and there are none for these CPUs.
- No Kerberos (GSS), no IPv6 (NT 4.0's Winsock has none), and only ASCII host names
  (no IDN): `nt4compat.cpp` stands in for GNU libidn.
- English only: the language DLLs are not built. The help (`FileZilla.chm`) is not built.
- Upstream's crash reporter (`misc/ExceptionReport.cpp`) is left out; `crashlog.c`
  writes `FileZilla-crash.txt` instead.
- With NT 4.0's own comctl32 (no Internet Explorer 4):
  - the toolbar and the Quickconnect bar sit one under the other, as there is no rebar;
  - the transfer queue has no progress bars. A list that is both virtual and owner-drawn
    brought that comctl32 down, and silently on PowerPC; the queue is now drawn by
    comctl32 itself there (`QueueView.cpp`).
  - The speed limit rules dialog has a date picker, which needs comctl32 4.70.

## Building

On Linux, with Docker. Every CPU needs a Visual C++ 6.0 CD, which nothing here
carries; Alpha needs the Alpha CD (`msvc60pro_en-us_alpha.iso`), the other CPUs take only
its MFC source, headers and resource compiler, so any CPU's CD works for them.

    filezilla/nt4/get-vc6.sh msvc60pro_en-us_alpha.iso ~/vc6

Alpha (`tenox7/ntsdk` runs the CD's compiler with `ntrun`):

    VC6=~/vc6 filezilla/nt4/build.sh alpha

MIPS and PowerPC (`tenox7/ntsdk`'s Visual C++ 4.1; first build MFC 6 for the CPU):

    filezilla/nt4/mfc/build-mfc.sh ppc ~/vc6/VC98 ~/mfc-ppc
    VC6=~/vc6 MFC=~/mfc-ppc filezilla/nt4/build.sh ppc

ARM: the same with `arm`, in the PINT 4.0 SDK (see `../pint/README.md`). Without the
`tenox7/pint40sdk` image in Docker, `PINT_SDK` and `PINT_MSVC` point at an unpacked copy
of it and at Microsoft C for ARM, as for ruststdwinnt4's `tools/pint-sdk`.

Each build puts `FileZilla.exe`, `FzSFtp.exe` and `FileZilla.map` in `filezilla/out/CPU`.
`build.sh` passes `MAKEARGS` to make: `MAKEARGS=DEBUGHOOKS=1` links `nt4/debughooks.c`.

## Building notes

- Visual C++ 4.1 compiles MFC 6 once `nt4/mfc/prepare.py` has rewritten a few things:
  explicit specializations as Visual C++ 4 writes them, no `__declspec(selectany)` or
  `(nothrow)`, `int` pointer-sized types (MFC's message maps then match FileZilla's
  handlers). `fz_vc4.h` makes `bool` an `int` and drops `explicit`, `mutable` and
  `typename`, as the Notepad++ and 7-Zip ports here do.
- MFC is built without the OLE control container, DHTML, OLE DB and DAO
  (`_AFX_NO_OCC_SUPPORT` and the rest), and so is FileZilla, for the same class layouts.
- Visual C++ 4.1 for PowerPC crashes (`mdmisc.c`, line 654) on many of FileZilla's files
  with a precompiled header, and not without one: MIPS and PowerPC build without.
- It also compiles every member of a class template it instantiates, has no local
  classes with member functions, and wrongly hides a class's protected nested types from
  templates. The STL is written for that, and some of FileZilla's classes moved
  (`nt4` comments in `FtpControlSocket.cpp`, `SftpControlSocket.cpp`, `MarkupSTL.h`,
  `QueueCtrl.h`).
- The STL's `list` keeps its end node on the heap, as Microsoft's does: a list in an
  object copied byte for byte stays whole. FileZilla crashed on connecting before that.
- On PowerPC, a fault in comctl32 drawing a window ended the process with no message.
  `nt4/debughooks.c` found it: a trace of every message and the stack of whatever ends
  the process.
- The PINT SDK's `cl`, `link` and `rc` set `INCLUDE` and `LIB` themselves, so MFC's
  folders go in through `CL` and `LINK`.
