#!/bin/sh
# Build 7-Zip 9.20 for PINT with the PINT 4.0 SDK.
# Usage: 7zip/pint/build.sh /path/to/7z920       (a copy that has the patch applied)
# Needs the SDK's one-time compiler setup: see pint/README.md.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
src=$(cd "$1" && pwd)

cp "$here/pint_rt.cpp" "$src/"
mkdir -p "$src/CPP/7zip/Archive/Rar5"
cp "$here"/../rar5/* "$src/CPP/7zip/Archive/Rar5/"

docker run --rm --platform linux/amd64 -v msvc-arm32:/opt/msvc -v "$src":/src \
    tenox7/pint40sdk sh -c '
    set -e
    cd /src
    cl -nologo -c -O1 -MT pint_rt.cpp
    for d in Bundles/Format7zF UI/Console UI/FileManager UI/GUI UI/Explorer \
             Bundles/Alone Bundles/SFXCon Bundles/SFXWin Archive/Rar5; do
        (cd CPP/7zip/$d && nmake CPU=ARM MY_STATIC_LINK=1)
    done'
