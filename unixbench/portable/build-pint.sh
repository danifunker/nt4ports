#!/bin/sh
# Build ubench-arm-pint.exe for PINT (Windows NT 4.0 on ARM) with the PINT SDK:
# https://github.com/tenox7/PINT, sdk/README.txt.
#
# Needs Docker. Once, install Microsoft C for ARM into a volume. This accepts the
# Visual Studio Build Tools licence, so read it first:
#
#   docker run --rm -e ACCEPT_MSVC_LICENSE=yes -v msvc-arm32:/opt/msvc \
#       tenox7/pint40sdk msvc-setup
#
# Usage: ./build-pint.sh      from UnixBench/portable
set -eu

cd "$(dirname "$0")/.."

docker run --rm --platform linux/amd64 -v msvc-arm32:/opt/msvc -v "$PWD":/src \
    tenox7/pint40sdk sh -c '
        cd /src/portable &&
        nmake /f Makefile.nt clean &&
        nmake /f Makefile.nt ARITHFLAGS=/Od &&
        mv ubench.exe ubench-arm-pint.exe'

ls -l portable/ubench-arm-pint.exe
