# Building for PINT

ARM builds use the [PINT 4.0 SDK](https://github.com/tenox7/PINT/blob/main/sdk/README.txt)
in Docker. It needs Microsoft C for ARM, which you install once. This accepts the Visual
Studio Build Tools licence, so read that first:

    docker run --rm -e ACCEPT_MSVC_LICENSE=yes -v msvc-arm32:/opt/msvc tenox7/pint40sdk msvc-setup

Then run each app's `pint/build.sh`.

## Testing

A PINT disk boots in QEMU `raspi2b`. Copy files on with `mcopy` while QEMU is stopped, after
a proper NT shutdown: stopping QEMU without one damaged files on the disk.

## NT 4.0 limits that matter here

- The SDK has no STL. See `notepad++/compat`.
- There's no `shlwapi.dll`.
- `comctl32` is NT 4.0's: 32-bit image lists fail, and newer messages are ignored.
- MSVC 14 needs `__std_terminate`, which the older runtime lacks: `npp_rt.cpp`.

## Pipeline

Not set up yet. How these builds should join a PINT pipeline is still open.
