# UnixBench portable harness

A single-executable build of UnixBench's CPU tests for systems that can't
run the regular Perl/`make`/`fork` harness. The first target is
**Windows NT 4.0 on PowerPC**. Classic Mac OS is planned, and the design
already accounts for it (see below).

| Test | Source | Indexed | Baseline |
|---|---|---|---|
| `dhry2reg` | `src/dhry_1.c` (adapted) + `src/dhry_2.c` (unchanged) | yes | 116700 lps |
| `whetstone-double` | `src/whets.c` (unchanged, `#include`d) | yes | 55.0 MWIPS |
| `arithoh`, `short`, `int`, `long`, `float`, `double` | `src/arith.c` (adapted) | no | — |
| `hanoi` | `src/hanoi.c` (adapted) | no | — |

"Adapted" means the benchmark body is unchanged, but the timing that
surrounds it is different (see *How it differs from the Unix build*).

## Building on Windows NT

Copy the whole `UnixBench` directory to the NT machine; the build reads
`..\src`. Then, from a command prompt with the compiler environment set
(`VCVARS32.BAT` for Visual C++ 4.x, or a Developer Command Prompt for
current Visual Studio):

```
cd UnixBench\portable
nmake /f Makefile.nt
ubench
```

The binary targets whatever CPU `cl.exe` targets. On NT 4.0 for PowerPC,
use the PowerPC edition of Visual C++ 4.x. A current Visual Studio
produces x86/x64/ARM64 binaries, which are useful for checking the Win32
code path but are not PowerPC builds.

The code is C89 throughout. It uses no `__int64`, `long long`, `//`
comments or declarations after statements. The only Win32 calls are ones
NT 3.51 already had: `QueryPerformanceCounter`, `CreateThread`, `Sleep`,
`GetVersionEx` and `GetSystemInfo`. The source files use LF line endings.
`cl` and `nmake` accept them, but Notepad shows them as one long line.

## Building for PINT (NT 4.0 on ARM)

[PINT](https://github.com/tenox7/PINT) is Windows NT 4.0 on the Raspberry
Pi. Its SDK cross-compiles from Docker, so no NT machine is needed:

```
./build-pint.sh
```

That makes `ubench-arm-pint.exe`. The header of `build-pint.sh` has the
one-time step that installs Microsoft C for ARM, which accepts the Visual
Studio Build Tools licence.

## CPU-specific builds

`ubench.exe` is the **generic** build. It uses no CPU-specific options, so
it runs on every CPU of its architecture, and its results are the ones to
compare across machines. On Alpha, the makefile can also build one binary
per CPU generation:

| Target | Binary | Runs on | What it adds |
|---|---|---|---|
| `ev4` | `ubench-ev4.exe` | 21064 / 21066 / 21068 and later | tuning for EV4 |
| `ev5` | `ubench-ev5.exe` | 21164 and later | tuning for EV5 |
| `ev56` | `ubench-ev56.exe` | 21164A / 21164PC and later | BWX byte/word instructions |
| `ev6` | `ubench-ev6.exe` | 21264 and later | BWX, FIX, MVI |

```
nmake /f Makefile.nt ev56
nmake /f Makefile.nt alpha
```

The first command builds one variant; `alpha` builds all four. Each
variant has its own object directory (`obj-ev56\` etc.), so variant
builds never mix object files.

**The code-generation flags** (`EV4FLAGS`, `EV5FLAGS`, `EV56FLAGS`,
`EV6FLAGS` at the top of `Makefile.nt`) are `/QA21064`, `/QA21164`,
`/QA21164A` and `/QA21264`. Those options are listed in the option table
of the Alpha `cl.exe` 12.01.8433, the compiler in the Windows 2000 build
2113 tools. Each one sets `_M_ALPHA` to the chip and selects that chip's
architecture. Without an option, the compiler builds for EV4 and tunes for
a blend of EV4, EV5 and EV6, so the generic build runs on a 21064.

Older Alpha compilers may lack these options. In that case, `cl` warns
`D4002: ignoring unknown option` and builds a generic binary under the
variant's name, so watch the build output for that warning. A variant
with empty flags refuses to build.

Only the benchmark kernels get the CPU flags. The runner and platform
layer stay generic, so every binary can start on any Alpha and check the
CPU first:

- A binary started on a CPU older than its target stops with a message,
  instead of crashing on an illegal instruction or having the OS emulate
  the instructions, which would make the timings meaningless. `-f`
  overrides the check.
- The `Build:` line under `System:` shows which variant produced the
  results. Record it, and the flags, with any results.

## Usage

```
ubench [-v] [-q] [-f] [-i passes] [test ...]
```

- With no test names, every test runs.
- `-q` is a smoke test: 1 second and 1 pass per test.
- `-i` sets the number of passes for `dhry2reg` and `whetstone-double`
  (default 10, like `Run -i`). The other tests run 3 passes.
- `-v` prints every pass, plus Whetstone's per-section table.
- `-f` runs a CPU-specific build even on a CPU older than its target.

Scoring follows `Run`'s `combinePassResults()`: drop the lowest third of
passes and take the geometric mean of the rest. The index for each test is
`score / baseline × 10`.

**Only 2 of the 12 System Benchmarks index tests are ported**, so the
summary line reads "Partial Index (2 of 12 index tests)". It is not
comparable to a full UnixBench index score.

## Layout

```
plat.h          platform interface (clock, alarm, sysinfo, argv)
plat_nt.c       Win32 implementation
plat_psx.c      POSIX implementation, for cross-checking scores
ubench.c/.h     runner: test table, passes, scoring, index, report
t_dhry.c        Dhrystone main loop + Proc_1..Proc_5 (from dhry_1.c)
t_whets.c       wraps src/whets.c unchanged
t_arith.c/.h    six arith variants from one template
t_hanoi.c       Tower of Hanoi
Makefile.nt     NMAKE, for Visual C++ 4.x and later
Makefile.posix  for comparing against the stock binaries
```

## How it differs from the Unix build

The Unix build runs each test as its own process. `alarm()` sends
SIGALRM, and the handler prints the count and calls `exit()`. `Run`
measures wall-clock time from outside the process.

The portable build has every test in one executable:

- Each time-limited loop polls a `volatile int ub_stop` instead of being
  interrupted. The platform alarm sets the flag, and the runner times the
  loop itself with the platform's high-resolution clock.
- The extra cost per iteration is one load of a volatile, which is
  negligible next to a Dhrystone pass or 100 arith divides.
- Process start-up time is no longer part of the measurement.
- Globals are re-initialised on each pass, because the tests no longer get
  a fresh process every time.
- On Windows (and classic Mac), `long` is 32 bits. For that reason, arith
  keeps a second counter for wrap-arounds, and counts reach the runner as
  `double`.

Checked on an arm64 Mac against the stock binaries compiled with the same
`-O3 -ffast-math`, 3 passes each:

| Test | Stock | Portable |
|---|---|---|
| Dhrystone | 106.6–109.9M lps | 105.8–107.0M lps |
| Whetstone | 12273–12690 MWIPS | 12518–12558 MWIPS |
| Hanoi | 747.8–750.1k lps | 743.0–751.9k lps |

The arith tests don't compare. A modern optimizer deletes `dumb_stuff()`
in both builds, because its result is never used. What remains is a loop
that only increments a counter: about 2.9G lps in the stock build and
about 1.0G here, where the per-iteration flag and wrap checks dominate
such an empty loop. Older compilers such as Visual C++ 4.x are unlikely
to delete the kernel, but check the arith numbers before you rely on them.

## Classic Mac OS: planned design

Classic Mac OS needs no changes to the tests or the runner, only a new
`plat_mac.c`. The interface was designed around that target's limits:

- **No threads or signals.** `plat_alarm_start()` becomes a Time Manager
  task (`InsTime`/`PrimeTime`). It runs at interrupt time, so it can only
  store to the flag. That is the reason the interface sets a flag instead
  of calling a callback that prints.
- **No command line.** `plat_init()` may replace `argc`/`argv`, e.g. with
  CodeWarrior SIOUX's `ccommand()`.
- **Clock:** `Microseconds()` (68k and PPC), or `UpTime()` on PPC under
  Mac OS 8.6 and later.
- **16-bit `int` on 68k.** Build with 4-byte ints. Dhrystone's own header
  warns that `Arr_2_Glob[8][7]` overflows with 16-bit ints. All harness
  counters are `unsigned long` or `double`.
- **Cooperative multitasking.** A test owns the machine until its alarm
  fires. This is good for measurement accuracy, but the UI will not respond
  during a pass.

## Not ported

- Process, pipe, file system and shell tests, which depend on
  `fork`/`exec`/`pipe`/`/bin/sh`.
- Parallel copies (the "N copies" index). On NT this would mean launching
  `ubench.exe` again for each CPU with `CreateProcess`. Classic Mac has no
  equivalent.
