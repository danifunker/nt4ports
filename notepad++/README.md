# Notepad++ 3.3 for NT 4.0

Licence: GPL v2, see `LICENSE`. Scintilla has its own licence, in `licenses/`.
Notepad++ is by Don Ho; this port is unofficial.

- `patches/npp33-nt4.diff`: every change, against the Notepad++ 3.3 source package
- `compat/`: for targets with no STL or `shlwapi.dll`: a small `string`, `vector`, `list`
  and `algorithm`, local path functions, and a forced header for Visual C++ 4
- `pint/`: ARM build

Apply the patch from inside the source folder:

    patch -p1 --binary < notepad++/patches/npp33-nt4.diff

## Status

- **Alpha:** built with Visual C++ 6.0. Runs. Menu text and redraw artifacts appear on the
  AlphaStation FPGA core; they look like a graphics problem, which isn't confirmed.
- **ARM:** builds with `pint/build.sh`. Runs on PINT in QEMU, not yet tried on a real Pi.
  Use `pint/config.xml`: the standard toolbar shows the wrong icons there.
- **PowerPC:** compiles and links with Visual C++ 4, using the NT 4.0 headers. Doesn't
  fully start yet: the work is in progress.

Printing, plugins and most dialogs have not been tested on any of them.

## Building notes

- Real config files are in `PowerEditor/src`. The `*Model.xml` files in `bin` are empty.
- Visual C++ 6.0's linker takes one `.res`, so compile `Notepad_plus.rc` with `/d __GNUC__`,
  which makes it include the dialogs.
- NT 4.0's `comctl32` can't add icons to a 32-bit image list. `NPP_NT4_HEADERS` selects 16-bit.
