#if !defined(CDRLIB_CDREADER_H)
#define CDRLIB_CDREADER_H

#if !defined(CDRLIB_ASPI_H)
#include "aspi.h"
#endif

#if !defined(CDRLIB_CDTEXT_H)
#include "cdtext.h"
#endif

#if !defined(CDRLIB_SECTOR_H)
#include "sector.h"
#endif

#if !defined(CDRLIB_DIRTREE_H)
#include "dirtree.h"
#endif

#if !defined(CDRLIB_CDFS_H)
#include "cdfs.h"
#endif

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

#define CDROM_74MIN_BLKCNT        333000    // CDROM block count (74 minute)
#define CDROM_80MIN_BLKCNT        360000    // CDROM block count (80 minute)

#define DVD_SL_BLKCNT             2295104   // DVD block count (single layer)
#define DVD_DL_BLKCNT             4590208   // DVD block count (dual layer)

#define CDROM_TRACK_BLKCNT_MIN    300
#define CDROM_TRACK_BLKCNT_MAX    333000

#define CDROM_LEADIN_TNO          0
#define CDROM_LEADOUT_TNO         0xAA

#define CDROM_TNO_MIN             1
#define CDROM_TNO_MAX             99

#define CDROM_INDEX_MIN           0
#define CDROM_INDEX_MAX           99

#define READERROR_MODE_ABORT      0
#define READERROR_MODE_IGNORE     1
#define READERROR_MODE_REPLACE    2

#define TRACKTYPE_CDDA            0
#define TRACKTYPE_DATA            1
#define TRACKTYPE_MODE0           2
#define TRACKTYPE_MODE1           3
#define TRACKTYPE_MODE2           4
#define TRACKTYPE_CDI             5

#define DATATYPE_CDDA             0
#define DATATYPE_CDROM            1
#define DATATYPE_CDROMXA          2
#define DATATYPE_CDI              3
#define DATATYPE_DVD              4

#define SUBCODE_MODE_NONE         0
#define SUBCODE_MODE_Q            1
#define SUBCODE_MODE_PW           2
#define SUBCODE_MODE_PW_ONLY      3

#define CONTROL_M_PRE_EMPHASIS    0x01    // for audio tracks only
#define CONTROL_M_INCREMENTAL     0x01    // for data tracks only
#define CONTROL_M_COPY_PERMITTED  0x02
#define CONTROL_M_DATA_TRACK      0x04
#define CONTROL_M_FOUR_CHANNEL    0x08

// CDROM and DVD disc types

#define DISCTYPE_CDR              0       // CD-R
#define DISCTYPE_CDR_DD           1       // CD-R Double-Density
#define DISCTYPE_CDRW             2       // CD-RW
#define DISCTYPE_CDRW_DD          3       // CD-RW Double-Density
#define DISCTYPE_DVDR             4       // DVD-R
#define DISCTYPE_DVDR_DL          5       // DVD-R Dual-Layer
#define DISCTYPE_DVDRW            6       // DVD-RW
#define DISCTYPE_DVDPLUSR         7       // DVD+R
#define DISCTYPE_DVDPLUSR_DL      8       // DVD+R Dual-Layer
#define DISCTYPE_DVDPLUSRW        9       // DVD+RW
#define DISCTYPE_DVDRAM           10      // DVD-RAM
#define DISCTYPE_BDR              11      // BD-R (Blu-Ray Recordable)
#define DISCTYPE_BDRE             12      // BD-RE (Blu-Ray Recordable/Erasable)
#define DISCTYPE_CDROM            13      // CDROM
#define DISCTYPE_CDROM_DD         14      // CDROM Double-Density
#define DISCTYPE_DVDROM           15      // DVD-ROM
#define DISCTYPE_BDROM            16      // BD-ROM (Blu-Ray)
#define DISCTYPE_UNKNOWN          255

#define DISCTYPE_M_CDR            (1L << DISCTYPE_CDR)
#define DISCTYPE_M_CDR_DD         (1L << DISCTYPE_DDCDR)
#define DISCTYPE_M_CDRW           (1L << DISCTYPE_CDRW)
#define DISCTYPE_M_CDRW_DD        (1L << DISCTYPE_DDCDRW)
#define DISCTYPE_M_DVDR           (1L << DISCTYPE_DVDR)
#define DISCTYPE_M_DVDR_DL        (1L << DISCTYPE_DVDR_DL)
#define DISCTYPE_M_DVDRW          (1L << DISCTYPE_DVDRW)
#define DISCTYPE_M_DVDPLUSR       (1L << DISCTYPE_DVDPLUSR)
#define DISCTYPE_M_DVDPLUSR_DL    (1L << DISCTYPE_DVDPLUSR_DL)
#define DISCTYPE_M_DVDPLUSRW      (1L << DISCTYPE_DVDPLUSRW)
#define DISCTYPE_M_DVDRAM         (1L << DISCTYPE_DVDRAM)
#define DISCTYPE_M_BDR            (1L << DISCTYPE_BDR)
#define DISCTYPE_M_BDRE           (1L << DISCTYPE_BDRE)
#define DISCTYPE_M_CDROM          (1L << DISCTYPE_CDROM)
#define DISCTYPE_M_CDROM_DD       (1L << DISCTYPE_DDCDROM)
#define DISCTYPE_M_DVDROM         (1L << DISCTYPE_DVDROM)
#define DISCTYPE_M_BDROM          (1L << DISCTYPE_BDROM)

// CDROM and DVD reading/writing speeds

#define SPEED_1X              0
#define SPEED_2X              1
#define SPEED_4X              2
#define SPEED_6X              3
#define SPEED_8X              4
#define SPEED_10X             5
#define SPEED_12X             6
#define SPEED_14X             7
#define SPEED_16X             8
#define SPEED_18X             9
#define SPEED_20X             10
#define SPEED_24X             11
#define SPEED_32X             12
#define SPEED_40X             13
#define SPEED_44X             14
#define SPEED_48X             15
#define SPEED_52X             16
#define SPEED_DVD_1X          17
#define SPEED_DVD_2X          18
#define SPEED_DVD_2POINT4X    19
#define SPEED_DVD_3X          20
#define SPEED_DVD_4X          21
#define SPEED_DVD_5X          22
#define SPEED_DVD_6X          23
#define SPEED_DVD_8X          24
#define SPEED_DVD_10X         25
#define SPEED_DVD_12X         26
#define SPEED_DVD_16X         27
#define SPEED_DVD_18X         28
#define SPEED_DVD_20X         29
#define SPEED_MAX             255

#define SPEED_M_1X            (1L << SPEED_1X)
#define SPEED_M_2X            (1L << SPEED_2X)
#define SPEED_M_4X            (1L << SPEED_4X)
#define SPEED_M_6X            (1L << SPEED_6X)
#define SPEED_M_8X            (1L << SPEED_8X)
#define SPEED_M_10X           (1L << SPEED_10X)
#define SPEED_M_12X           (1L << SPEED_12X)
#define SPEED_M_14X           (1L << SPEED_14X)
#define SPEED_M_16X           (1L << SPEED_16X)
#define SPEED_M_18X           (1L << SPEED_18X)
#define SPEED_M_20X           (1L << SPEED_20X)
#define SPEED_M_24X           (1L << SPEED_24X)
#define SPEED_M_32X           (1L << SPEED_32X)
#define SPEED_M_40X           (1L << SPEED_40X)
#define SPEED_M_44X           (1L << SPEED_44X)
#define SPEED_M_48X           (1L << SPEED_48X)
#define SPEED_M_52X           (1L << SPEED_52X)
#define SPEED_M_DVD_1X        (1L << SPEED_DVD_1X)
#define SPEED_M_DVD_2X        (1L << SPEED_DVD_2X)
#define SPEED_M_DVD_2POINT4X  (1L << SPEED_DVD_2POINT4X)
#define SPEED_M_DVD_3X        (1L << SPEED_DVD_3X)
#define SPEED_M_DVD_4X        (1L << SPEED_DVD_4X)
#define SPEED_M_DVD_5X        (1L << SPEED_DVD_5X)
#define SPEED_M_DVD_6X        (1L << SPEED_DVD_6X)
#define SPEED_M_DVD_8X        (1L << SPEED_DVD_8X)
#define SPEED_M_DVD_10X       (1L << SPEED_DVD_10X)
#define SPEED_M_DVD_12X       (1L << SPEED_DVD_12X)
#define SPEED_M_DVD_16X       (1L << SPEED_DVD_16X)
#define SPEED_M_DVD_18X       (1L << SPEED_DVD_18X)
#define SPEED_M_DVD_20X       (1L << SPEED_DVD_20X)

// CDROM speeds - kilobytes per second (1000 bytes per kilobyte)

#define SPEED_1X_KBPS   (((176400 * 1) + 999) / 1000)
#define SPEED_2X_KBPS   (((176400 * 2) + 999) / 1000)
#define SPEED_4X_KBPS   (((176400 * 4) + 999) / 1000)
#define SPEED_6X_KBPS   (((176400 * 6) + 999) / 1000)
#define SPEED_8X_KBPS   (((176400 * 8) + 999) / 1000)
#define SPEED_10X_KBPS  (((176400 * 10) + 999) / 1000)
#define SPEED_12X_KBPS  (((176400 * 12) + 999) / 1000)
#define SPEED_14X_KBPS  (((176400 * 14) + 999) / 1000)
#define SPEED_16X_KBPS  (((176400 * 16) + 999) / 1000)
#define SPEED_18X_KBPS  (((176400 * 18) + 999) / 1000)
#define SPEED_20X_KBPS  (((176400 * 20) + 999) / 1000)
#define SPEED_24X_KBPS  (((176400 * 24) + 999) / 1000)
#define SPEED_32X_KBPS  (((176400 * 32) + 999) / 1000)
#define SPEED_40X_KBPS  (((176400 * 40) + 999) / 1000)
#define SPEED_44X_KBPS  (((176400 * 44) + 999) / 1000)
#define SPEED_48X_KBPS  (((176400 * 48) + 999) / 1000)
#define SPEED_52X_KBPS  (((176400 * 52) + 999) / 1000)

// DVD speeds - kilobytes per second (1024 bytes per kilobyte)

#define SPEED_DVD_1X_KBPS       1385
#define SPEED_DVD_2X_KBPS       2770
#define SPEED_DVD_2POINT4X_KBPS 3324
#define SPEED_DVD_3X_KBPS       4155
#define SPEED_DVD_4X_KBPS       5540
#define SPEED_DVD_5X_KBPS       6925
#define SPEED_DVD_6X_KBPS       8310
#define SPEED_DVD_8X_KBPS       11080
#define SPEED_DVD_10X_KBPS      13850
#define SPEED_DVD_12X_KBPS      16620
#define SPEED_DVD_16X_KBPS      22160
#define SPEED_DVD_18X_KBPS      24930
#define SPEED_DVD_20X_KBPS      27700

// Media Catalog Number

struct MCN {
  char number[13];
  };

// International Standard Recording Code

struct ISRC {
  char code[12];
  };

// Minutes-Seconds-Frames

struct MSF {
  UBYTE minutes;
  UBYTE seconds;
  UBYTE frames;
  };

// Snapshot interface (for Windows)

#define JITTER_MODE_AUTO        0
#define JITTER_MODE_DISABLE     1
#define JITTER_MODE_ENABLE      2

#define SCAN_SUBCODE_AUTO       0
#define SCAN_SUBCODE_DISABLE    1
#define SCAN_SUBCODE_FIXED      2
#define SCAN_SUBCODE_QUICK      3
#define SCAN_SUBCODE_FULL       4

//
// READ TOC command
//

#define TOC_FORMAT_TRACKS       0
#define TOC_FORMAT_LASTSESSION  1
#define TOC_FORMAT_SUBCODES     2
#define TOC_FORMAT_PMA          3
#define TOC_FORMAT_ATIP         4
#define TOC_FORMAT_CDTEXT       5

struct READTOC {
  UBYTE cmd;
  BFLAG : 1;
  BFLAG msf : 1;
  BFLAG : 3;
  BFLAG lun : 3;
  BFLAG format : 4;   // new format field
  BFLAG : 4;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE track;
  UBYTE data_len[2];
  CONTROL control;
  };

struct TRACKTOCDESC {
  BFLAG : 8;
  BFLAG control : 4;
  BFLAG adr : 4;
  UBYTE track;
  BFLAG : 8;
  UBYTE lba[4];
  };

struct LASTSESSIONTOCDESC {
  UBYTE session;
  BFLAG control : 4;
  BFLAG adr : 4;
  UBYTE first_track;
  BFLAG : 8;
  UBYTE lba[4];
  };

struct SUBCODETOCDESC {
  UBYTE session;
  BFLAG control : 4;
  BFLAG adr : 4;
  UBYTE tno;
  UBYTE point;
  UBYTE min;
  UBYTE sec;
  UBYTE frame;
  UBYTE zero;
  UBYTE pmin;
  UBYTE psec;
  UBYTE pframe;
  };

// Table of Contents

struct TRACKTOC {
  UBYTE len[2];
  UBYTE first_track;
  UBYTE last_track;
  TRACKTOCDESC trackdesc[100];
  };

struct LASTSESSIONTOC {
  UBYTE len[2];
  union {
    UBYTE first_session;
    UBYTE session_count;
    };
  union {
    UBYTE last_session;
    UBYTE unfinished_session;
    };
  LASTSESSIONTOCDESC sessiondesc;
  };

struct SUBCODETOC {
  UBYTE len[2];
  UBYTE first_session;
  UBYTE last_session;
  SUBCODETOCDESC subcodedesc[200];
  };

struct ATIPTOC {
  UBYTE len[2];
  UBYTE reserved1;
  UBYTE reserved2;
  BFLAG reference_speed : 3;
  BFLAG ddcd_flag: 1;
  BFLAG itwp : 3;
  BFLAG : 1;
  BFLAG : 6;
  BFLAG uru_flag : 1;
  BFLAG : 1;
  BFLAG a3_valid : 1;
  BFLAG a2_valid : 1;
  BFLAG a1_valid : 1;
  BFLAG disc_subtype : 3;
  BFLAG disc_type : 1;
  BFLAG : 1;
  BFLAG : 8;
  MSF leadin_msf;
  BFLAG : 8;
  MSF max_leadout_msf;
  BFLAG : 8;
  BFLAG highest_clv_speed : 4;
  BFLAG lowest_clv_speed : 3;
  BFLAG : 1;
  BFLAG : 1;
  BFLAG power_y_value : 3;
  BFLAG power_mult_factor : 3;
  BFLAG : 1;
  BFLAG : 4;
  BFLAG erase_write_power_ratio : 3;
  BFLAG : 1;
  BFLAG : 8;
  UBYTE a2_values[3];
  BFLAG : 8;
  UBYTE a3_values[3];
  BFLAG : 8;
  };

struct CDTEXTTOC {
  UBYTE len[2];
  UBYTE reserved1;
  UBYTE reserved2;
  CDTEXTPACK cdtextpack[255];
  };

// Toshiba style MODE1 Subcode-Q data (Position/Time)

struct TOSHIBASUBQ1 {
  UBYTE track;
  UBYTE index;
  UBYTE rel_minutes;
  UBYTE rel_seconds;
  UBYTE rel_frames;
  UBYTE minutes;
  UBYTE seconds;
  UBYTE frames;
  UBYTE mbz[6];
  };

// Toshiba style MODE2 Subcode-Q data (MCN)

struct TOSHIBASUBQ2 {
  UBYTE mcn[13];
  UBYTE frames;
  };

// Toshiba style MODE3 Subcode-Q data (ISRC)

struct TOSHIBASUBQ3 {
  UBYTE isrc[12];
  UBYTE frames;
  UBYTE reserved;
  };

// Union of Toshiba style Subcode-Q data modes.

struct TOSHIBASUBQ {
  UBYTE control;
  UBYTE mode;
  union {
    TOSHIBASUBQ1 subqmode1;
    TOSHIBASUBQ2 subqmode2;
    TOSHIBASUBQ3 subqmode3;
    };
  };

//
// READ SUBCHANNEL command
//

#define SUBCHAN_FORMAT_SUBQ         0x00
#define SUBCHAN_FORMAT_CURPOS       0x01
#define SUBCHAN_FORMAT_MCN          0x02
#define SUBCHAN_FORMAT_ISRC         0x03
#define SUBCHAN_FORMAT_APPCODE      0xF0

struct RDSUBCHAN {
  UBYTE cmd;
  BFLAG : 1;
  BFLAG msf_flag : 1;
  BFLAG : 3;
  BFLAG lun : 3;
  BFLAG : 6;
  BFLAG subq_flag : 1;
  BFLAG : 1;
  UBYTE format;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE track;
  UBYTE data_len[2];
  CONTROL control;
  };

// Subchannel header

struct SUBCHANHEAD {
  BFLAG : 8;
  UBYTE audio_status;
  UBYTE data_len[2];
  };

// Subchannel "Sub-Q Channel Data"

struct SUBCHAN_SUBQ {
  UBYTE format;
  BFLAG control : 4;
  BFLAG adr : 4;
  UBYTE track;
  UBYTE index;
  UBYTE absadr[4];
  UBYTE reladr[4];
  BFLAG : 7;
  BFLAG mcn_valid : 1;
  UBYTE mcn[15];
  BFLAG : 7;
  BFLAG isrc_valid : 1;
  UBYTE isrc[15];
  };

// Subchannel "Current Position Data"

struct SUBCHAN_CURPOS {
  UBYTE format;
  BFLAG control : 4;
  BFLAG adr : 4;
  UBYTE track;
  UBYTE index;
  UBYTE absadr[4];
  UBYTE reladr[4];
  };

// Subchannel "Media Catalog Number"

struct SUBCHAN_MCN {
  UBYTE format;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 7;
  BFLAG mcn_valid : 1;
  MCN mcn;
  BFLAG : 8;
  BFLAG : 8;
  };

// Subchannel "International Standard Recording Code"

struct SUBCHAN_ISRC {
  UBYTE format;
  BFLAG control : 4;
  BFLAG adr : 4;
  UBYTE track;
  BFLAG : 8;
  BFLAG : 7;
  BFLAG isrc_valid : 1;
  ISRC isrc;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  };

// Subchannel "Disc Application Code"

struct SUBCHAN_APPCODE {
  UBYTE format;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG appcode : 7;
  BFLAG : 1;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  };

// Subchannel information

struct SUBCHANNEL {
  SUBCHANHEAD h;
  union {
    SUBCHAN_SUBQ subq;
    SUBCHAN_CURPOS curpos;
    SUBCHAN_MCN mcn;
    SUBCHAN_ISRC isrc;
    SUBCHAN_APPCODE appcode;
    };
  };

// "READ CDDA" command

#define READCDDA_SUBCODE_NONE       0x00
#define READCDDA_SUBCODE_Q          0x01
#define READCDDA_SUBCODE_PW         0x02
#define READCDDA_SUBCODE_PW_ONLY    0x03

struct READCDDA {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  UBYTE lba[4];
  UBYTE blocks[4];
  UBYTE subcode_type;
  CONTROL control;
  };

// "READ CD" command

#define READCD_SECTORTYPE_ANY         0x00
#define READCD_SECTORTYPE_CDDA        0x01
#define READCD_SECTORTYPE_MODE1       0x02
#define READCD_SECTORTYPE_MODE2       0x03
#define READCD_SECTORTYPE_MODE2FORM1  0x04
#define READCD_SECTORTYPE_MODE2FORM2  0x05

#define READCD_SUBCHANNEL_NONE        0x00
#define READCD_SUBCHANNEL_RAW         0x01
#define READCD_SUBCHANNEL_Q           0x02
#define READCD_SUBCHANNEL_PW          0x04

#define READCD_M_RESERVED             0x01
#define READCD_M_C2BITS               0x02
#define READCD_M_C2BITS_BLKERR        0x04
#define READCD_M_EDCECC               0x08
#define READCD_M_USERDATA             0x10
#define READCD_M_HEADER               0x20
#define READCD_M_SUBHEADER            0x40
#define READCD_M_ALL_HEADERS          0x60
#define READCD_M_SYNC                 0x80

struct READCD {
  UBYTE cmd;
  BFLAG reladr : 1;
  BFLAG : 1;
  BFLAG sectortype : 3;
  BFLAG : 3;
  UBYTE lba[4];
  UBYTE blocks[3];
  UBYTE data_flags; 
  BFLAG subchannel_mode : 3;
  BFLAG : 5;
  CONTROL control;
  };

// "SET CD SPEED" command

#define ROTATION_CONTROL_CLV  0
#define ROTATION_CONTROL_CAV  1

struct SETCDSPEED {
  UBYTE cmd;
  BFLAG rotation_control: 2;
  BFLAG : 6;
  UBYTE read_speed[2];    // kilobytes per second
  UBYTE write_speed[2];   // kilobytes per second
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  CONTROL control;
  };

// "PLAY AUDIO" command

struct PLAYAUDIO10 {
  UBYTE cmd;
  BFLAG reladr : 1;
  BFLAG : 4;
  BFLAG lun : 3;
  UBYTE lba[4];
  BFLAG : 8;
  UBYTE blocks[2];
  CONTROL control;
  };

struct PLAYAUDIO12 {
  UBYTE cmd;
  BFLAG reladr : 1;
  BFLAG : 4;
  BFLAG lun : 3;
  UBYTE lba[4];
  UBYTE blocks[4];
  BFLAG : 8;
  CONTROL control;
  };

// "PLAY AUDIO MSF" command

struct PLAYAUDIOMSF {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  BFLAG : 8;
  MSF start_msf;
  MSF end_msf;
  CONTROL control;
  };

// "PLAY AUDIO TRACK" command

struct PLAYAUDIOTRACK {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  BFLAG : 8;
  BFLAG : 8;
  UBYTE start_track;
  UBYTE start_index;
  BFLAG : 8;
  UBYTE end_track;
  UBYTE end_index;
  CONTROL control;
  };

// "PAUSE/RESUME" command

struct PAUSERESUME {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG resume_flag : 1;
  BFLAG : 7;
  CONTROL control;
  };

// "STOP PLAY/SCAN" command

struct STOPPLAYSCAN {
  UBYTE cmd;
  BFLAG : 5;
  BFLAG lun : 3;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  BFLAG : 8;
  CONTROL control;
  };

// "READ DVD STRUCTURE" command

#define MMC_DVD_BOOKTYPE_DVDROM       0x00
#define MMC_DVD_BOOKTYPE_DVDRAM       0x01
#define MMC_DVD_BOOKTYPE_DVDR         0x02
#define MMC_DVD_BOOKTYPE_DVDRW        0x03
#define MMC_DVD_BOOKTYPE_DVDPLUSR     0x08
#define MMC_DVD_BOOKTYPE_DVDPLUSRW    0x09

struct READDVDSTRUCT {
  UBYTE cmd;
  UBYTE reserved;
  UBYTE address[4];
  UBYTE layer;
  UBYTE format;
  UBYTE param_len[2];
  BFLAG : 7;
  BFLAG agid_flag : 1;
  CONTROL control;
  };

struct DVDLAYERDESC {
  UBYTE data_len[2];
  UBYTE reserved1;
  UBYTE reserved2;
  BFLAG part_version : 4;
  BFLAG book_type : 4;
  BFLAG max_read_rate : 4;
  BFLAG disc_diameter : 4;
  BFLAG layer_type : 4;
  BFLAG track_path : 1;
  BFLAG layer_count : 2;
  BFLAG : 1;
  BFLAG track_density : 4;
  BFLAG linear_density : 4;
  UBYTE first_physical_sector[4];
  UBYTE last_physical_sector[4];
  UBYTE last_sector_layer0[4];
  BFLAG : 7;
  BFLAG bca_flag : 1;
  UBYTE media_specific[2031];
  };

//**********************************************************************
//
// CDROM Reader class
//
//**********************************************************************

// Symbol classes for factory functions.

class CDReaderGenericATAPI {};
class CDReaderGenericSCSI {};
class CDReaderKenwood52X {};
class CDReaderMatshita4X {};
class CDReaderMatshita6X {};
class CDReaderMatshita8X {};
class CDReaderMatshita12X {};
class CDReaderMatshita24X {};
class CDReaderNEC3X {};
class CDReaderNEC4X {};
class CDReaderNEC6X {};
class CDReaderNEC8X {};
class CDReaderNEC16X {};
class CDReaderNEC24X {};
class CDReaderNEC32X {};
class CDReaderNEC40X {};
class CDReaderPioneer4X {};
class CDReaderPioneer10X {};
class CDReaderPioneer12X {};
class CDReaderPioneer24X {};
class CDReaderPioneer32X {};
class CDReaderPioneer36X {};
class CDReaderPioneer40X {};
class CDReaderPioneerDVD32X {};
class CDReaderPlextor4X {};
class CDReaderPlextor6X {};
class CDReaderPlextor8X {};
class CDReaderPlextor12X {};
class CDReaderPlextor20X {};
class CDReaderPlextor32X {};
class CDReaderPlextor40X {};
class CDReaderSony2X {};
class CDReaderSony4X {};
class CDReaderSony12X {};
class CDReaderSony24X {};
class CDReaderTeac6X {};
class CDReaderTeac16X {};
class CDReaderTeac32X {};
class CDReaderToshiba2X {};
class CDReaderToshiba4X {};
class CDReaderToshiba6X {};
class CDReaderToshiba12X {};
class CDReaderToshiba14X {};
class CDReaderToshiba24X {};
class CDReaderToshiba32X {};
class CDReaderToshiba40X {};
class CDReaderToshibaDVD32X {};

class CDReader : public ASPIDevice {
  public:
    CDReader(ADCB *adcbP) : ASPIDevice(adcbP) {}

    // Factory functions
    static CDReader *make(CDReaderGenericATAPI, ADCB *adcbP);
    static CDReader *make(CDReaderGenericSCSI, ADCB *adcbP);
    static CDReader *make(CDReaderKenwood52X, ADCB *adcbP);
    static CDReader *make(CDReaderMatshita4X, ADCB *adcbP);
    static CDReader *make(CDReaderMatshita6X, ADCB *adcbP);
    static CDReader *make(CDReaderMatshita8X, ADCB *adcbP);
    static CDReader *make(CDReaderMatshita12X, ADCB *adcbP);
    static CDReader *make(CDReaderMatshita24X, ADCB *adcbP);
    static CDReader *make(CDReaderNEC3X, ADCB *adcbP);
    static CDReader *make(CDReaderNEC4X, ADCB *adcbP);
    static CDReader *make(CDReaderNEC6X, ADCB *adcbP);
    static CDReader *make(CDReaderNEC8X, ADCB *adcbP);
    static CDReader *make(CDReaderNEC16X, ADCB *adcbP);
    static CDReader *make(CDReaderNEC24X, ADCB *adcbP);
    static CDReader *make(CDReaderNEC32X, ADCB *adcbP);
    static CDReader *make(CDReaderNEC40X, ADCB *adcbP);
    static CDReader *make(CDReaderPioneer4X, ADCB *adcbP);
    static CDReader *make(CDReaderPioneer10X, ADCB *adcbP);
    static CDReader *make(CDReaderPioneer12X, ADCB *adcbP);
    static CDReader *make(CDReaderPioneer24X, ADCB *adcbP);
    static CDReader *make(CDReaderPioneer32X, ADCB *adcbP);
    static CDReader *make(CDReaderPioneer36X, ADCB *adcbP);
    static CDReader *make(CDReaderPioneer40X, ADCB *adcbP);
    static CDReader *make(CDReaderPioneerDVD32X, ADCB *adcbP);
    static CDReader *make(CDReaderPlextor4X, ADCB *adcbP);
    static CDReader *make(CDReaderPlextor6X, ADCB *adcbP);
    static CDReader *make(CDReaderPlextor8X, ADCB *adcbP);
    static CDReader *make(CDReaderPlextor12X, ADCB *adcbP);
    static CDReader *make(CDReaderPlextor20X, ADCB *adcbP);
    static CDReader *make(CDReaderPlextor32X, ADCB *adcbP);
    static CDReader *make(CDReaderPlextor40X, ADCB *adcbP);
    static CDReader *make(CDReaderSony2X, ADCB *adcbP);
    static CDReader *make(CDReaderSony4X, ADCB *adcbP);
    static CDReader *make(CDReaderSony12X, ADCB *adcbP);
    static CDReader *make(CDReaderSony24X, ADCB *adcbP);
    static CDReader *make(CDReaderTeac6X, ADCB *adcbP);
    static CDReader *make(CDReaderTeac16X, ADCB *adcbP);
    static CDReader *make(CDReaderTeac32X, ADCB *adcbP);
    static CDReader *make(CDReaderToshiba2X, ADCB *adcbP);
    static CDReader *make(CDReaderToshiba4X, ADCB *adcbP);
    static CDReader *make(CDReaderToshiba6X, ADCB *adcbP);
    static CDReader *make(CDReaderToshiba12X, ADCB *adcbP);
    static CDReader *make(CDReaderToshiba14X, ADCB *adcbP);
    static CDReader *make(CDReaderToshiba24X, ADCB *adcbP);
    static CDReader *make(CDReaderToshiba32X, ADCB *adcbP);
    static CDReader *make(CDReaderToshiba40X, ADCB *adcbP);
    static CDReader *make(CDReaderToshibaDVD32X, ADCB *adcbP);

    // Get a list of installed CD reader devices.
    static int GetDeviceObjects(CDReader **device_list, int list_size);

    // Jacket routines
    int EjectDisc(BOOL sig_flag);
    int LoadDisc(BOOL sig_flag);
    int LoadDisc(BOOL wait_flag, BOOL sig_flag);
    int LoadDisc(UBYTE timeout, BOOL wait_flag, BOOL sig_flag);
    int StartPlayAudio(SLONG lba, ULONG blocks, BOOL sig_flag);
    int StartPlayAudio(MSF *start_msf, MSF *end_msf, BOOL sig_flag);
    int StartPlayAudio(UBYTE track, BOOL sig_flag);
    int PausePlayAudio(BOOL sig_flag);
    int ResumePlayAudio(BOOL sig_flag);
    int StopPlayScan(BOOL sig_flag);
    int ReadDiscAudio(
      void *bufP, UWORD blklen, UWORD blocks, SLONG lba, BOOL sig_flag);
    int ReadDiscAudio(
      void *bufP, UWORD blklen, UWORD blocks, SLONG lba,
      UBYTE retry_count, UBYTE error_mode, UWORD *badblocksP, BOOL sig_flag);
    int ReadDiscData(
      void *bufP, UWORD blklen, UWORD blocks, SLONG lba, BOOL sig_flag);
    int ReadDiscData(
      void *bufP, UWORD blklen, UWORD blocks, SLONG lba,
      UBYTE retry_count, UBYTE error_mode, UBYTE datatype,
      UWORD *badblocksP, BOOL sig_flag);
    int ReadSubchannel(
      SUBCHANNEL *subchannel, UBYTE format, UBYTE track,
      BOOL subq_flag, BOOL msf_flag, BOOL sig_flag);
    int ReadTOC(void *tocbufP, UWORD toclen, UBYTE format, BOOL sig_flag);
    int SetBlockLength(UBYTE datatype, UWORD blklen, BOOL sig_flag);
    int SetSpindleSpeed(UBYTE speed, BOOL data_flag, BOOL sig_flag);

    // Virtual functions
    virtual int GetDiscSessionCount();

    virtual int EjectDisc()
      {return ExecuteStartStopUnit (FALSE, TRUE, FALSE);}
    virtual int LoadDisc()
      {return ExecuteStartStopUnit (TRUE, TRUE, FALSE);}

    virtual int ExecuteReadTOC(void *tocbufP, UWORD toclen, UBYTE format)
      {return ExecuteOldReadTOC (tocbufP, toclen, format);}

    virtual void PostProcessReadBuffer(void *bufP, UWORD blklen, UWORD blocks)
      {NULL;}

    virtual int ReadDiscAudio(void *bufP, UWORD blklen, UWORD blocks, SLONG lba)
      {return ExecuteReadCDDA (bufP, blklen, blocks, lba);}
    virtual int ReadDiscData(void *bufP, UWORD blklen, UWORD blocks, SLONG lba)
      {return ExecuteReadBlocks (bufP, blklen, blocks, lba);}

    virtual int SetBlockLength(UBYTE datatype, UWORD blklen)
      {return (datatype == DATATYPE_CDDA) ? E_Success : SetBlockDesc (0, blklen, 0);}
    virtual int SetSpindleSpeed(UBYTE speed, BOOL data_flag) {return E_Success;}

    // typedef for extract callback
    typedef void (EXTRACTCALLBACK)(void *handle, void *bufP, UWORD blklen, UWORD blocks);

    // High level functions
    void DumpAllModePages();
    void DumpAllFormatDescriptors();
    void ExtractSectors(
      UBYTE datatype, UWORD blklen, SLONG lba, ULONG blkcnt,
      EXTRACTCALLBACK *callback, void *handle,
      UBYTE jitter_mode, UBYTE retry_count, UBYTE error_mode,
      BOOL flip_flag, BOOL cache_flag, BOOL log_flag);
    void ExtractSectorsToFile (
      FILEHANDLE handle, UBYTE datatype, UWORD blklen, SLONG lba, ULONG blkcnt,
      UBYTE jitter_mode, UBYTE retry_count, UBYTE error_mode,
      BOOL flip_flag, BOOL log_flag);
    SLONG FindLastAudioSector();
    SLONG FindLastDataSector();
    SLONG FindAudioTrackEnd(SLONG max_end_lba, UWORD offset);
    SLONG FindMode1TrackEnd(SLONG max_end_lba, UWORD offset);
    SLONG FindMode2TrackEnd(SLONG max_end_lba, UWORD offset);
    BOOL GetLastSessionLBA(SLONG *lbaP);
    BOOL GetLastSessionVolumeInfo(CDFSIMPORTINFO *importinfoP);
    BOOL GetNextSessionLBA(SLONG *lbaP);
    int GetSectorType(SLONG lba);
    int GetTrackType(SLONG lba);
    UWORD GetPostgapBlockCount();
    int HandleAudioReadErrors(
      void *bufP, UWORD blklen, UWORD blocks, SLONG lba, UWORD *badblocksP);
    int HandleDataReadErrors(
      void *bufP, UBYTE datatype, UWORD blklen, UWORD blocks, SLONG lba,
      UWORD *badblocksP, BOOL repair_flag, BOOL scramble_flag);
    DirectoryTree *ImportLastSession(
      BOOL joliet_flag, CDFSIMPORTINFO *importinfoP);
    int ReadMediaCatalogNumber(MCN *mcnP, BOOL sig_flag = FALSE);
    int ReadTrackISRC(UBYTE track, ISRC *isrcP, BOOL sig_flag = FALSE);
    int ReadRawSector (CDSector *sectorP, SLONG lba, BOOL set_blklen_flag);
    void ReloadDisc();

    // Get functions
    ULONG GetCRC()                {return CRC;}
    ULONG GetReadFlags()          {return ReadFlags;}
    ULONG GetAudioReadSpeedMask() {return AudioReadSpeedMask;}
    ULONG GetDataReadSpeedMask()  {return DataReadSpeedMask;}
    UWORD GetRawReadBlklen()      {return RawReadBlklen;}
    UBYTE GetMaxDataReadSpeed()   {return MaxDataReadSpeed;}
    UBYTE GetMaxAudioReadSpeed()  {return MaxAudioReadSpeed;}

    // Set functions
    void SetCRC(ULONG crc)        {CRC = crc;}

    // Flags
    enum CDReaderFlag {
      crfReadCDDA       = 0x0001, // supports reading of CDDA
      crfReadSUBQ       = 0x0002, // supports reading of Subcode-Q
      crfReadCDG        = 0x0004, // supports reading of CD+G
      crfReadCDTEXT     = 0x0008, // supports reading of CDTEXT
      crfFixJitter      = 0x0010, // jitter correction required for CDDA extraction
      crfComputeCRC     = 0x0020, // compute running CRC on all data reads
      crfUnscrambleRaw  = 0x0040  // unscramble raw sectors
      };

    // Flag functions
    void SetFlag(CDReaderFlag mask) {ReadFlags |= (UWORD)(mask);}
    void ClearFlag(CDReaderFlag mask) {ReadFlags &= ~(UWORD)(mask);}
    BOOL IsFlagSet(CDReaderFlag mask)
      {return (ReadFlags & (ULONG)(mask)) ? TRUE : FALSE;}
    BOOL IsFlagClear(CDReaderFlag mask)
      {return (ReadFlags & (ULONG)(mask)) ? FALSE : TRUE;}

    // Read TOC functions
    int ExecuteNewReadTOC(void *tocbufP, UWORD toclen, UBYTE format);
    int ExecuteOldReadTOC(void *tocbufP, UWORD toclen, UBYTE format);

  protected:
    // Functions that execute an ASPI command.
    int ExecutePauseResume(BOOL resume_flag);
    int ExecutePlayAudio(SLONG lba, ULONG blocks);
    int ExecutePlayAudio(MSF *start_msf, MSF *end_msf);
    int ExecutePlayAudio(UBYTE track);
    int ExecuteReadCD(
      void *bufP, UBYTE datatype, UWORD blklen, UWORD blocks, SLONG lba);
    int ExecuteReadCDDA (
      void *bufP, UWORD blklen, UWORD blocks, SLONG lba);
    int ExecuteReadDVDStructure(
      UBYTE format, UBYTE layer, ULONG address, void *bufP, UWORD buflen);
    int ExecuteReadSubchannel(
      SUBCHANNEL *subchannel, UBYTE format, UBYTE track,
      BOOL subq_flag, BOOL msf_flag);
    int ExecuteSetSpeed(UBYTE read_speed, UBYTE write_speed);
    int ExecuteStopPlayScan();

  private:
    // Functions called by ExtractSectors
    void ExtractAudio(
      UWORD blklen, SLONG start_lba, ULONG blkcnt,
      void *disccacheP, EXTRACTCALLBACK *callback, void *callback_handle,
      UBYTE jitter_mode, UBYTE retry_count, UBYTE error_mode,
      BOOL flip_flag, BOOL log_flag);
    void ExtractData(
      UBYTE datatype, UWORD in_blklen, UWORD out_blklen, SLONG start_lba, ULONG blkcnt,
      void *disccacheP, EXTRACTCALLBACK *callback, void *callback_handle,
      UBYTE retry_count, UBYTE error_mode, BOOL check_raw_flag, BOOL flip_flag, BOOL log_flag);

  protected:
    ULONG ReadFlags;          // Misc flags
    ULONG CRC;                // Running CRC of data read
    ULONG AudioReadSpeedMask; // Supported audio reading speeds
    ULONG DataReadSpeedMask;  // Supported data reading speeds
    UWORD RawReadBlklen;      // Maximum data block length
    UWORD ReadBlockLength;    // Current reading block length
    UBYTE MaxAudioReadSpeed;  // Maximum audio reading speed
    UBYTE MaxDataReadSpeed;   // Maximum data reading speed
    UBYTE ReadDatatype;       // Current reading datatype
    UBYTE Reserved;

  private:
    CDReader(const CDReader&);
    CDReader& operator =(const CDReader&);
  };

// Misc prototypes.

SLONG ConvertMSF2LBA(MSF *msf);
SLONG ConvertMSF2LBA(UBYTE minutes, UBYTE seconds, UBYTE frames);

ULONG ConvertMSF2Blocks(MSF *msf);
ULONG ConvertMSF2Blocks(UBYTE minutes, UBYTE seconds, UBYTE frames);

void ConvertLBA2MSF(SLONG lba, MSF *msf);
void ConvertBlocks2MSF(ULONG blkcnt, MSF *msf);

int CompareMSF(MSF *msf, UBYTE minutes, UBYTE seconds, UBYTE frames);
int CompareMSF(MSF *msf1P, MSF *msf2);

// Restore previous packing alignment.

#pragma pack(pop)

#endif
