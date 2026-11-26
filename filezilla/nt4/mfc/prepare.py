#!/usr/bin/env python3
"""Prepare Visual C++ 6.0's MFC for a compiler that never had it.

  prepare.py VC98 OUT vc4|arm

VC98 is the VC98 folder of a Visual C++ 6.0 CD (any CPU's: the MFC source is the same).
OUT gets:
  include/  MFC's headers
  src/      MFC's source
  sdk/      the CD's Win32 headers, without the C library's and the STL's, so that the
            compiler's own C library headers are used with them

then the edits for the compiler:
  vc4  Visual C++ 4.1 (MIPS, PowerPC): explicit specializations written as Visual C++ 4
       writes them (no template<>), no __declspec(selectany). The rest of Visual C++ 5's
       C++ is fz_vc4.h's macros.
  arm  Microsoft C for ARM with the PINT SDK: ARM is a CPU MFC knows of (afxv_cpu.h), and
       the fixes of PiXPSP1's MFC port for a compiler of today.

Then mfc6-nt4.diff is applied (patch -p1 in OUT). None of OUT goes into the repository:
it is Microsoft's. Written for the NT 4.0 port of FileZilla.
"""
import os
import re
import shutil
import subprocess
import sys

# The C library's headers: the compiler's own are used, not the CD's.
CRT = """assert.h conio.h crtdbg.h ctype.h direct.h dos.h eh.h errno.h excpt.h fcntl.h float.h
fpieee.h fstream.h io.h iomanip.h ios.h iostream.h istream.h limits.h locale.h malloc.h
math.h mbctype.h mbstring.h memory.h minmax.h new.h ostream.h process.h search.h setjmp.h
setjmpex.h share.h signal.h stdarg.h stddef.h stdexcpt.h stdio.h stdiostr.h stdlib.h
streamb.h string.h strstrea.h tchar.h time.h typeinfo.h use_ansi.h varargs.h wchar.h
yvals.h xlocinfo.h xmath.h""".split()


# The CD's headers the ARM build takes in place of the PINT SDK's (see main).
IE4 = ("commctrl.h prsht.h zmouse.h docobj.h servprov.h wininet.h multimon.h commdlg.h "
       "sql.h sqlext.h sqltypes.h sqlucode.h odbcver.h").split()


def copytree_upper(src, dst, keep):
    os.makedirs(dst, exist_ok=True)
    for name in os.listdir(src):
        s = os.path.join(src, name)
        if os.path.isdir(s):
            continue
        if keep(name):
            shutil.copyfile(s, os.path.join(dst, name.lower()))


def edit(path, pairs, count=None):
    """Replace each (old, new) in path; old may be a compiled regex. count: must match."""
    with open(path, "rb") as f:
        s = f.read().decode("latin-1")
    for old, new in pairs:
        if isinstance(old, str):
            n = s.count(old)
            s = s.replace(old, new)
        else:
            s, n = old.subn(new, s)
        if n == 0:
            sys.exit(f"prepare.py: {path}: no {old!r}")
    with open(path, "wb") as f:
        f.write(s.encode("latin-1"))


def main():
    if len(sys.argv) != 4 or sys.argv[3] not in ("vc4", "arm"):
        sys.exit(__doc__)
    vc98, out, flavor = sys.argv[1:]
    mfc = os.path.join(vc98, "MFC")
    if os.path.exists(out):
        shutil.rmtree(out)
    copytree_upper(os.path.join(mfc, "INCLUDE"), os.path.join(out, "include"), lambda n: True)
    copytree_upper(os.path.join(mfc, "SRC"), os.path.join(out, "src"),
                   lambda n: n.upper().endswith((".CPP", ".H", ".INL", ".DEF", ".RC")))
    # Win32 headers: not the C library's, and not the STL's (no extension, or truncated
    # to 8.3 on the CD), which nt4/stl replaces.
    copytree_upper(os.path.join(vc98, "INCLUDE"), os.path.join(out, "sdk"),
                   lambda n: "." in n and n.lower() not in CRT)
    # MFC 6's afxconv.h takes the string conversions from ATL.
    for n in ("ATLCONV.H", "ATLDEF.H"):
        shutil.copyfile(os.path.join(vc98, "ATL", "INCLUDE", n), os.path.join(out, "include", n.lower()))
    inc = lambda n: os.path.join(out, "include", n)
    src = lambda n: os.path.join(out, "src", n)

    # MFC's source is built here without OLE DB (atlbase.h): FileZilla doesn't use it.
    edit(src("stdafx.h"), [("#ifndef _AFX_NO_OLEDB_SUPPORT\r\n\t#include \"atlbase.h\"",
                            "#if 0 /* nt4: no OLE DB */\r\n\t#include \"atlbase.h\"")])

    if flavor == "vc4":
        # Explicit specializations of function templates: plain overloads, as in MFC 4.
        spec = re.compile(r"template\s*<>\s*((?:\w+\s+)+?)AFXAPI\s+(\w+)\s*<[^()]*?>\s*\(")
        for d in ("include", "src"):
            for n in os.listdir(os.path.join(out, d)):
                p = os.path.join(out, d, n)
                with open(p, "rb") as f:
                    s = f.read().decode("latin-1")
                t = spec.sub(r"\1AFXAPI \2(", s)
                if t != s:
                    with open(p, "wb") as f:
                        f.write(t.encode("latin-1"))
        for d in ("include", "src"):
            for n in os.listdir(os.path.join(out, d)):
                p = os.path.join(out, d, n)
                with open(p, "rb") as f:
                    s = f.read()
                if b"__declspec(nothrow)" in s:
                    with open(p, "wb") as f:
                        f.write(s.replace(b"__declspec(nothrow)", b""))
        # The pointer-sized integers of 32-bit Windows as later SDKs have them, int and not
        # long: FileZilla's OnTimer(UINT_PTR) is then MFC 6's OnTimer(UINT), which
        # Visual C++ 4.1 (unlike 6.0) requires of message map entries.
        edit(os.path.join(out, "sdk", "basetsd.h"), [
            ("typedef long INT_PTR, *PINT_PTR;\r\ntypedef unsigned long UINT_PTR, *PUINT_PTR;",
             "typedef int INT_PTR, *PINT_PTR;\r\ntypedef unsigned int UINT_PTR, *PUINT_PTR;")])
        # No __declspec(selectany): data defined once, or static in each file.
        edit(src("stdafx.h"), [("#define AFX_COMDAT __declspec(selectany)", "#define AFX_COMDAT")])
        edit(inc("afxver_.h"), [("#define AFX_STATIC_DATA extern __declspec(selectany)",
                                 "#define AFX_STATIC_DATA static")])

    # The two specializations of CTypedPtrList for lists of lists, which nothing here
    # uses: Visual C++ 4 can't write them, and they name their template's parameters,
    # which only Visual C++ 6.0 accepts.
    edit(inc("afxtempl.h"), [(re.compile(r"template<> class CTypedPtrList<(CObList|CPtrList), \1\*>.*?\n};",
                                         re.S), r"/* nt4: no CTypedPtrList<\1, \1*> */")])

    # typename where afxtempl.h names a type of a template's own (a return type outside
    # its class, the BASE_ types of a map's base class), as compilers of today want it;
    # Visual C++ 4.1 has none, and fz_vc4.h makes it nothing.
    edit(inc("afxtempl.h"), [
        (re.compile(r"^((?:CList|CMap)<[^>\n]*>::C(?:Node|Assoc)\*)", re.M), r"typename \1"),
        ("BASE_CLASS::BASE_", "typename BASE_CLASS::BASE_")])

    # Message map entries take their handler's address with &, as compilers of today
    # require; ten of MFC 6's reflection macros leave it out.
    edit(inc("afxmsg_.h"), [(re.compile(r"\)\)(CtlColor|DrawItem|MeasureItem|DeleteItem|CharToItem|"
                                        r"VKeyToItem|CompareItem|HScroll|VScroll|ParentNotify) \}"), r"))&\1 }")])

    # No OLE control container (_AFX_NO_OCC_SUPPORT, which FileZilla is built with too):
    # occimpl.h, which files include whether or not, needs ATL's OLE DB classes.
    edit(src("occimpl.h"), [(re.compile(r"\A"), "#ifndef _AFX_NO_OCC_SUPPORT /* nt4 */\r\n")])
    with open(src("occimpl.h"), "ab") as f:
        f.write(b"\r\n#endif /* nt4: _AFX_NO_OCC_SUPPORT */\r\n")

    if flavor == "arm":
        # The PINT SDK's headers are NT 4.0's, with ARM in them; MFC 6 also needs those of
        # the common controls of Internet Explorer 4, which are CPU-neutral: ie4/ holds the
        # CD's, to come before PINT's.
        os.makedirs(os.path.join(out, "ie4"))
        for n in IE4:
            shutil.copyfile(os.path.join(out, "sdk", n), os.path.join(out, "ie4", n))
        # A cast a compiler of today wants (as PiXPSP1's port of MFC has it).
        edit(src("tooltip.cpp"), [("m_mapString.LookupKey(ti.lpszText, ti.lpszText)",
                                   "m_mapString.LookupKey(ti.lpszText, (LPCTSTR&)ti.lpszText)")])
        edit(inc("afxv_cpu.h"), [
            ("#if !defined(_M_MRX000) && !defined(_M_ALPHA) && !defined(_M_PPC)",
             "#if !defined(_M_MRX000) && !defined(_M_ALPHA) && !defined(_M_PPC) && !defined(_M_ARM)")])

    diff = os.path.join(os.path.dirname(os.path.abspath(__file__)), "mfc6-nt4.diff")
    if os.path.getsize(diff):
        subprocess.run(["patch", "-s", "-p1", "--binary", "-i", diff], cwd=out, check=True)
    print(f"prepare.py: {out}")


if __name__ == "__main__":
    main()
