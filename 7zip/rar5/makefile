PROG = Rar5.dll
DEF_FILE = ../Archive.def
CFLAGS = $(CFLAGS) -I ../../../

AR_OBJS = \
  $O\ArchiveExports.obj \
  $O\DllExports.obj \

RAR5_OBJS = \
  $O\Rar5Handler.obj \

RAR5_RAW_OBJS = \
  $O\rar5.obj \
  $O\rar5shim.obj \
  $O\archive_blake2s.obj \
  $O\archive_blake2sp.obj \

COMMON_OBJS = \
  $O\IntToString.obj \
  $O\MyString.obj \
  $O\MyVector.obj \
  $O\NewHandler.obj \
  $O\StringConvert.obj \
  $O\UTFConvert.obj \

WIN_OBJS = \
  $O\PropVariant.obj \

7ZIP_COMMON_OBJS = \
  $O\StreamUtils.obj \

AR_COMMON_OBJS = \
  $O\ItemNameUtils.obj \

OBJS = \
  $O\StdAfx.obj \
  $(AR_OBJS) \
  $(RAR5_OBJS) \
  $(RAR5_RAW_OBJS) \
  $(COMMON_OBJS) \
  $(WIN_OBJS) \
  $(7ZIP_COMMON_OBJS) \
  $(AR_COMMON_OBJS) \

!include "../../../Build.mak"

$(AR_OBJS): ../$(*B).cpp
	$(COMPL)
$(RAR5_OBJS): $(*B).cpp
	$(COMPL)
$(RAR5_RAW_OBJS): $(*B).cpp
	$(COMPL_O2)
$(COMMON_OBJS): ../../../Common/$(*B).cpp
	$(COMPL)
$(WIN_OBJS): ../../../Windows/$(*B).cpp
	$(COMPL)
$(7ZIP_COMMON_OBJS): ../../Common/$(*B).cpp
	$(COMPL)
$(AR_COMMON_OBJS): ../Common/$(*B).cpp
	$(COMPL)
