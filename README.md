# nt4ports

Ports of applications to Windows NT 4.0 on non-x86 CPUs: Alpha, MIPS, PowerPC and ARM
([PINT](https://github.com/tenox7/PINT), NT 4.0 on the Raspberry Pi).

These are unofficial ports, not from the original authors. Each folder is under that
project's own licence. Most hold patches and new files, not a copy of the upstream source;
`filezilla` has its source, unchanged in the commit that added it.

| App | Alpha | ARM (PINT) | MIPS | PowerPC |
|---|---|---|---|---|
| [7-Zip 9.20](7zip/) | Built and tested | Built, tested in QEMU | Built | Built and tested |
| [Notepad++ 3.3](notepad++/) | Built and tested | Built, tested in QEMU | Built | Built and tested |
| [UnixBench](unixbench/) | Built | Built, tested in QEMU | Built | Built |
| [FileZilla 2.2.32](filezilla/) | Built | Built, tested in QEMU | Built | Built, tested in QEMU |
| [Neko Project II (np2_gdi)](np2_gdi/) | Built | Built, tested in QEMU | Built | Built, tested in QEMU |
| [ScummVM 0.6.1](scummvm/) | Built | Built, tested in QEMU | Built | Built, tested in QEMU |
| [ScummVM 1.2.1](scummvm-1.2/) | Built, tested on the MiSTer core | — | — | — |
| [ScummVM 2.9.1](scummvm-2.9/) | — | Built, tested in QEMU | — | — |

Every app but ScummVM 1.2.1 and 2.9.1 builds for Alpha, MIPS and PowerPC on Linux with
[tenox7/ntsdk](https://github.com/tenox7/ntsdk), with no NT machine: `7zip/ntsdk`,
`notepad++/ntsdk`, `unixbench/portable/build-ntsdk.sh`, `filezilla/nt4/build.sh`,
`np2_gdi/ntsdk` and `scummvm/ntsdk`. ARM
uses the PINT SDK, as ntsdk has no ARM target; notes for building on PINT are in
[pint/](pint/).

ScummVM comes three times: 0.6.1 for all four CPUs, patched for Visual C++ 4.1; 1.2.1, the
first with Sierra's SCI games, for Alpha only, patched for the Visual C++ 6.0 of the Alpha
CD (too much for 4.1); and 2.9.1 for ARM only, which needs the modern compiler of the PINT
SDK. 1.2.1 was tested on the AlphaStation 200 FPGA core on a MiSTer.
