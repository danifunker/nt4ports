# Neko Project II (np2_gdi) for NT 4.0

[np2_gdi](https://github.com/spaztron64/np2_gdi) is spaztron64's backport of Neko Project II
0.81a, Yui's PC-9801 emulator (C version), to GDI and `waveOut` in place of DirectX. It
already builds for NT 4.0 on RISC: this folder only holds build scripts, so it builds on
Linux for every CPU. The source isn't copied here; built from upstream commit `ac36bf2`.

- `ntsdk/build.sh`: Alpha, MIPS and PowerPC with [tenox7/ntsdk](https://github.com/tenox7/ntsdk)
  (Visual C++ 4.1). `ntsdk/csrc.txt` is the file list of upstream's Visual C++ 4 MIPS
  project, `np2cmips.mak`.
- `pint/build.sh`: ARM with the PINT 4.0 SDK, run as `filezilla/nt4/sdkrun.sh` does.
  `pint/include/afxres.h` stands in for MFC's, which the SDK lacks.
- `package.sh`: the release zips, `np2_gdi-0.81a-nt4-CPU.zip`, and
  `np2_gdi-0.81a-nt4-src.zip` (upstream's source without `bin/` and other build debris,
  plus this folder).

## Building

    git clone https://github.com/spaztron64/np2_gdi && git -C np2_gdi checkout ac36bf2
    np2_gdi/ntsdk/build.sh alpha|mips|ppc /path/to/np2_gdi     # obj-CPU/np2.exe
    np2_gdi/pint/build.sh /path/to/np2_gdi                     # obj-arm/np2.exe
    np2_gdi/package.sh /path/to/np2_gdi OUTDIR                 # after all four

No source changes were needed. Notes:

- The RISC SDKs in ntsdk have no `rc`; the x86 SDK's makes the resources. It ignores
  `#pragma code_page(932)`, which doesn't matter: the C version's resources are all ASCII.
- The PINT SDK has no MFC, so no `afxres.h`; the stand-in includes `windows.h` and
  `commctrl.h` and defines `IDC_STATIC`.
- The ARM build imports nothing NT 4.0 lacks.

## Status

| | Alpha | MIPS | PowerPC | ARM (PINT) |
|---|---|---|---|---|
| Builds | Yes | Yes | Yes | Yes |
| Tested on | | | NT 4.0 SP2, QEMU 40P | PINT 4.0, QEMU raspi2b |
| Boots a floppy image, draws, runs | Not run | Not run | Yes (36 FPS) | Yes (11 FPS) |
| Sound, menus and dialogs | Not tested | Not tested | Not tested | Not tested |

np2 makes its font (`font.tmp`) from the Windows fonts on first run when there's no
`FONT.ROM`. Upstream's `bin/` holds commercial disk images and a font ROM: they were used
only to test, and are not distributed here.

## Licence

See `LICENSE`, which goes in each zip. Neko Project II's terms of use (in its 0.82 help
file, and on Yui's site,
[archived](https://web.archive.org/web/20070202133650/http://www.yui.ne.jp:80/np2/copy.html)):
free for personal use, source under the modified BSD licence, no copyright notice needed
for the core and Windows parts, and Yui asks to be told first of commercial binary
redistribution. np2_gdi states no licence for spaztron64's changes; this port treats them
as freely redistributable, as upstream publishes its own builds. None of NONAKA Kimihiro's
BSD-noticed files (`i386c`, `x11`) are in this build. The scripts here are MIT.
