#if !defined(CDRLIB_CDFS_H)
#define CDRLIB_CDFS_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

// CDROM/DVD Filesystem Types

#define CDFS_TYPE_ISO9660                 0     // ISO9660 filesystem
#define CDFS_TYPE_UDFBRIDGE               1     // UDF-Bridge filesystem
#define CDFS_TYPE_UDF                     2     // UDF filesystem

// File Date Options

#define CDFS_FILEDATE_ORIGINAL            0     // use the original file datetimes
#define CDFS_FILEDATE_SYSTEM              1     // use the current system datetime
#define CDFS_FILEDATE_NEW                 2     // use the specified new datetime

// File Sorting Options

#define CDFS_FILESORT_NAME                0     // sort by file name
#define CDFS_FILESORT_SIZE                1     // sort by file size
#define CDFS_FILESORT_CREATION_DATE       2     // sort by creation date
#define CDFS_FILESORT_MODIFICATION_DATE   3     // sort by modification date
#define CDFS_FILESORT_LASTACCESS_DATE     4     // sort by last access date

// UDF Format

#define CDFS_UDF_FORMAT_UNICODE           0     // store all names/strings in Unicode format
#define CDFS_UDF_FORMAT_ASCII             1     // store all names/strings in ASCII format

// UDF Version Numbers

#define CDFS_UDF_VERSION_102              0     // UDF version 1.02
#define CDFS_UDF_VERSION_150              1     // UDF version 1.50
#define CDFS_UDF_VERSION_201              2     // UDF version 2.01

// ISO9660 Filename Levels

#define CDFS_ISO9660_LEVEL1               0     // maximum 11 characters (8.3 format)
#define CDFS_ISO9660_LEVEL2               1     // maximum 31 characters

// ISO9660 Character Sets

#define CDFS_ISO9660_CHARSET_STANDARD     0     // Standard character set (A-Z, 0-9, and underscores)
#define CDFS_ISO9660_CHARSET_DOS          1     // DOS character set (lowercase letters not allowed)
#define CDFS_ISO9660_CHARSET_ASCII        2     // ASCII character set (lowercase letters allowed)

// ISO9660 Bootable Media Types

#define CDFS_ISO9660_BOOT_MEDIA_CUSTOM    0x00
#define CDFS_ISO9660_BOOT_MEDIA_FLOPPY120 0x01
#define CDFS_ISO9660_BOOT_MEDIA_FLOPPY144 0x02
#define CDFS_ISO9660_BOOT_MEDIA_FLOPPY288 0x03
#define CDFS_ISO9660_BOOT_MEDIA_HARDDISK  0x04

// CDFS Unknown Timezone

#define CDFS_TIMEZONE_UNKNOWN             0x7FFF

//
// CDROM File System Options
//

struct CDFSOPTIONS {
  UBYTE filesystem_type;                // ISO9660, UDF, or UDF-Bridge

  UBYTE file_date_option;               // filename date/time option
  UBYTE file_sort_option;               // filename sorting option (UDF only)

  UBYTE udf_filesystem_format;          // UDF filesystem format (Unicode or ASCII)
  UBYTE udf_filesystem_version;         // UDF filesystem version (only version 1.02 is supported)

  UBYTE iso9660_filename_level;         // ISO9660 filename level 
  UBYTE iso9660_character_set;          // ISO9660 character set (STANDARD, DOS, ASCII)
  UBYTE iso9660_boot_media_type;        // ISO9660 bootable media type

  UWORD iso9660_boot_load_segment;      // ISO9660 boot load segment
  UWORD iso9660_boot_load_count;        // ISO9660 boot load count

  SYSDATETIME creation_datetime;        // filesystem creation datetime
  SYSDATETIME modification_datetime;    // filesystem modification datetime (ISO9660 only)
  SYSDATETIME effective_datetime;       // filesystem effective datetime (ISO9660 only)
  SYSDATETIME expiration_datetime;      // filesystem expiration datetime (ISO9660 only)

  SYSDATETIME file_datetime;            // file/directory datetime

  BOOL iso9660_joliet_flag;             // make a Joliet (long filenames) compatible disc
  BOOL iso9660_bootable_flag;           // make a bootable disc (El Torito specification)
  BOOL iso9660_no_version_numbers;      // do not add ";1" version numbers to filenames
  BOOL iso9660_no_blank_extensions;     // do not add "." to filenames without extensions
  BOOL iso9660_allow_deep_directory;    // allow directories deeper than 8 levels
  BOOL iso9660_allow_max_joliet_len;    // allow maximum joliet name length

  BOOL sort_descending;                 // sort names in descending order (UDF only)
  BOOL sort_directories_first;          // sort directory names before filenames (UDF only)
  BOOL sort_case_insensitive;           // sort names using case insensitive comparison (UDF only)
  BOOL sort_dvdvideo_compatible;        // sort names in DVD-Video compatible order (UDF only)

  char volume_id[33];
  TCHAR volume_id_unicode[17];

  char iso9660_system_id[33];
  char iso9660_volume_set_name[129];
  char iso9660_publisher_name[129];
  char iso9660_preparer_name[129];
  char iso9660_application_name[129];
  char iso9660_copyright_file[38];
  char iso9660_abstract_file[38];
  char iso9660_bibliographic_file[38];

  TCHAR joliet_system_id[17];
  TCHAR joliet_volume_set_name[65];
  TCHAR joliet_publisher_name[65];
  TCHAR joliet_preparer_name[65];
  TCHAR joliet_application_name[65];
  TCHAR joliet_copyright_file[19];
  TCHAR joliet_abstract_file[19];
  TCHAR joliet_bibliographic_file[19];

  TCHAR iso9660_boot_image_filnam[_MAX_PATH + 1];
  char iso9660_boot_developer[25];
  };
  
// Imported session information.

struct CDFSIMPORTINFO {
  BOOL joliet_flag;
  BOOL bootable_flag;

  SLONG iso9660_rootdir_lba;
  ULONG iso9660_rootdir_blkcnt;

  SLONG joliet_rootdir_lba;
  ULONG joliet_rootdir_blkcnt;

  char volume_id[33];
  WCHAR volume_id_unicode[17];

  char iso9660_system_id[33];
  char iso9660_volume_set_name[129];
  char iso9660_publisher_name[129];
  char iso9660_preparer_name[129];
  char iso9660_application_name[129];
  char iso9660_copyright_file[38];
  char iso9660_abstract_file[38];
  char iso9660_bibliographic_file[38];

  WCHAR joliet_system_id[17];
  WCHAR joliet_volume_set_name[65];
  WCHAR joliet_publisher_name[65];
  WCHAR joliet_preparer_name[65];
  WCHAR joliet_application_name[65];
  WCHAR joliet_copyright_file[19];
  WCHAR joliet_abstract_file[19];
  WCHAR joliet_bibliographic_file[19];
  };

struct IMPORTDIRCONTEXT {
  BOOL joliet_flag;         // processing Joliet long filename structures?

  SLONG cur_dir_lba;        // current LBA of directory being imported
  SLONG end_dir_lba;        // last LBA of current directory
  ULONG cur_sector_offset;  // current offset into directory sector

  SLONG dir_entry_lba;      // LBA of last imported directory entry
  ULONG dir_entry_blkcnt;   // block count of last imported directory entry

  FILEINFO finfo;           // FILEINFO for last imported directory entry

  UBYTE sector[2048];       // current directory sector being scanned
  };

// Callback function for reading and writing sectors.

typedef BOOL (CDFSREADCALLBACK)(void *bufP, UWORD blkcnt, SLONG lba, BOOL log_flag);
typedef void (CDFSWRITECALLBACK)(void *bufP, UWORD blkcnt, BOOL log_flag);

// Function prototypes.

SWORD CDFSGetTimezoneOffset();

BOOL CDFSBuildImageFile(
  const TCHAR *filnamP, DirectoryTree *dirtreeP, CDFSOPTIONS *cdfs_optionsP,
  SLONG base_lba, BOOL raw_flag, BOOL overwrite_flag, BOOL log_flag);

SLONG CDFSAssignDirectorySectors(
  DirectoryTree *dirtreeP, CDFSOPTIONS *cdfs_optionsP, SLONG base_lba,
  SLONG *iso9660_dir_lbaP, SLONG *udf_dir_lbaP);

SLONG CDFSAssignFileSectors(DirectoryTree *dirtreeP, CDFSOPTIONS *cdfs_optionsP, SLONG base_lba);
void CDFSAssignFileSectorsRecurse(DIRNODE *dirnodeP, SLONG *lbaP);

void CDFSBuildDirectories(
  DirectoryTree *dirtreeP, CDFSOPTIONS *cdfs_optionsP, CDFSWRITECALLBACK *callback, BOOL log_flag);

void CDFSCopyFiles(
  DirectoryTree *dirtreeP, CDFSOPTIONS *cdfs_optionsP, CDFSWRITECALLBACK *callback, BOOL log_flag);

void CDFSCopyFilesFromCache(
  DirectoryTree *dirtreeP, CDFSWRITECALLBACK *callback, BOOL log_flag);

void CDFSCopyFilesRecurse(
  DIRNODE *dirnodeP, CDFSWRITECALLBACK *callback, BOOL log_flag);

void CDFSCopyFileData(const TCHAR *filnamP, CDFSWRITECALLBACK *callback, BOOL log_flag);

void CDFSWriteSectors(void *bufP, UWORD blocks);

void CDFSWritePaddingBlocks(ULONG blkcnt, CDFSWRITECALLBACK *callback, BOOL log_flag);

int CDFSCopyFilenameToBuffer(
  char *targetP, const char *sourceP, int source_len, BOOL terminator_flag);

int CDFSCopyFilenameToBuffer(
  char *targetP, const WCHAR *sourceP, int source_len, BOOL terminator_flag);

int CDFSCopyFilenameToBuffer(
  WCHAR *targetP, const WCHAR *sourceP, int source_len,
  BOOL terminator_flag, BOOL big_endian_flag);

int CDFSCopyFilenameToBuffer(
  WCHAR *targetP, const char *sourceP, int source_len,
  BOOL terminator_flag, BOOL big_endian_flag);

int CDFSCopyFilenameFromBuffer(
  char *targetP, const char *sourceP, int source_len);

int CDFSCopyFilenameFromBuffer(
  WCHAR *targetP, const char *sourceP, int source_len);

int CDFSCopyFilenameFromBuffer(
  char *targetP, const WCHAR *sourceP, int source_len, BOOL big_endian_flag);

int CDFSCopyFilenameFromBuffer(
  WCHAR *targetP, const WCHAR *sourceP, int source_len, BOOL big_endian_flag);

DirectoryTree *CDFSImportImageFile(
  TCHAR *image_filnamP, BOOL joliet_flag, CDFSIMPORTINFO *importinfoP);

DirectoryTree *CDFSImportISO9660DirectoryTree(
  BOOL joliet_flag, CDFSIMPORTINFO *importinfoP, CDFSREADCALLBACK *callback);

BOOL CDFSScanISO9660VolumeDescriptors(
  SLONG session_lba, CDFSIMPORTINFO *importinfoP, CDFSREADCALLBACK *callback);

// Restore previous packing alignment.

#pragma pack(pop)

#endif
