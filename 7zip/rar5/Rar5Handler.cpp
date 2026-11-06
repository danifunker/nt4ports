// Rar5Handler.cpp: RAR5 extraction for 7-Zip 9.20, as a Formats\Rar5.dll plugin.
// The decoder is libarchive's reader (BSD), see rar5.cpp and LICENSE-RAR5.TXT.

#include "StdAfx.h"

#include "Common/ComTry.h"
#include "Common/MyCom.h"
#include "Common/MyString.h"
#include "Common/MyVector.h"
#include "Common/StringConvert.h"
#include "Common/UTFConvert.h"

#include "Windows/PropVariant.h"

#include "../../Common/RegisterArc.h"
#include "../../Common/StreamUtils.h"

#include "../Common/ItemNameUtils.h"

#include "rar5api.h"

using namespace NWindows;

namespace NArchive {
namespace NRar5 {

STATPROPSTG kProps[] =
{
  { NULL, kpidPath, VT_BSTR},
  { NULL, kpidIsDir, VT_BOOL},
  { NULL, kpidSize, VT_UI8},
  { NULL, kpidPackSize, VT_UI8},
  { NULL, kpidMTime, VT_FILETIME},
  { NULL, kpidCTime, VT_FILETIME},
  { NULL, kpidATime, VT_FILETIME},
  { NULL, kpidEncrypted, VT_BOOL},
  { NULL, kpidCRC, VT_UI4}
};

STATPROPSTG kArcProps[] =
{
  { NULL, kpidSolid, VT_BOOL}
};

struct CItem
{
  UString Name;
  UInt64 Size;
  UInt64 PackSize;
  FILETIME MTime, CTime, ATime;
  bool HasSize, HasMTime, HasCTime, HasATime;
  bool IsDir, IsLink, Encrypted, HasCRC;
  UInt32 CRC;
};

static void UnixToFileTime(time_t t, FILETIME &ft)
{
  UInt64 v = ((UInt64)(Int64)t + (UInt64)11644473600) * (UInt64)10000000;
  ft.dwLowDateTime = (DWORD)v;
  ft.dwHighDateTime = (DWORD)(v >> 32);
}

struct CReadContext
{
  IInStream *Stream;
  HRESULT Result;
};

static long ReadFromStream(void *ctx, void *buf, unsigned long size)
{
  CReadContext *c = (CReadContext *)ctx;
  UInt32 processed = 0;
  HRESULT res = c->Stream->Read(buf, (UInt32)size, &processed);
  if (res != S_OK)
  {
    c->Result = res;
    return -1;
  }
  return (long)processed;
}

class CReader
{
public:
  CReadContext Context;
  r5handle *Handle;
  CReader(): Handle(0) {}
  ~CReader() { Close(); }
  void Close() { if (Handle) { r5_close(Handle); Handle = 0; } }
  HRESULT Open(IInStream *stream)
  {
    Close();
    RINOK(stream->Seek(0, STREAM_SEEK_SET, NULL));
    Context.Stream = stream;
    Context.Result = S_OK;
    Handle = r5_open(ReadFromStream, &Context);
    return Handle ? S_OK : E_OUTOFMEMORY;
  }
};

class CHandler:
  public IInArchive,
  public CMyUnknownImp
{
  CMyComPtr<IInStream> _stream;
  CObjectVector<CItem> _items;
  bool _solid;
  bool _volume;
public:
  MY_UNKNOWN_IMP1(IInArchive)
  INTERFACE_IInArchive(;)
};

IMP_IInArchive_Props
IMP_IInArchive_ArcProps

STDMETHODIMP CHandler::GetArchiveProperty(PROPID propID, PROPVARIANT *value)
{
  NCOM::CPropVariant prop;
  switch(propID)
  {
    case kpidSolid: prop = _solid; break;
  }
  prop.Detach(value);
  return S_OK;
}

STDMETHODIMP CHandler::GetNumberOfItems(UInt32 *numItems)
{
  *numItems = _items.Size();
  return S_OK;
}

STDMETHODIMP CHandler::GetProperty(UInt32 index, PROPID propID, PROPVARIANT *value)
{
  COM_TRY_BEGIN
  NCOM::CPropVariant prop;
  const CItem &item = _items[index];
  switch(propID)
  {
    case kpidPath: prop = NItemName::GetOSName2(item.Name); break;
    case kpidIsDir: prop = item.IsDir; break;
    case kpidSize: if (item.HasSize) prop = item.Size; break;
    case kpidPackSize: prop = item.PackSize; break;
    case kpidMTime: if (item.HasMTime) prop = item.MTime; break;
    case kpidCTime: if (item.HasCTime) prop = item.CTime; break;
    case kpidATime: if (item.HasATime) prop = item.ATime; break;
    case kpidEncrypted: prop = item.Encrypted; break;
    case kpidCRC: if (item.HasCRC) prop = item.CRC; break;
  }
  prop.Detach(value);
  return S_OK;
  COM_TRY_END
}

STDMETHODIMP CHandler::Open(IInStream *stream,
    const UInt64 * /* maxCheckStartPosition */,
    IArchiveOpenCallback *callback)
{
  COM_TRY_BEGIN
  Close();
  CReader reader;
  RINOK(reader.Open(stream));
  if (!r5_bid(reader.Handle))
    return S_FALSE;

  // Header walk only: nothing is decompressed here, even in solid archives.
  for (;;)
  {
    r5item it;
    int res = r5_next(reader.Handle, &it);
    if (res != 0)
    {
      if (res < 0 && reader.Context.Result != S_OK)
        return reader.Context.Result;
      if (res < 0 && _items.Size() == 0)
        return S_FALSE;
      break;
    }
    CItem item;
    AString utf(it.path);
    if (!ConvertUTF8ToUnicode(utf, item.Name))
      item.Name = MultiByteToUnicodeString(utf);
    item.Size = (UInt64)it.size;
    item.HasSize = (it.has_size != 0);
    item.PackSize = (UInt64)it.packed;
    item.HasMTime = (it.has_mtime != 0);
    item.HasCTime = (it.has_ctime != 0);
    item.HasATime = (it.has_atime != 0);
    if (item.HasMTime) UnixToFileTime(it.mtime, item.MTime);
    if (item.HasCTime) UnixToFileTime(it.ctime_, item.CTime);
    if (item.HasATime) UnixToFileTime(it.atime, item.ATime);
    item.IsDir = (it.is_dir != 0);
    item.IsLink = (it.is_link != 0);
    item.Encrypted = (it.encrypted != 0);
    item.CRC = it.crc;
    item.HasCRC = (it.has_crc != 0);
    _items.Add(item);
    if (r5_skip_packed(reader.Handle) != 0)
      break;
    if (callback && (_items.Size() & 0xFF) == 0)
    {
      UInt64 numFiles = _items.Size();
      RINOK(callback->SetCompleted(&numFiles, NULL));
    }
  }
  _solid = (r5_is_solid(reader.Handle) != 0);
  _volume = (r5_is_volume(reader.Handle) != 0);
  _stream = stream;
  return S_OK;
  COM_TRY_END
}

STDMETHODIMP CHandler::Close()
{
  _items.Clear();
  _stream.Release();
  _solid = false;
  _volume = false;
  return S_OK;
}

static HRESULT ReportResult(IArchiveExtractCallback *callback, UInt32 index,
    Int32 askMode, Int32 opRes)
{
  CMyComPtr<ISequentialOutStream> out;
  RINOK(callback->GetStream(index, &out, askMode));
  RINOK(callback->PrepareOperation(askMode));
  out.Release();
  return callback->SetOperationResult(opRes);
}

STDMETHODIMP CHandler::Extract(const UInt32 *indices, UInt32 numItems,
    Int32 testMode, IArchiveExtractCallback *extractCallback)
{
  COM_TRY_BEGIN
  bool allFilesMode = (numItems == (UInt32)-1);
  if (allFilesMode)
    numItems = _items.Size();
  if (numItems == 0)
    return S_OK;

  UInt64 totalSize = 0;
  UInt32 i;
  for (i = 0; i < numItems; i++)
    totalSize += _items[allFilesMode ? i : indices[i]].Size;
  extractCallback->SetTotal(totalSize);

  Int32 askMode = testMode ?
      NExtract::NAskMode::kTest :
      NExtract::NAskMode::kExtract;

  // One file of a multi-volume set can continue in the next volume, which
  // this plugin doesn't read. Report that rather than produce a short file.
  if (_volume)
  {
    for (i = 0; i < numItems; i++)
    {
      RINOK(ReportResult(extractCallback, allFilesMode ? i : indices[i], askMode,
          NExtract::NOperationResult::kUnSupportedMethod));
    }
    return S_OK;
  }

  CReader reader;
  RINOK(reader.Open(_stream));

  UInt64 completed = 0;
  UInt32 pos = 0;
  UInt32 index = 0;
  bool broken = false;
  for (index = 0; pos < numItems && index < (UInt32)_items.Size(); index++)
  {
    RINOK(extractCallback->SetCompleted(&completed));
    r5item it;
    if (!broken && r5_next(reader.Handle, &it) != 0)
      broken = true;
    if (broken)
    {
      if (reader.Context.Result != S_OK)
        return reader.Context.Result;
      UInt32 wanted = allFilesMode ? pos : indices[pos];
      if (wanted == index)
      {
        RINOK(ReportResult(extractCallback, index, askMode,
            NExtract::NOperationResult::kDataError));
        pos++;
      }
      continue;
    }

    UInt32 wanted = allFilesMode ? pos : indices[pos];
    if (wanted != index)
    {
      // Solid: earlier files must be decoded to rebuild the dictionary.
      int res = _solid ? r5_skip_decode(reader.Handle) : r5_skip_packed(reader.Handle);
      if (res != 0)
        broken = true;
      continue;
    }
    pos++;

    const CItem &item = _items[index];
    CMyComPtr<ISequentialOutStream> outStream;
    RINOK(extractCallback->GetStream(index, &outStream, askMode));
    if (!testMode && !outStream && !item.IsDir)
    {
      if (r5_skip_decode(reader.Handle) != 0)
        broken = true;
      continue;
    }
    RINOK(extractCallback->PrepareOperation(askMode));

    Int32 opRes = NExtract::NOperationResult::kOK;
    if (item.IsDir)
    {
      if (r5_skip_decode(reader.Handle) != 0)
        broken = true;
    }
    else if (item.Encrypted || item.IsLink)
    {
      opRes = NExtract::NOperationResult::kUnSupportedMethod;
      if (r5_skip_decode(reader.Handle) != 0)
        broken = true;
    }
    else
    {
      for (;;)
      {
        const void *data = 0;
        size_t size = 0;
        int res = r5_read(reader.Handle, &data, &size);
        if (res < 0)
        {
          if (reader.Context.Result != S_OK)
            return reader.Context.Result;
          const char *err = r5_error(reader.Handle);
          if (strncmp(err, "Not enough memory", 17) == 0)
            return E_OUTOFMEMORY;
          opRes = (strncmp(err, "Checksum error", 14) == 0) ?
              NExtract::NOperationResult::kCRCError :
              NExtract::NOperationResult::kDataError;
          broken = true;
          break;
        }
        if (res > 0)
          break;
        if (size != 0)
        {
          if (outStream)
          {
            RINOK(WriteStream(outStream, data, size));
          }
          completed += size;
          RINOK(extractCallback->SetCompleted(&completed));
        }
      }
    }
    outStream.Release();
    RINOK(extractCallback->SetOperationResult(opRes));
  }
  return S_OK;
  COM_TRY_END
}

static IInArchive *CreateArc() { return new CHandler; }

static CArcInfo g_ArcInfo =
  { L"Rar5", L"rar", 0, 0xCC, { 'R', 'a', 'r', '!', 0x1A, 0x07, 0x01, 0x00 }, 8, false, CreateArc, 0 };

REGISTER_ARC(Rar5)

}}
