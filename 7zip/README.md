# 7-Zip 9.20 for NT 4.0

Licence: GNU LGPL, with the unRAR restriction on RAR code. See `LICENSE` and `licenses/`.
7-Zip is by Igor Pavlov; this port is unofficial.

- `patches/7zip-9.20-nt4.diff`: every change, against the 7-Zip 9.20 and LZMA SDK 9.20 sources
- `rar5/`: `Formats\Rar5.dll`, RAR5 extraction for 9.20. See below.
- `pint/`: ARM build
- `ppc/`: PowerPC build, run on the NT machine

Apply the patch from a folder holding `7z920/` and `lzma920/`:

    patch -p1 --binary < 7zip/patches/7zip-9.20-nt4.diff

## Building

Alpha, with Visual C++ 6.0, in each component folder (`UI/FileManager`, `UI/GUI`,
`UI/Console`, `UI/Explorer`, `Bundles/Format7zF`, `Bundles/SFXWin`, `Bundles/SFXCon`):

    nmake CPU=ALPHA MY_STATIC_LINK=1

ARM: `7zip/pint/build.sh /path/to/7z920`.

PowerPC, with Visual C++ 4.x on the NT machine: copy `ppc/` into the patched `7z920` folder
and `rar5/` to `CPP/7zip/Archive/Rar5`, then run this from a prompt with the compiler
environment set:

    ppc\build D:\src\7z920 D:\src\nt4inc

The second folder holds NT 4.0 headers, as for Notepad++: see `../notepad++/README.md`.

## Rar5.dll

9.20 predates RAR5. 7-Zip loads any DLL from a `Formats` folder beside `7z.dll`, so this
adds RAR5 without changing `7z.dll`. To build it, copy `rar5/` to
`CPP/7zip/Archive/Rar5` and run nmake there.

The decoder is libarchive's RAR5 reader behind a small shim, and doesn't use unRAR code:
licence in `rar5/LICENSE`. `gen.py` regenerates `rar5.cpp` from libarchive 3.7.7.

Encrypted files, multi-volume sets and links report Unsupported Method. It only extracts.

## Known limits

- ARM and PowerPC: the file manager's toolbar has icons but no labels, and Shift and Ctrl
  clicks in its file list aren't detected.
- `7-zip.dll` (the Explorer menu) hasn't been tested. On PowerPC it loads, but it hasn't been
  registered or used.
- `7z.sfx` has been tested on PowerPC only.
- RAR4 archives have not been tested on PowerPC.

## Visual C++ 4 notes

`ppc/p7_vc4.h` is forced into every file. These limits shaped the patch:

- No `bool`. It's a `long` here, because `CPropVariant` has `bool`, `Int32` and `UInt32`
  overloads.
- No member templates, and every member of a class template is compiled even if unused. So
  vector searches and Huffman decoding are free functions.
- `string + number` picks `operator+(string, char)`, not pointer arithmetic, with no warning.
- An aggregate entry that uses a named constant or a non-constant pointer can shift every entry
  after it, with no warning. `ArchiveCommandLine.cpp`'s switch table uses literals for this.
- `throw` as a function's last statement doesn't count as returning a value.
