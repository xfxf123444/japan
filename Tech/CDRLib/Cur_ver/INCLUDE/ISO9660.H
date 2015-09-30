// 13-sep-04 (sjk) ISO9660_LEVEL and ISO9660_CHARSET options.

#if !defined(CDRLIB_ISO9660_H)
#define CDRLIB_ISO9660_H

#if !defined(CDRLIB_ELTORITO_H)
#include "eltorito.h"
#endif

#if !defined(CDRLIB_DIRTREE_H)
#include "dirtree.h"
#endif

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

// Volume descriptor types.

#define ISO9660_VD_BOOT           0x00
#define ISO9660_VD_PRIMARY        0x01
#define ISO9660_VD_SUPPLEMENTAL   0x02
#define ISO9660_VD_END            0xFF

#define ISO9660_VOLUME_DESC_ID    "CD001"

// File flags.

#define ISO9660_M_HIDDEN_FILE     0x01
#define ISO9660_M_DIRECTORY       0x02
#define ISO9660_M_ASSOCIATED_FILE 0x04

// Maximum directory depth (including root).

#define ISO9660_MAX_DIR_DEPTH     8

// Maximum name lengths.

#define ISO9660_MAX_LEVEL1_NAME_LENGTH  12
#define ISO9660_MAX_LEVEL2_NAME_LENGTH  31
#define ISO9660_MAX_JOLIET_NAME_LENGTH  64

// Maximum directory record length.

#define ISO9660_MAX_DIR_RECORD_LENGTH   255

// Record lengths.

#define ISO9660_DIR_RECORD_LEN(namelen)                               \
  (sizeof(ISO9660DIRECTORYREC) + (namelen) - (ODD(namelen) ? 1 : 0))

#define ISO9660_PATHTABLE_RECORD_LEN(namelen)                         \
  (sizeof(ISO9660PATHTABLEREC) + (namelen) - (EVEN(namelen) ? 1 : 0))

// ISO date and time.

struct ISO9660DATETIME {
  UBYTE year;
  UBYTE month;
  UBYTE day;
  UBYTE hour;
  UBYTE minute;
  UBYTE second;
  UBYTE gmt_offset; // offset from Greenwich Mean Time
  };

// Path table record.

struct ISO9660PATHTABLEREC {
  UBYTE name_length;
  UBYTE ext_attr_blkcnt;
  SLONG dir_lba;
  UWORD parent_recnum;
  char name[1];
  };

// Directory record.

struct ISO9660DIRECTORYREC {
  UBYTE record_length;
  UBYTE ext_attr_blkcnt;
  SLONG lba[2];
  ULONG data_length[2];
  ISO9660DATETIME date;
  UBYTE flags;
  UBYTE file_unit_size;
  UBYTE interleave;
  UWORD volume_seq_number[2];
  UBYTE name_length;
  char name[1];
  };

// ISO9660 Primary Volume Descriptor

struct ISO9660PRIMARYVOLUMEDESC {
  UBYTE mbz1[1];
  char system_id[32];
  char volume_id[32];
  UBYTE mbz2[8];
  ULONG volume_blkcnt[2];
  UBYTE mbz3[32];
  UWORD volume_set_count[2];
  UWORD volume_sequence_number[2];
  UWORD logical_blklen[2];
  ULONG pathtable_len[2];
  SLONG le_pathtable_lba;
  SLONG opt_le_pathtable_lba;
  SLONG be_pathtable_lba;
  SLONG opt_be_pathtable_lba;
  ISO9660DIRECTORYREC root_directory;
  char volume_set_name[128];
  char publisher_name[128];
  char preparer_name[128];
  char application_name[128];
  char copyright_file[37];
  char abstract_file[37];
  char bibliographic_file[37];
  char creation_date[17];
  char modification_date[17];
  char expiration_date[17];
  char effective_date[17];
  UBYTE file_structure_version;
  UBYTE mbz4[142];
  UBYTE xa_id[8];
  UWORD xa_flags;
  UBYTE xa_startup[8];
  UBYTE mbz5[1006];
  };

// ISO9660 Joliet Volume Descriptor

struct ISO9660JOLIETVOLUMEDESC {
  UBYTE mbz1[1];
  WCHAR system_id[16];
  WCHAR volume_id[16];
  UBYTE mbz2[8];
  ULONG volume_blkcnt[2];
  UBYTE escape_sequence[3];   // character set escape sequence
  UBYTE mbz3[29];
  UWORD volume_set_count[2];
  UWORD volume_sequence_number[2];
  UWORD logical_blklen[2];
  ULONG pathtable_len[2];
  SLONG le_pathtable_lba;
  SLONG opt_le_pathtable_lba;
  SLONG be_pathtable_lba;
  SLONG opt_be_pathtable_lba;
  ISO9660DIRECTORYREC root_directory;
  WCHAR volume_set_name[64];
  WCHAR publisher_name[64];
  WCHAR preparer_name[64];
  WCHAR application_name[64];
  WCHAR copyright_file[18];
  UBYTE mbz4[1];
  WCHAR abstract_file[18];
  UBYTE mbz5[1];
  WCHAR bibliographic_file[18];
  UBYTE mbz6[1];
  char creation_date[17];
  char modification_date[17];
  char expiration_date[17];
  char effective_date[17];
  UBYTE file_structure_version;
  UBYTE mbz7[1166];
  };

// ISO9660 Terminating Volume Descriptor

struct ISO9660TERMINATINGVOLUMEDESC {
  UBYTE mbz[2041];
  };

// ISO9660 Boot Record Volume Descriptor

struct ISO9660BOOTRECORDVOLUMEDESC {
  char boot_id[32];
  UBYTE mbz1[32];
  SLONG boot_catalog_lba;
  UBYTE mbz2[1973];
  };

// ISO9660 Volume Descriptor

struct ISO9660VOLUMEDESC {
  UBYTE type;
  char id[5];
  UBYTE version;
  union {
    ISO9660PRIMARYVOLUMEDESC primarydesc;
    ISO9660JOLIETVOLUMEDESC jolietdesc;
    ISO9660TERMINATINGVOLUMEDESC terminatingdesc;
    ISO9660BOOTRECORDVOLUMEDESC bootrecorddesc;
    };
  };

// Function prototypes.

void ISO9660AssignDirectorySectors(
  DirectoryTree *dirtreeP, CDFSOPTIONS *cdfs_optionsP, SLONG *lbaP);

void ISO9660AssignDirectorySectorsRecurse(
  DIRNODE *dirnodeP, UBYTE depth, SLONG *lbaP,
  BOOL joliet_flag, BOOL no_version_numbers, BOOL no_blank_extensions);

UWORD ISO9660ComputeDirectoryLength(
  DIRNODE *dirnodeP, BOOL joliet_flag, BOOL no_version_numbers, BOOL no_blank_extensions);

void ISO9660BuildDirectories(
  DirectoryTree *dirtreeP, CDFSOPTIONS *cdfs_optionsP,
  CDFSWRITECALLBACK *callback, BOOL log_flag);

void ISO9660BuildDirectoriesRecurse(
  DIRNODE *dirnodeP, CDFSWRITECALLBACK *callback, UBYTE depth,
  BOOL joliet_flag, BOOL no_version_numbers, BOOL no_blank_extensions, BOOL log_flag);

void ISO9660BuildOneDirectory(
  DIRNODE *dirnodeP, CDFSWRITECALLBACK *callback,
  BOOL joliet_flag, BOOL no_version_numbers, BOOL no_blank_extensions, BOOL log_flag);

ULONG ISO9660ComputePathtableLength(DirectoryTree *dirtreeP, BOOL joliet_flag);

void ISO9660ComputePathtableLengthRecurse(
  DIRNODE *dirnodeP, BOOL joliet_flag, ULONG *lengthP);

void ISO9660BuildPathtables(
  DirectoryTree *dirtreeP, CDFSWRITECALLBACK *callback,
  BOOL joliet_flag, BOOL log_flag);

void ISO9660BuildOnePathtable(
  DirectoryTree *dirtreeP, CDFSWRITECALLBACK *callback,
  BOOL joliet_flag, BOOL big_endian_flag, BOOL log_flag);

void ISO9660BuildOnePathtableRecurse(
  DIRNODE *dirnodeP, CDFSWRITECALLBACK *callback,
  IOBUF *iobufP, ULONG *bytes_in_buffer, UWORD *recnumP, UBYTE depth,
  BOOL joliet_flag, BOOL big_endian_flag, BOOL log_flag);

void ISO9660InitPrimaryVolumeDesc(
  ISO9660VOLUMEDESC *volumedescP, CDFSOPTIONS *cdfs_optionsP, ULONG volume_blkcnt,
  ULONG pathtable_len, SLONG le_pathtable_lba, SLONG be_pathtable_lba,
  SLONG rootdir_lba, UWORD rootdir_blkcnt);

void ISO9660InitJolietVolumeDesc(
  ISO9660VOLUMEDESC *volumedescP, CDFSOPTIONS *cdfs_optionsP, ULONG volume_blkcnt,
  ULONG pathtable_len, SLONG le_pathtable_lba, SLONG be_pathtable_lba,
  SLONG rootdir_lba, UWORD rootdir_blkcnt);

void ISO9660InitBootRecordVolumeDesc(
  ISO9660VOLUMEDESC *volumedescP, ULONG boot_catalog_lba);

void ISO9660InitTermVolumeDesc(ISO9660VOLUMEDESC *volumedescP);

void ISO9660BuildBootCatalog(
  BOOTCATALOG *bootcatalog_vec, const char *developer, UBYTE media_type,
  UWORD load_segment, UWORD load_sector_count, ULONG load_lba);

void ISO9660ConvertDate(SYSDATETIME *datetimeP, ISO9660DATETIME *isodateP);
void ISO9660ConvertDate(ISO9660DATETIME *isodateP, SYSDATETIME *datetimeP);

void ISO9660InitRootDirectory(
  ISO9660DIRECTORYREC *isodirP, SLONG lba, ULONG blkcnt);

// Restore previous packing alignment.

#pragma pack(pop)

#endif
