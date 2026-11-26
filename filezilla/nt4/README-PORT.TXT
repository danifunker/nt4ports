FileZilla 2.2.32 for Windows NT 4.0 on Alpha, MIPS, PowerPC and ARM (PINT)
==========================================================================

An unofficial port of FileZilla 2.2.32 by Tim Kosse, from github.com/danifunker/nt4ports
(folder filezilla). FileZilla is free software under the GNU General Public License,
version 2 or later: see LICENSE.TXT. The source, with every change of the port, is in
that repository and in this release's filezilla-2.2.32-nt4-src.zip.

Files
-----
FileZilla.exe   the program
FzSFtp.exe      its SFTP helper: keep it beside FileZilla.exe
FileZilla.map   the linker's map, to read FileZilla-crash.txt with
LICENSE.TXT     the GPL
ZLIB.TXT        zlib's licence

FileZilla keeps its settings in FileZilla.xml beside FileZilla.exe, or in the registry.
If it crashes, FileZilla-crash.txt beside it says where: please send it with a report.

Which build
-----------
Alpha    Visual C++ 6.0 for Alpha, MFC 6.0              built, not yet run
MIPS     Visual C++ 4.1, MFC 6.0 built for MIPS          built, not yet run
PowerPC  Visual C++ 4.1, MFC 6.0 built for PowerPC       tested on NT 4.0 SP2 (QEMU 40P)
ARM      Microsoft C 19, PINT SDK, MFC 6.0 built for ARM tested on PINT 4.0 (QEMU raspi2b)

On PowerPC and ARM it connects, lists, downloads and uploads (checked byte for byte).
SFTP, the Site Manager and most settings dialogs have not been tried on any CPU.

Limits
------
- No FTP over SSL/TLS, no Kerberos, no IPv6, host names in ASCII only.
- English only, and no help file.
- Without Internet Explorer 4's comctl32 (NT 4.0 as it came): the toolbar and the
  Quickconnect bar are one under the other, the transfer queue shows no progress bars,
  and the speed limit rules dialog (which has a date picker) needs comctl32 4.70.
