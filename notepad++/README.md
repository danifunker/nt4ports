# Notepad++ 3.3 for NT 4.0

Licence: GPL v2, see `LICENSE`. Scintilla has its own licence, in `licenses/`.
Notepad++ is by Don Ho; this port is unofficial.

- `patches/npp33-nt4.diff`: every change, against the Notepad++ 3.3 source package
- `compat/`: for targets with no STL or `shlwapi.dll`: a small `string`, `vector`, `list`
  and `algorithm`, local path functions, and a forced header for Visual C++ 4
- `pint/`: ARM build
- `ppc/`: PowerPC build, run on the NT machine

Apply the patch from inside the source folder:

    patch -p1 --binary < notepad++/patches/npp33-nt4.diff

## Status

- **Alpha:** built with Visual C++ 6.0. Runs. Menu text and redraw artifacts appear on the
  AlphaStation FPGA core; they look like a graphics problem, which isn't confirmed.
- **ARM:** builds with `pint/build.sh`. Runs on PINT in QEMU, not yet tried on a real Pi.
  Use `pint/config.xml`: the standard toolbar shows the wrong icons there.
- **PowerPC:** builds with Visual C++ 4 with `ppc/build.bat`. Runs on the RS/6000 7248 FPGA
  core. Opening, editing and saving a file, Find, the Style Configurator and exit all worked.
  Some toolbar buttons are blank; the cause isn't known.

Printing, plugins and the other dialogs have not been tested on any of them.

## Building notes

- Real config files are in `PowerEditor/src`. The `*Model.xml` files in `bin` are empty.
- Visual C++ 6.0's linker takes one `.res`, so compile `Notepad_plus.rc` with `/d __GNUC__`,
  which makes it include the dialogs.
- NT 4.0's `comctl32` can't add icons to a 32-bit image list. `NPP_NT4_HEADERS` selects 16-bit.
- Visual C++ 4 has no `bool`, so `compat/npp_vc4.h` makes it an `int`. Code that assumes one
  byte breaks: `LastRecentFileList` looped to `sizeof` of a `bool` array and overwrote the
  members after it. The patch fixes that loop.
- On PowerPC, a fault in a window procedure ended the process with no message at all. To
  find one, wrap the `runProc` call in `__try`/`__except` and log the exception address.

## Building for PowerPC

Needs Visual C++ 4.x for PowerPC on NT 4.0, and NT 4.0 headers. The compiler's own headers are
too old, and `/opt/pint40sdk/include` from the PINT SDK image works. Those aren't included here
and are only needed to build.

1. Patch the source, then copy `compat` and `ppc` into its top folder.
2. Copy the source and headers to the NT machine.
3. Run this from a prompt with the compiler environment set:

       ppc\build D:\src\npp33 D:\src\nt4inc

That makes `no\notepad++.exe` and `so\SciLexer.dll`. Keep the headers last on the include path:
their `propset.h` would otherwise replace Scintilla's `PropSet.h`.

Start with `pint/config.xml`, as the PowerPC build was only tested with it.
