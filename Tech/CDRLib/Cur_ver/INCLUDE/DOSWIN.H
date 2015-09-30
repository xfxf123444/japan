// 07-sep-04 (sjk) FileFindFirst, FileFindNext no longer take long_filenames_flag.

#if !defined(CDRLIB_DOSWIN_H)
#define CDRLIB_DOSWIN_H

#define DOS_OPEN_FILE   0x3D00
#define DOS_IOCTL_READ  0x4402
#define DOS_CLOSE_FILE  0x3E

#define PATH_IS_BAD     0
#define PATH_IS_FILE    1
#define PATH_IS_DIR     2

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

// System date and time.

struct SYSDATE {
  UWORD day;
  UWORD month;
  UWORD year;
  UWORD dayofweek;
  };

struct SYSTIME {
  UWORD hour;
  UWORD minute;
  UWORD second;
  UWORD milliseconds;
  };

struct SYSDATETIME {
  struct SYSDATE date;
  struct SYSTIME time;
  };

// File information block.

struct FILEINFO {
  UQUAD size;                       // file size (in bytes)
  SYSDATETIME creation_datetime;    // creation date and time
  SYSDATETIME modified_datetime;    // last modification date and time
  SYSDATETIME accessed_datetime;    // last access date and time
  BOOL archive_flag;                // archive file attribute
  BOOL associated_flag;             // associated file attribute
  BOOL dir_flag;                    // directory attribute
  BOOL hidden_flag;                 // hidden file attribute
  BOOL readonly_flag;               // readonly file attribute
  BOOL system_flag;                 // system file attribute
  TCHAR name[_MAX_PATH];            // filename
  TCHAR alternative_name[14];       // alternative filename (8.3 format)
  ULONG reserved;
  };

// Pathname.

struct PATHNAME {
  TCHAR name[_MAX_PATH + 1];
  };

// Path block counts.

struct PATHBLKCNTS {
  ULONG total_file_blkcnt_toplevel;
  ULONG hidden_file_blkcnt_toplevel;
  ULONG system_file_blkcnt_toplevel;
  ULONG archive_file_blkcnt_toplevel;

  ULONG total_file_blkcnt_recurse;
  ULONG hidden_file_blkcnt_recurse;
  ULONG system_file_blkcnt_recurse;
  ULONG archive_file_blkcnt_recurse;
  };

void BeepUser(void);

void Delay(ULONG milliseconds);
void GotoSleep(ULONG seconds);

void GetDate(SYSDATE *sysdateP);
void GetTime(SYSTIME *systimeP);
void GetDateTime(SYSDATETIME *sysdatetimeP);

BOOL IsDateTimeValid(SYSDATETIME *sysdatetimeP);

TCHAR *FileSearchPath(const TCHAR *filnamP);

void *FileFindFirst(const TCHAR *pathnameP, FILEINFO *finfoP);
BOOL FileFindNext(void *handleP, FILEINFO *finfoP);
BOOL FileFindDone(void *handleP);

int PathnameExists(const TCHAR *pathnameP);

BOOL FullPath(const TCHAR *pathname, TCHAR *fullpath, UWORD buflen);

FILEHANDLE FileCreate(const TCHAR *filnamP);
FILEHANDLE FileOpenRead(const TCHAR *filnamP);
FILEHANDLE FileOpenWrite(const TCHAR *filnamP);

BOOL FileRead(FILEHANDLE handle, void *bufP, ULONG buflen);
BOOL FileWrite(FILEHANDLE handle, void *bufP, ULONG buflen);

BOOL FileClose(FILEHANDLE handle);

BOOL FileDelete(const TCHAR *filnamP);

BOOL FileSeekBegin(FILEHANDLE handle, SLONG offset);
BOOL FileSeekBegin(FILEHANDLE handle, SQUAD *offsetP, SQUAD *out_newposP = NULL);

BOOL FileSeekCurrent(FILEHANDLE handle, SLONG offset);
BOOL FileSeekCurrent(FILEHANDLE handle, SQUAD *offsetP, SQUAD *out_newposP = NULL);

BOOL FileSeekEnd(FILEHANDLE handle, SLONG offset);
BOOL FileSeekEnd(FILEHANDLE handle, SQUAD *offsetP, SQUAD *out_newposP = NULL);

ULONG FileGetLength(FILEHANDLE handle, ULONG *sizehighP = NULL);
ULONG FileGetPosition(FILEHANDLE handle);

int CompareDateTime(SYSDATETIME *dt1P, SYSDATETIME *dt2P);

BOOL GetPathBlockCounts(const TCHAR *pathnameP, UWORD dest_blklen, PATHBLKCNTS *blkcntsP);

void GetPathBlockCountsRecurse(
  const TCHAR *pathnameP, const TCHAR *filenameP, UWORD dest_blklen,
  BOOL recurse_flag, PATHBLKCNTS *blkcntsP);

// Class to automatically close files during exceptions.

class AutoFileHandle {
  public:
    AutoFileHandle() {handle = NULL;}
    ~AutoFileHandle()
      {if (handle != NULL) {FileClose(handle); handle = NULL;}}
  public:
    FILEHANDLE handle;
  };

class AutoFileStream {
  public:
    AutoFileStream() {stream = NULL;}
    ~AutoFileStream()
      {if (stream != NULL) {fclose(stream); stream = NULL;}}
  public:
    FILE *stream;
  };

// Restore previous packing alignment.

#pragma pack(pop)

#endif
