#!/bin/sh
# Build ubench for Windows NT 4.0 on Alpha, MIPS and PowerPC with tenox7/ntsdk
# (Visual C++ 4.1 for each, https://github.com/tenox7/ntsdk): no NT machine.
#
# Usage: ./build-ntsdk.sh [alpha] [mips] [ppc]      from UnixBench/portable; default all three
# Makes ubench-alpha.exe, ubench-mips.exe and ubench-ppc.exe. ntsdk's ntrun can run them
# (ntrun ubench-mips.exe), slowly: it emulates the CPU.
set -eu

cd "$(dirname "$0")/.."
[ $# -gt 0 ] || set -- alpha mips ppc

for cpu; do
    case $cpu in alpha|mips|ppc) ;; *) echo "build-ntsdk.sh: alpha, mips or ppc" >&2; exit 2 ;; esac
    docker run --rm -v "$PWD":/src -w /src/portable tenox7/ntsdk nt40 $cpu sh -c "
        nmake /nologo /f Makefile.nt clean >/dev/null 2>&1
        nmake /nologo /f Makefile.nt && mv ubench.exe ubench-$cpu.exe
        s=\$?; chown -R $(id -u):$(id -g) . ; exit \$s"
done
ls -l portable/ubench-*.exe
