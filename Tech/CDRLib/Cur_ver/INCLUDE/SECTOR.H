#if !defined(CDRLIB_SECTOR_H)
#define CDRLIB_SECTOR_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

#define MODE0                               0
#define MODE1                               1
#define MODE2                               2

#define SECTORTYPE_UNKNOWN                  0
#define SECTORTYPE_CDDA                     1
#define SECTORTYPE_MODE0                    2
#define SECTORTYPE_MODE1                    3
#define SECTORTYPE_MODE2                    4
#define SECTORTYPE_MODE2FORM1               5
#define SECTORTYPE_MODE2FORM2               6

#define SECTOR_SYNC_LENGTH                  12
#define SECTOR_HEADER_LENGTH                4
#define SECTOR_SUBHEADER_LENGTH             8
#define SECTOR_MODE1_MBZ_LENGTH             8
#define SECTOR_EDC_LENGTH                   4
#define SECTOR_ECC_PPARITY_LENGTH           172
#define SECTOR_ECC_QPARITY_LENGTH           104

#define SECTOR_DATA_BLKLEN                  2048
#define SECTOR_MODE0_BLKLEN                 2336
#define SECTOR_MODE1_BLKLEN                 2048
#define SECTOR_MODE2_BLKLEN                 2336
#define SECTOR_MODE2FORM1_BLKLEN            2048
#define SECTOR_MODE2FORM1_SUBHEADER_BLKLEN  2056
#define SECTOR_MODE2FORM2_BLKLEN            2324
#define SECTOR_MODE2MIXED_BLKLEN            2332
#define SECTOR_RAW_BLKLEN                   2352
#define SECTOR_RAW_NOSYNC_BLKLEN            2340
#define SECTOR_RAW_SUBQ_BLKLEN              2368
#define SECTOR_RAW_SUBPW_BLKLEN             2448
#define SECTOR_CDDA_BLKLEN                  2352
#define SECTOR_CDDA_SUBQ_BLKLEN             2368
#define SECTOR_CDDA_SUBPW_BLKLEN            2448
#define SECTOR_SUBQ_BLKLEN                  16
#define SECTOR_SUBPW_BLKLEN                 96

#define SECTOR_DISK_BLKLEN                  512

#define SUBCODE_BIT_P   7
#define SUBCODE_BIT_Q   6
#define SUBCODE_BIT_R   5
#define SUBCODE_BIT_S   4
#define SUBCODE_BIT_T   3
#define SUBCODE_BIT_U   2
#define SUBCODE_BIT_V   1
#define SUBCODE_BIT_W   0

// Error correction codes.

struct ECC {
  UBYTE p_parity[SECTOR_ECC_PPARITY_LENGTH];
  UBYTE q_parity[SECTOR_ECC_QPARITY_LENGTH];
  };

// Header for all DATA sectors.

struct SECTORHEADER {
  UBYTE minutes;      //
  UBYTE seconds;      // stored in BCD format
  UBYTE frames;       //
  UBYTE mode;
  };

// CDROM-XA submode field.

struct SUBMODE {
  BFLAG eor_flag : 1;
  BFLAG video_flag : 1;
  BFLAG audio_flag : 1;
  BFLAG data_flag : 1;
  BFLAG trigger_flag : 1;
  BFLAG form2_flag : 1;
  BFLAG rt_flag : 1;
  BFLAG eof_flag : 1;
  };

// Subheader for CDROM-XA (Mode2) sectors.

struct SECTORSUBHEADER {
  UBYTE filenum;
  UBYTE channel;
  SUBMODE submode;
  UBYTE coding;
  };

// Mode0 Data Sector.

struct SECTORMODE0 {
  UBYTE userdata[SECTOR_MODE0_BLKLEN];
  };

// Mode1 Data Sector.

struct SECTORMODE1 {
  UBYTE userdata[SECTOR_MODE1_BLKLEN];
  ULONG edc;
  UBYTE mbz[SECTOR_MODE1_MBZ_LENGTH];
  ECC ecc;
  };

// Mode2 Data Sector.

struct SECTORMODE2 {
  UBYTE userdata[SECTOR_MODE2_BLKLEN];
  };

// Mode2 CDROM-XA Data Sector (Form1 and Form2).

struct FORM1 {
  UBYTE userdata[SECTOR_MODE2FORM1_BLKLEN];
  ULONG edc;
  ECC ecc;
  };

struct FORM2 {
  UBYTE userdata[SECTOR_MODE2FORM2_BLKLEN];
  ULONG edc;
  };

struct SECTORMODE2XA {
  SECTORSUBHEADER subheader;      // subheader
  SECTORSUBHEADER subheader_rpt;  // subheader repeat
  union {
    FORM1 form1;                  // Mode2 Form1
    FORM2 form2;                  // Mode2 Form2
    };
  };

// Data Sector (union of all data sector modes).

struct SECTORDATA {
  UBYTE sync[SECTOR_SYNC_LENGTH];
  SECTORHEADER header;
  union {
    SECTORMODE0   mode0;
    SECTORMODE1   mode1;
    SECTORMODE2   mode2;
    SECTORMODE2XA mode2xa;
    };
  };

// CDDA Sector (audio).

struct SECTORCDDA {
  UBYTE userdata[SECTOR_CDDA_BLKLEN];
  };

// Subcode data format returned by most drives. If the drive doesn't
// return the subcode in this format, then we convert to it. All numbers
// are in BCD format.

struct SUBCODEQ {
  BFLAG adr : 4;
  BFLAG control : 4;
  UBYTE track;
  UBYTE index;
  UBYTE rel_minutes;
  UBYTE rel_seconds;
  UBYTE rel_frames;
  UBYTE zero;
  UBYTE minutes;
  UBYTE seconds;
  UBYTE frames;
  UWORD crc;
  UBYTE pad1;
  UBYTE pad2;
  UBYTE pad3;
  UBYTE flags;
  };

struct SUBCODEPACK {
  UBYTE data[24];
  };

struct SUBCODEPW {
  SUBCODEPACK packs[4];
  };

// Misc prototypes.

void BuildDummySector(
  void *bufP, UBYTE datatype, UWORD blklen, SLONG lba, BOOL repair_flag, BOOL scramble_flag);

void WriteEmptySectors(
  FILEHANDLE handle, UBYTE datatype, UWORD blklen, SLONG lba, ULONG blkcnt);

void DumpECC(ECC *eccP);

void InitializeSubcodePQ(SUBCODEPW *subpwP, SUBCODEQ *subqP);

void ConvertSubcodeBinary2BCD(void *bufP, UWORD blklen, UWORD blocks);

//
// CDSector class
//

class CDSector {
  public:
    CDSector();
    BOOL CheckEDC();
    void Dump();
    void Format(
      UBYTE sectortype, SLONG lba, SECTORSUBHEADER *subheader, void *userdata,
      BOOL edc_flag, BOOL leadin_flag, BOOL scramble_flag);
    ULONG ComputeEDC(BOOL update_flag = TRUE);
    void ComputeECC(BOOL empty_flag = FALSE);
    ULONG GetEDC();
    int GetType();
    UBYTE *GetUserData();
    BOOL HasSyncPattern();
    BOOL IsAudio(SLONG lba);
    BOOL IsData(SLONG lba);
    BOOL IsMode1(SLONG lba);
    BOOL IsMode2(SLONG lba);
    BOOL IsEmpty();
    void SetEDC(ULONG edc);
    void Transform(BOOL flip_flag, BOOL reverse_flag, BOOL scramble_flag);
    CDSector *FindSyncSector(
      void *bufP, UWORD blkcnt, UWORD overlap_blkcnt, SLONG *outjitterP);
  public:
    union {
      SECTORCDDA cdda;
      SECTORDATA data;
    };
  };

struct SECTORSUBQ {
  CDSector sector;
  SUBCODEQ subq;
  };

struct SECTORSUBPW {
  CDSector sector;
  SUBCODEPW subpw;
  };

// Restore previous packing alignment.

#pragma pack(pop)

#endif
