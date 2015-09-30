// 06-sep-04 (sjk) Changes.

#if !defined(CDRLIB_DIRTREE_H)
#define CDRLIB_DIRTREE_H

#if !defined(CDRLIB_QUEUE_H)
#include "queue.h"
#endif

#if !defined(CDRLIB_DOSWIN_H)
#include "doswin.h"
#endif

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

// Duplicate directory options.

#define DIRECTORY_MERGE       0
#define DIRECTORY_REPLACE     1

// Duplicate file options.

#define FILE_REPLACE_CHANGED  0
#define FILE_REPLACE_ALWAYS   1
#define FILE_REPLACE_NEVER    2

//
// File Backup List
//

struct FBLHEADER {
  char id[16];
  UWORD entrycount;
  WFLAG fullpath_flag : 1;
  WFLAG long_names_flag : 1;
  WFLAG recurse_flag : 1;
  WFLAG hidden_flag : 1;
  WFLAG system_flag : 1;
  WFLAG archive_only_flag : 1;
  WFLAG unicode_flag : 1;
  WFLAG : 9;
  UBYTE reserved[64-20];
  };

struct FBLENTRY {
  TCHAR pathname[_MAX_PATH + 1];
#if !defined(_UNICODE)
  UBYTE reserved[512 - (_MAX_PATH + 1)];
#endif
  };

struct FILEBACKUPLIST {
  FBLHEADER fblheader;
  FBLENTRY fblentry[1];
  };

// Directory path descriptor

struct DIRPATHDESC {
  const TCHAR *pathnameP;         // input directory or file pathname
  const TCHAR *out_pathnameP;     // output directory pathname (filename ignored)
  BOOL fullpath_flag;
  BOOL recurse_flag;
  BOOL hidden_flag;
  BOOL system_flag;
  BOOL archive_only_flag;
  };

// Extended file information

struct EXTFILEINFO {
  char *associated_filnam;  // associated filename (or NULL)
  UBYTE *client_dataP;      // pointer to client specific data
  UBYTE client_data_len;    // client data length
  UBYTE reserved[3];
  };

// Directory tree file node.

class FILENODE {
  public:
    FILENODE(TCHAR *pathnameP, FILEINFO *finfoP, QUE *qhdP);
    FILENODE(TCHAR *pathnameP, FILENODE *fileP, QUE *qhdP);
    ~FILENODE();

    // Get a pointer to the source filename.
    const TCHAR *GetSourceFileName(BOOL alt_flag = FALSE)
      {return ((alt_flag && (src_alt_nameP != NULL)) ? src_alt_nameP : src_nameP);}

    // Get a pointer to the destination filename.
    const TCHAR *GetDestFileName(BOOL long_names_flag);

  public:
    QUE nextfile;                   // next file in queue

    TCHAR *src_pathnameP;           // pointer to source file pathname (in the cache)

    TCHAR *src_nameP;               // pointer to source filename
    TCHAR *src_alt_nameP;           // pointer to source alternative filename (if any)

    TCHAR *dst_iso9660_nameP;       // pointer to destination ISO9660 filename (if any)
    TCHAR *dst_joliet_nameP;        // pointer to destination Joliet filename (if any)

    WFLAG archive_flag : 1;         // archive file attribute
    WFLAG dir_flag : 1;             // directory attribute
    WFLAG hidden_flag : 1;          // hidden file attribute
    WFLAG readonly_flag : 1;        // readonly file attribute
    WFLAG system_flag : 1;          // system file attribute
    WFLAG import_flag : 1;          // file was imported from previous session
    WFLAG reserved_flags : 10;

    SYSDATETIME creation_datetime;  // creation date and time
    SYSDATETIME modified_datetime;  // last modification date and time
    SYSDATETIME accessed_datetime;  // last access date and time

    SLONG lba;                      // file LBA
    ULONG size;                     // file length in bytes

    SLONG iso9660_lba;              // ISO9660 file/directory LBA
    ULONG iso9660_size;             // ISO9660 file/directory size

    SLONG joliet_lba;               // Joliet file/directory LBA
    ULONG joliet_size;              // Joliet file/directory size

    SLONG udf_lba;                  // UDF file/directory LBA
    ULONG udf_size;                 // UDF file/directory size
  };


// Directory tree node.

class DIRNODE {
  public:
    DIRNODE(
       DIRNODE *parent_dirnodeP, FILENODE *parent_filenodeP,
      UBYTE depth, QUE *qhdP);
    ~DIRNODE();

    // Return the number of files in a directory.
    int GetFileCount(BOOL include_files_flag, BOOL include_dirs_flag);

    // Return the number of subdirectories in a directory.
    int GetSubdirCount() {return ABSQ_COUNT(&subdir_qhd);}

    // Get a pointer to either the short/long directory name.
    const TCHAR *GetDirName(BOOL long_names_flag)
      {return parent_filenodeP->GetDestFileName(long_names_flag);}

  private:

  public:
    QUE subdir_qhd;             // subdirectory queuehead
    QUE file_qhd;               // file queuehead
    QUE nextdir;                // next directory at this level

    DIRNODE *parent_dirnodeP;   // pointer to parent directory node
    FILENODE *parent_filenodeP; // pointer to parent file node

    SLONG iso9660_lba;          // ISO9660 directory LBA
    SLONG joliet_lba;           // Joliet directory LBA
    SLONG udf_lba;              // UDF directory LBA

    UWORD iso9660_blkcnt;       // ISO9660 directory block count
    UWORD joliet_blkcnt;        // Joliet directory block count
    UWORD udf_blkcnt;           // UDF directory block count

    UWORD pathtable_recnum;     // pathtable record number

    UBYTE depth;                // depth that this directory resides at
    UBYTE reserved1;
    UWORD reserved2;
  };

class DirectoryTree {
  public:
    // Default constructor
    DirectoryTree();
  
    // Construct a tree from a single directory pathname.
    DirectoryTree(
      TCHAR *pathnameP, TCHAR *out_pathnameP, BOOL fullpath_flag,
      BOOL recurse_flag, BOOL hidden_flag, BOOL system_flag, BOOL archive_only_flag);

    // Construct a tree from a list of directory pathname descriptors.
    DirectoryTree(DIRPATHDESC *pathdesc_vec, int desc_count);

    // Destructor.
    ~DirectoryTree();

    // Merge two trees.
    void DirectoryTree::Merge(
      DirectoryTree *dirtreeP, int duplicate_dir_opt, int duplicate_file_opt,
      BOOL statistics_flag = TRUE);

    // Add a pathname.
    TCHAR *AddPathname(const TCHAR *pathnameP);

    // Misc tree operations.
    int ComputeDepth();
    void CollectStatistics();
    void Dump(const TCHAR *filnam = NULL);
    void GenerateDestinationNames(
      int iso9660_level, int iso9660_character_set,
      BOOL allow_max_filename_len, BOOL joliet_flag);
    void Sort(
      int file_field, BOOL descending_flag, BOOL dirs_first_flag,
      BOOL case_insensitive_flag, BOOL dvdvideo_compatible, BOOL long_names_flag);
    void Sort(BOOL long_names_flag, BOOL dirnodes_only_flag = FALSE);

  private:
    void Cleanup();
    void Initialize();

    DIRNODE *AddDirectoryPathNodes(const TCHAR *pathnameP, BOOL real_path_flag);

    void Build(DIRPATHDESC *pathdesc_vec, int desc_count);
    void Build(DIRPATHDESC *descP);
    void BuildRecurse(
      DIRNODE *dirnodeP, const TCHAR *pathname, const TCHAR *filename,
      BOOL recurse_flag, BOOL hidden_flag, BOOL system_flag, BOOL archive_only_flag);

    void MergeFile(
      DIRNODE *dirP, FILENODE *fileP, int duplicate_dir_opt, int duplicate_file_opt);
    void MergeRecurse(
      DIRNODE *cd_dirP, DIRNODE *hd_dirP, int duplicate_dir_opt, int duplicate_file_opt);

    void ComputeDepthRecurse(DIRNODE *dirnodeP, int *depthP);
    void CollectStatisticsRecurse(DIRNODE *dirnodeP);
    void DumpRecurse(DIRNODE *dirnodeP, FILE *file);
    void GenerateDestinationNamesRecurse(
      DIRNODE *dirnodeP, int iso9660_level, int iso9660_character_set,
      BOOL allow_max_filename_len, BOOL joliet_flag);
    void SortRecurse(DIRNODE *dirnodeP);

  public:
    DIRNODE *pRootDirNode;        // pointer to root directory node

    UWORD nDirCount;              // directory count
    UWORD nMaxDirDepth;           // maximum directory depth

    ULONG nFileCount;             // file count

    ULONG nTotalNewDiscBlkcnt;    // total disc blocks for all new files
    ULONG nTotalImportDiscBlkcnt; // total disc blocks for all imported files
    ULONG nTotalDiscBlkcnt;       // total disc blocks for all new/imported files

  private:
    QUE DummySubdirQhd;         // dummy subdirectory queuehead
    QUE DummyFileQhd;           // dummy file queuehead

    TCHAR **pPathnameVec;       // pointer to vector of pathname pointers
    UWORD nPathnameVecCountMax; // maximum number of entries in pathname vector
    UWORD nPathnameVecCountCur; // current number of entries in pathname vector
  };

// Restore previous packing alignment.

#pragma pack(pop)

#endif
