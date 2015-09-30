#if defined(WIN32)

#if !defined(CDRLIB_THREADS_H)
#define CDRLIB_THREADS_H

#if !defined(CDRLIB_DIRTREE_H)
#include "dirtree.h"
#endif

#if !defined(CDRLIB_FILEOBJ_H)
#include "fileobj.h"
#endif

// 8 byte alignment throughout this file.

#pragma pack(push, 8)

typedef unsigned (WINAPI THREADFUNC)(void *param);
typedef unsigned THREADID;

// Cache size (in megabytes).

#define CACHE_SIZE_MIN  1
#define CACHE_SIZE_MAX  32
#define CACHE_SIZE_DEF  2

//
// Thread class
//

class ThreadObject {
  public:
    ThreadObject();
    ~ThreadObject();

    static void SetDebugLogEnabled(BOOL enabled)
      {DebugLogEnabled = enabled;}

    static DWORD WINAPI ThreadMain(void *param);

    BOOL StartThread(DWORD priority); 
    void EndThread(DWORD status);
    void WaitForExit();

  protected:
    static BOOL DebugLogEnabled;
    static FILE *DebugLogFile;

    virtual DWORD ThreadMemberMain() = 0;

  protected:
    HANDLE m_hThread;       // thread handle
    DWORD m_ThreadId;       // thread ID
    HANDLE m_hExitEvent;    // exit event handle
    FILE *m_LogFile;        // debug log file
  };

//
// Base caching class
//

class CacheThread : public ThreadObject {
  public:
    CacheThread();
    ~CacheThread();

    // Get and Set functions.
    static BOOL CacheExists()
      {return ((AllocateBufferPtr != NULL) && (AllocateBufferLength != 0));}

    static BOOL GetEnabled() {return (Enabled);}
    static UBYTE GetSize() {return (SizeMb);}
    static UWORD GetAlignment() {return (Alignment);}
    static DWORD GetPriority() {return (Priority);}
    static UBYTE GetEmptyPercentage() {return (EmptyPercentage);}

    static void SetEnabled(BOOL enabled) {Enabled = enabled;}
    static void SetSize(UBYTE sizemb) {SizeMb = sizemb;}
    static void SetAlignment(UWORD alignment) {Alignment = alignment;}
    static void SetPriority(DWORD priority) {Priority = priority;}
    static void SetEmptyPercentage(UBYTE percentage) {EmptyPercentage = percentage;}
    static void SetUserBuffer(void *bufferP, ULONG length) {UserBufferPtr = bufferP; UserBufferLength = length;}

    // Allocate/deallocate cache buffer.
    static void AllocateBuffer();
    static void DeallocateBuffer();

    // Read data from the cache.
    int Read(void *bufP, ULONG buflen, ULONG *bytes_read = NULL);

  protected:
    // Cache settings.
    static BOOL Enabled;
    static UBYTE SizeMb;
    static UBYTE EmptyPercentage;
    static UWORD Alignment;
    static DWORD Priority;

    // User cache buffer pointer and size (in bytes).
    static void *UserBufferPtr;
    static ULONG UserBufferLength;

    // Allocated cache buffer pointer and size (in bytes).
    static UBYTE *AllocateBufferPtr;
    static ULONG AllocateBufferLength;
    static BOOL AllocatedFlag;

    // Check amount of data in cache.
    int CheckDataLength(ULONG length, ULONG *bytes_available);

    // Check cache free space.
    int CheckFreeSpace(ULONG length);

    // Update the cursors.
    void UpdateReadCursor(ULONG length);
    void UpdateWriteCursor(ULONG length);

    // Get cache information.
    ULONG GetFreeSpaceLength();
    ULONG GetDataLength();

  protected:
    UBYTE *m_pStartBuffer;              // pointer to start of cache buffer
    UBYTE *m_pEndBuffer;                // pointer to end of cache buffer
    UBYTE *m_pReadCursor;               // current reading pointer
    UBYTE *m_pWriteCursor;              // current writing pointer
    HANDLE m_hWaitCacheDataEvent;       // event for waiting on an empty cache
    HANDLE m_hWaitFreeSpaceEvent;       // event for waiting on a full cache
    CRITICAL_SECTION m_CriticalSection; // critical section for maintaining cache pointers
    ULONG m_nSize;                      // size of the cache in bytes
    ULONG m_nFreespaceThreshold;        // wait for freespace threshold
    ULONG m_nWaitFreeSpace;             // amount of free space being waited for
    ULONG m_nWaitCacheData;             // amount of cache data being waited for
    BOOL m_bReadError;                  // read error encountered
    BOOL m_bEndOfData;                  // end-of-data encountered
  };

//
// File caching class
//

class FileCache : public CacheThread {
  public:
    FileCache(const TCHAR *filnamP, UBYTE filetype, UQUAD datalen, ULONG offset);
    ~FileCache();

    // Create and start a new file cache.
    static FileCache *Startup(const TCHAR *filnamP, UBYTE filetype, UQUAD datalen, ULONG offset);

    // File caching main entry point.
    virtual DWORD ThreadMemberMain();

    // Shutdown the cache.
    int Shutdown();

  private:
    void ReadDataIntoCache(ULONG buflen);

  private:
    ImageFile *m_pImageFile;  // pointer to file object
    UQUAD m_nDataLength;      // data segment length
    ULONG m_nDataOffset;      // data segment offset
  };

//
// Disc caching class
//

class DiscCache : public CacheThread {
  public:
    DiscCache(
      CDReader *cdreaderP, UBYTE datatype, UWORD blklen, SLONG start_lba, ULONG blkcnt,
      UBYTE retry_count = 0, UBYTE error_mode = READERROR_MODE_ABORT);
    ~DiscCache();

    // Start a new disc cache.
    static DiscCache *Startup(
      CDReader *cdreaderP, UBYTE datatype, UWORD blklen, SLONG start_lba, ULONG blkcnt,
      UBYTE retry_count = 0, UBYTE error_mode = READERROR_MODE_ABORT);

    // Disc caching main entry point.
    virtual DWORD ThreadMemberMain();

    // Shutdown the disc cache.
    int Shutdown();

  private:
    void ReadAudioIntoCache(UWORD blocks);
    void ReadDataIntoCache(UWORD blocks);

  private:
    CDReader *m_pReader;
    UBYTE *m_pReadBuffer;
    SLONG m_nStartLBA;
    SLONG m_nCurrentLBA;
    ULONG m_nBlockCount;
    UWORD m_nBlockLength;
    UBYTE m_nDatatype;
    UBYTE m_nRetryCount;
    UBYTE m_nErrorMode;
  };

//
// Tree caching class
//

class TreeCache : public CacheThread {
  public:
    TreeCache(DirectoryTree *dirtreeP, UWORD blklen);
    ~TreeCache();

    // Start a new disc cache.
    static TreeCache *Startup(DirectoryTree *dirtreeP, UWORD blklen);

    // Disc caching main entry point.
    virtual DWORD ThreadMemberMain();

    // Shutdown the tree cache.
    int Shutdown();

  private:
    void FileReads(DIRNODE *dirnodeP);
    void ReadOneFile(const TCHAR *pathname, const TCHAR *name);
    void ReadDataIntoCache(ULONG buflen, BOOL padding_flag);

  private:
    DIRNODE *m_pRootDir;    // tree root directory
    UWORD m_nBlockLength;   // padding block length
    HANDLE m_hFile;         // current open file handle
  };

// Restore previous packing alignment.

#pragma pack(pop)

#endif

#endif
