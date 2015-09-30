#if !defined(CDRLIB_CDWRITER_H)
#define CDRLIB_CDWRITER_H

#if !defined(CDRLIB_CDREADER_H)
#include "cdreader.h"
#endif

#if !defined(CDRLIB_CDTEXT_H)
#include "cdtext.h"
#endif

#if !defined(CDRLIB_LAYOUT_H)
#include "layout.h"
#endif

#if !defined(CDRLIB_CUE_H)
#include "cue.h"
#endif

#if !defined(CDRLIB_DIRTREE_H)
#include "dirtree.h"
#endif

#if !defined(CDRLIB_CDFS_H)
#include "cdfs.h"
#endif

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

#define WRITEMODE_TRACKATONCE         0x00
#define WRITEMODE_DISCATONCE          0x01
#define WRITEMODE_DISCATONCE_RAW      0x02
#define WRITEMODE_SESSIONATONCE       0x03
#define WRITEMODE_PACKET_VARIABLE     0x04
#define WRITEMODE_PACKET_FIXED        0x05

#define DISC_STATUS_EMPTY             0
#define DISC_STATUS_WRITABLE          1
#define DISC_STATUS_COMPLETE          2
#define DISC_STATUS_UNKNOWN           255

#define LAST_SESSION_STATUS_EMPTY     0
#define LAST_SESSION_STATUS_WRITABLE  1
#define LAST_SESSION_STATUS_COMPLETE  2
#define LAST_SESSION_STATUS_UNKNOWN   255

#define GAPTYPE_PREGAP                0
#define GAPTYPE_POSTGAP               1
#define GAPTYPE_LEADIN                2
#define GAPTYPE_LEADOUT               3

#define FIRST_PREGAP_LBA              0xFFFFFF6A

#define CDRW_DEF_PACKET_SIZE          32

struct RECORDOPTIONS {
  UBYTE speed;                  // recording speed
  UBYTE disc_datatype;          // disc datatype (used by TAO finalize)
  UBYTE reserved[2];
  // Disc-at-once only flags
  BOOL raw_flag;                // use raw mode (if supported)
  // Track-at-once only flags
  BOOL close_session_flag;      // close session (finalize)
  BOOL mode2_flag;              // write sectors in MODE2 format
  BOOL postgap_flag;            // write postgap at end of track
  // DAO and TAO flags
  BOOL multisession_flag;       // open new session
  BOOL underrun_protect_flag;   // enable buffer underrun protection (if supported)
  BOOL test_flag;               // test recording mode
  BOOL verify_flag;             // verify disc after recording
  BOOL beep_flag;               // beep user at end of recording
  BOOL eject_flag;              // eject disc at end of recording
  BOOL log_flag;                // log progress
  };

// Disc-at-once recording context block.

struct DAOCB {
  DiscLayout *disclayoutP;      // pointer to disc layout block
  FILECB *filecb_vec;           // pointer to FILECB vector
  UWORD file_count;             // # of files
  UWORD reserved;
  RECORDOPTIONS recordoptions;  // recording options
  };

// Track-at-once recording context block.

struct TAOCB {
  TRACKINFO trackinfo;          // TRACKINFO block
  FILECB filecb;                // FILE context block
  RECORDOPTIONS recordoptions;  // recording options
  };

// Disc status information.

struct DISCSTATUS {
  int disc_type;
  int write_status;
  int last_session_status;
  };

//**********************************************************************
//
// CDROM Writer class
//
//**********************************************************************

// Symbol classes for factory functions.

class CDWriterAsusCRW3212 {};
class CDWriterAsusCRW4012 {};
class CDWriterAsusCRW4816 {};
class CDWriterAsusCRW5224 {};
class CDWriterBenqDW400 {};
class CDWriterBenqDW800 {};
class CDWriterBenqDW822 {};
class CDWriterBenqDW830 {};
class CDWriterBenqDW1600 {};
class CDWriterBenqDW1620 {};
class CDWriterBenqDW1640 {};
class CDWriterGeneric8X8X24 {};
class CDWriterGeneric12X10X24 {};
class CDWriterGeneric16X10X24 {};
class CDWriterGeneric20X10X24 {};
class CDWriterGeneric20X10X40 {};
class CDWriterGeneric24X10X24 {};
class CDWriterGeneric24X10X40 {};
class CDWriterGeneric24X12X40 {};
class CDWriterGeneric32X10X40 {};
class CDWriterGeneric32X12X40 {};
class CDWriterGeneric40X12X48 {};
class CDWriterGeneric40X16X48 {};
class CDWriterGeneric48X16X48 {};
class CDWriterGeneric52X24X52 {};
class CDWriterGeneric52X32X52 {};
class CDWriterHP6020 {};
class CDWriterJVC2001 {};
class CDWriterJVC2010 {};
class CDWriterJVC2020 {};
class CDWriterJVC2040 {};
class CDWriterJVC2080 {};
class CDWriterJVC4080 {};
class CDWriterKodakPCD600 {};
class CDWriterLGE4X2X24 {};
class CDWriterLGE4X4X24 {};
class CDWriterLGE4X4X32 {};
class CDWriterLGE8X4X32 {};
class CDWriterLGE12X8X32 {};
class CDWriterLGE16X10X40 {};
class CDWriterLGE24X10X40 {};
class CDWriterLGE32X10X40 {};
class CDWriterLGE40X12X40 {};
class CDWriterLGE48X16X48 {};
class CDWriterLGE48X24X48 {};
class CDWriterLGE52X24X52 {};
class CDWriterLGEGMA4020 {};
class CDWriterLGEGSA4040 {};
class CDWriterLGEGSA4081 {};
class CDWriterLGEGSA4082 {};
class CDWriterLGEGSA4120 {};
class CDWriterLGEGSA4160 {};
class CDWriterMatshitaCW7501 {};
class CDWriterMatshitaCW7502 {};
class CDWriterMatshitaCW7503 {};
class CDWriterMatshitaCW7582 {};
class CDWriterMatshitaCW7585 {};
class CDWriterMatshitaCW8572 {};
class CDWriterMitsumiCR4801TE {};
class CDWriterMitsumiCR4802TE {};
class CDWriterMitsumiCR4804TE {};
class CDWriterMitsumiCR4805TE {};
class CDWriterMitsumiCR4808TE {};
class CDWriterMitsumiCR4809TE {};
class CDWriterMitsumiCR48XATE {};
class CDWriterMitsumiCR48XCTE {};
class CDWriterMitsumiCR48XFTE {};
class CDWriterMitsumiCR487ETE {};
class CDWriterNecND1000 {};
class CDWriterNecND1100 {};
class CDWriterNecND1300 {};
class CDWriterNecND2100 {};
class CDWriterNecND2500 {};
class CDWriterNecND3500 {};
class CDWriterNecND3520 {};
class CDWriterNecND3530 {};
class CDWriterNecND4550 {};
class CDWriterPhilipsCDD521 {};
class CDWriterPhilipsCDD522 {};
class CDWriterPhilipsCDD2000 {};
class CDWriterPhilipsCDD2600 {};
class CDWriterPhilipsCDD3600 {};
class CDWriterPhilipsCDD3800 {};
class CDWriterPhilipsCDD4200 {};
class CDWriterPhilipsCDD4400 {};
class CDWriterPhilipsCDD4600 {};
class CDWriterPhilipsCDD4800 {};
class CDWriterPhilipsCDRW1004 {};
class CDWriterPhilipsCDRW1208 {};
class CDWriterPhilipsCDRW1610 {};
class CDWriterPhilipsCDRW2010 {};
class CDWriterPhilipsCDRW2410 {};
class CDWriterPhilipsCDRW3210 {};
class CDWriterPhilipsCDRW4012 {};
class CDWriterPhilipsCDRW4824 {};
class CDWriterPhilipsCDRW5224 {};
class CDWriterPioneerDWS114X {};
class CDWriterPioneerDVR103 {};
class CDWriterPioneerDVR104 {};
class CDWriterPioneerDVR105 {};
class CDWriterPioneerDVR106 {};
class CDWriterPioneerDVR107 {};
class CDWriterPioneerDVR108 {};
class CDWriterPioneerDVR109 {};
class CDWriterPioneerDVR110 {};
class CDWriterPioneerDVR111 {};
class CDWriterPlextorPXR412 {};
class CDWriterPlextorPXR820 {};
class CDWriterPlextorPXW4220 {};
class CDWriterPlextorPXW8220 {};
class CDWriterPlextorPXW8432 {};
class CDWriterPlextorPXW124 {};
class CDWriterPlextorPXW1210 {};
class CDWriterPlextorPXW1610 {};
class CDWriterPlextorPXW2410 {};
class CDWriterPlextorPXW4012 {};
class CDWriterPlextorPXW4824 {};
class CDWriterPlextorPXW5224 {};
class CDWriterPlextorPXW5232 {};
class CDWriterPlextorPXS88 {};
class CDWriterPlextorPX320 {};
class CDWriterPlextorPX708 {};
class CDWriterPlextorPX712 {};
class CDWriterPlextorPX716 {};
class CDWriterPlextorPX750 {};
class CDWriterPlextorPX755 {};
class CDWriterPlextorPX760 {};
class CDWriterPlextorPXB900 {};
class CDWriterQsiSBW241 {};
class CDWriterQsiDBW481 {};
class CDWriterQsiDBW521 {};
class CDWriterQsiSDW041 {};
class CDWriterQsiDDW041 {};
class CDWriterQsiDDW061 {};
class CDWriterQsiDDW082 {};
class CDWriterRicohRS1420C {};
class CDWriterRicohMP6200S {};
class CDWriterRicohMP6200A {};
class CDWriterRicohMP7040 {};
class CDWriterRicohMP7060 {};
class CDWriterRicohMP7080 {};
class CDWriterRicohMP7120 {};
class CDWriterRicohMP7125 {};
class CDWriterRicohMP7163 {};
class CDWriterRicohMP7200 {};
class CDWriterRicohMP7240 {};
class CDWriterRicohMP7320 {};
class CDWriterRicohMP7400 {};
class CDWriterRicohMP7480 {};
class CDWriterRicohMP7520 {};
class CDWriterRicohMP8040 {};
class CDWriterRicohMP8080 {};
class CDWriterRicohMP5120 {};
class CDWriterRicohMP5125 {};
class CDWriterRicohMP5163 {};
class CDWriterRicohMP5240 {};
class CDWriterRicohMP8160 {};
class CDWriterSamsungSW208B {};
class CDWriterSamsungSW212B {};
class CDWriterSanyoCDR8020 {};
class CDWriterSanyoCDRW8220 {};
class CDWriterSanyoCDRW8432 {};
class CDWriterSanyoCDRW12432 {};
class CDWriterSanyoCDRW121032 {};
class CDWriterSanyoCDRW161040 {};
class CDWriterSanyoCDRW201040 {};
class CDWriterSanyoCDRW241040 {};
class CDWriterSanyoCDRW321040 {};
class CDWriterSanyoCDRW401248 {};
class CDWriterSanyoCDRW481648 {};
class CDWriterSanyoCDRW522452 {};
class CDWriterSanyoCRDBPDV2 {};
class CDWriterSanyoCRDBPDV3 {};
class CDWriterSonyCDW900E {};
class CDWriterSonyCDU920S {};
class CDWriterSonyCDU924S {};
class CDWriterSonyCDU926S {};
class CDWriterSonyCDU948S {};
class CDWriterSonyCRX100 {};
class CDWriterSonyCRX110 {};
class CDWriterSonyCRX120_2X {};
class CDWriterSonyCRX120_4X {};
class CDWriterSonyCRX130 {};
class CDWriterSonyCRX140 {};
class CDWriterSonyCRX145 {};
class CDWriterSonyCRX160 {};
class CDWriterSonyCRX168 {};
class CDWriterSonyCRX85 {};
class CDWriterSonyCRX175 {};
class CDWriterSonyCRX185 {};
class CDWriterSonyCRX195 {};
class CDWriterSonyCRX225 {};
class CDWriterSonyCRX230 {};
class CDWriterSonyCRX300 {};
class CDWriterSonyDRU500 {};
class CDWriterSonyDRU510 {};
class CDWriterSonyDRU530 {};
class CDWriterSonyDRU540 {};
class CDWriterSonyDRU710 {};
class CDWriterSonyDRU720 {};
class CDWriterSonyDRU800 {};
class CDWriterSonyDWU50 {};
class CDWriterSonyDWU54 {};
class CDWriterSonyDWU55 {};
class CDWriterSonyDWD56 {};
class CDWriterTeacCDR50S {};
class CDWriterTeacCDR55S {};
class CDWriterTeacCDR56S {};
class CDWriterTeacCDR56S4 {};
class CDWriterTeacCDR58S {};
class CDWriterTeacCDW54 {};
class CDWriterTeacCDW58 {};
class CDWriterTeacCDW512 {};
class CDWriterTeacCDW516 {};
class CDWriterTeacCDW524 {};
class CDWriterTeacCDW540 {};
class CDWriterTeacCDW548 {};
class CDWriterTeacCDW552 {};
class CDWriterToshibaSDR1002 {};
class CDWriterToshibaSDR1102 {};
class CDWriterToshibaSDR1202 {};
class CDWriterToshibaSDR5002 {};
class CDWriterToshibaSDR5112 {};
class CDWriterToshibaSDR5272 {};
class CDWriterToshibaSDR5372 {};
class CDWriterToshibaSDR6012 {};
class CDWriterToshibaSDR6112 {};
class CDWriterYamahaCDR100 {};
class CDWriterYamahaCDR102 {};
class CDWriterYamahaCDR200 {};
class CDWriterYamahaCDR400 {};
class CDWriterYamahaCRW2260 {};
class CDWriterYamahaCRW2216 {};
class CDWriterYamahaCRW4260 {};
class CDWriterYamahaCRW4416 {};
class CDWriterYamahaCRW6416 {};
class CDWriterYamahaCRW8424 {};
class CDWriterYamahaCRW8824 {};
class CDWriterYamahaCRW2100 {};
class CDWriterYamahaCRW2200 {};
class CDWriterYamahaCRW3200 {};
class CDWriterYamahaCRWF1 {};
class CDWriterYamahaCRW70 {};

class CDWriter : public CDReader {
  public:
    CDWriter(ADCB *adcbP) : CDReader(adcbP) {}

    // Factory functions
    static CDWriter *make(CDWriterGeneric8X8X24,    ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric12X10X24,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric16X10X24,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric20X10X24,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric20X10X40,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric24X10X24,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric24X10X40,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric24X12X40,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric32X10X40,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric32X12X40,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric40X12X48,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric40X16X48,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric48X16X48,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric52X24X52,  ADCB *adcbP);
    static CDWriter *make(CDWriterGeneric52X32X52,  ADCB *adcbP);
    static CDWriter *make(CDWriterAsusCRW3212,      ADCB *adcbP);
    static CDWriter *make(CDWriterAsusCRW4012,      ADCB *adcbP);
    static CDWriter *make(CDWriterAsusCRW4816,      ADCB *adcbP);
    static CDWriter *make(CDWriterAsusCRW5224,      ADCB *adcbP);
    static CDWriter *make(CDWriterBenqDW400,        ADCB *adcbP);
    static CDWriter *make(CDWriterBenqDW800,        ADCB *adcbP);
    static CDWriter *make(CDWriterBenqDW822,        ADCB *adcbP);
    static CDWriter *make(CDWriterBenqDW830,        ADCB *adcbP);
    static CDWriter *make(CDWriterBenqDW1600,       ADCB *adcbP);
    static CDWriter *make(CDWriterBenqDW1620,       ADCB *adcbP);
    static CDWriter *make(CDWriterBenqDW1640,       ADCB *adcbP);
    static CDWriter *make(CDWriterHP6020,           ADCB *adcbP);
    static CDWriter *make(CDWriterJVC2001,          ADCB *adcbP);
    static CDWriter *make(CDWriterJVC2010,          ADCB *adcbP);
    static CDWriter *make(CDWriterJVC2020,          ADCB *adcbP);
    static CDWriter *make(CDWriterJVC2040,          ADCB *adcbP);
    static CDWriter *make(CDWriterJVC2080,          ADCB *adcbP);
    static CDWriter *make(CDWriterJVC4080,          ADCB *adcbP);
    static CDWriter *make(CDWriterKodakPCD600,      ADCB *adcbP);
    static CDWriter *make(CDWriterLGE4X2X24,        ADCB *adcbP);
    static CDWriter *make(CDWriterLGE4X4X24,        ADCB *adcbP);
    static CDWriter *make(CDWriterLGE4X4X32,        ADCB *adcbP);
    static CDWriter *make(CDWriterLGE8X4X32,        ADCB *adcbP);
    static CDWriter *make(CDWriterLGE12X8X32,       ADCB *adcbP);
    static CDWriter *make(CDWriterLGE16X10X40,      ADCB *adcbP);
    static CDWriter *make(CDWriterLGE24X10X40,      ADCB *adcbP);
    static CDWriter *make(CDWriterLGE32X10X40,      ADCB *adcbP);
    static CDWriter *make(CDWriterLGE40X12X40,      ADCB *adcbP);
    static CDWriter *make(CDWriterLGE48X16X48,      ADCB *adcbP);
    static CDWriter *make(CDWriterLGE48X24X48,      ADCB *adcbP);
    static CDWriter *make(CDWriterLGE52X24X52,      ADCB *adcbP);
    static CDWriter *make(CDWriterLGEGMA4020,       ADCB *adcbP);
    static CDWriter *make(CDWriterLGEGSA4040,       ADCB *adcbP);
    static CDWriter *make(CDWriterLGEGSA4081,       ADCB *adcbP);
    static CDWriter *make(CDWriterLGEGSA4082,       ADCB *adcbP);
    static CDWriter *make(CDWriterLGEGSA4120,       ADCB *adcbP);
    static CDWriter *make(CDWriterLGEGSA4160,       ADCB *adcbP);
    static CDWriter *make(CDWriterMatshitaCW7501,   ADCB *adcbP);
    static CDWriter *make(CDWriterMatshitaCW7502,   ADCB *adcbP);
    static CDWriter *make(CDWriterMatshitaCW7503,   ADCB *adcbP);
    static CDWriter *make(CDWriterMatshitaCW7582,   ADCB *adcbP);
    static CDWriter *make(CDWriterMatshitaCW7585,   ADCB *adcbP);
    static CDWriter *make(CDWriterMatshitaCW8572,   ADCB *adcbP);
    static CDWriter *make(CDWriterMitsumiCR4801TE,  ADCB *adcbP);
    static CDWriter *make(CDWriterMitsumiCR4802TE,  ADCB *adcbP);
    static CDWriter *make(CDWriterMitsumiCR4804TE,  ADCB *adcbP);
    static CDWriter *make(CDWriterMitsumiCR4805TE,  ADCB *adcbP);
    static CDWriter *make(CDWriterMitsumiCR4808TE,  ADCB *adcbP);
    static CDWriter *make(CDWriterMitsumiCR4809TE,  ADCB *adcbP);
    static CDWriter *make(CDWriterMitsumiCR48XATE,  ADCB *adcbP);
    static CDWriter *make(CDWriterMitsumiCR48XCTE,  ADCB *adcbP);
    static CDWriter *make(CDWriterMitsumiCR48XFTE,  ADCB *adcbP);
    static CDWriter *make(CDWriterMitsumiCR487ETE,  ADCB *adcbP);
    static CDWriter *make(CDWriterNecND1000,        ADCB *adcbP);
    static CDWriter *make(CDWriterNecND1100,        ADCB *adcbP);
    static CDWriter *make(CDWriterNecND1300,        ADCB *adcbP);
    static CDWriter *make(CDWriterNecND2100,        ADCB *adcbP);
    static CDWriter *make(CDWriterNecND2500,        ADCB *adcbP);
    static CDWriter *make(CDWriterNecND3500,        ADCB *adcbP);
    static CDWriter *make(CDWriterNecND3520,        ADCB *adcbP);
    static CDWriter *make(CDWriterNecND3530,        ADCB *adcbP);
    static CDWriter *make(CDWriterNecND4550,        ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDD521,    ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDD522,    ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDD2000,   ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDD2600,   ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDD3600,   ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDD3800,   ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDD4200,   ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDD4400,   ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDD4600,   ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDD4800,   ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDRW1004,  ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDRW1208,  ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDRW1610,  ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDRW2010,  ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDRW2410,  ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDRW3210,  ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDRW4012,  ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDRW4824,  ADCB *adcbP);
    static CDWriter *make(CDWriterPhilipsCDRW5224,  ADCB *adcbP);
    static CDWriter *make(CDWriterPioneerDWS114X,   ADCB *adcbP);
    static CDWriter *make(CDWriterPioneerDVR103,    ADCB *adcbP);
    static CDWriter *make(CDWriterPioneerDVR104,    ADCB *adcbP);
    static CDWriter *make(CDWriterPioneerDVR105,    ADCB *adcbP);
    static CDWriter *make(CDWriterPioneerDVR106,    ADCB *adcbP);
    static CDWriter *make(CDWriterPioneerDVR107,    ADCB *adcbP);
    static CDWriter *make(CDWriterPioneerDVR108,    ADCB *adcbP);
    static CDWriter *make(CDWriterPioneerDVR109,    ADCB *adcbP);
    static CDWriter *make(CDWriterPioneerDVR110,    ADCB *adcbP);
    static CDWriter *make(CDWriterPioneerDVR111,    ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXR412,    ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXR820,    ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW4220,   ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW8220,   ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW8432,   ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW124,    ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW1210,   ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW1610,   ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW2410,   ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW4012,   ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW4824,   ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW5224,   ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXW5232,   ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXS88,     ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPX320,     ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPX708,     ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPX712,     ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPX716,     ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPX750,     ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPX755,     ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPX760,     ADCB *adcbP);
    static CDWriter *make(CDWriterPlextorPXB900,    ADCB *adcbP);
    static CDWriter *make(CDWriterQsiSBW241,        ADCB *adcbP);
    static CDWriter *make(CDWriterQsiDBW481,        ADCB *adcbP);
    static CDWriter *make(CDWriterQsiDBW521,        ADCB *adcbP);
    static CDWriter *make(CDWriterQsiSDW041,        ADCB *adcbP);
    static CDWriter *make(CDWriterQsiDDW041,        ADCB *adcbP);
    static CDWriter *make(CDWriterQsiDDW061,        ADCB *adcbP);
    static CDWriter *make(CDWriterQsiDDW082,        ADCB *adcbP);
    static CDWriter *make(CDWriterRicohRS1420C,     ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP6200S,     ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP6200A,     ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7040,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7060,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7080,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7120,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7125,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7163,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7200,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7240,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7320,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7400,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7480,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP7520,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP8040,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP8080,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP5120,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP5125,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP5163,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP5240,      ADCB *adcbP);
    static CDWriter *make(CDWriterRicohMP8160,      ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDR8020,     ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW8220,    ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW8432,    ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW12432,   ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW121032,  ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW161040,  ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW201040,  ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW241040,  ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW321040,  ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW401248,  ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW481648,  ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCDRW522452,  ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCRDBPDV2,    ADCB *adcbP);
    static CDWriter *make(CDWriterSanyoCRDBPDV3,    ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCDW900E,      ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCDU920S,      ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCDU924S,      ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCDU926S,      ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCDU948S,      ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX100,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX110,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX120_2X,    ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX120_4X,    ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX130,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX140,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX145,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX160,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX168,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX85,        ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX175,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX185,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX195,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX225,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX230,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyCRX300,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDRU500,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDRU510,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDRU530,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDRU540,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDRU710,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDRU720,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDRU800,       ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDWU50,        ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDWU54,        ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDWU55,        ADCB *adcbP);
    static CDWriter *make(CDWriterSonyDWD56,        ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDR50S,       ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDR55S,       ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDR56S,       ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDR56S4,      ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDR58S,       ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDW54,        ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDW58,        ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDW512,       ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDW516,       ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDW524,       ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDW540,       ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDW548,       ADCB *adcbP);
    static CDWriter *make(CDWriterTeacCDW552,       ADCB *adcbP);
    static CDWriter *make(CDWriterToshibaSDR1002,   ADCB *adcbP);
    static CDWriter *make(CDWriterToshibaSDR1102,   ADCB *adcbP);
    static CDWriter *make(CDWriterToshibaSDR1202,   ADCB *adcbP);
    static CDWriter *make(CDWriterToshibaSDR5002,   ADCB *adcbP);
    static CDWriter *make(CDWriterToshibaSDR5112,   ADCB *adcbP);
    static CDWriter *make(CDWriterToshibaSDR5272,   ADCB *adcbP);
    static CDWriter *make(CDWriterToshibaSDR5372,   ADCB *adcbP);
    static CDWriter *make(CDWriterToshibaSDR6012,   ADCB *adcbP);
    static CDWriter *make(CDWriterToshibaSDR6112,   ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCDR100,     ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCDR102,     ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCDR200,     ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCDR400,     ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW2260,    ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW2216,    ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW4260,    ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW4416,    ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW6416,    ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW8424,    ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW8824,    ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW2100,    ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW2200,    ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW3200,    ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRWF1,      ADCB *adcbP);
    static CDWriter *make(CDWriterYamahaCRW70,      ADCB *adcbP);

    // Get a list of installed CD writer devices.
    static int GetDeviceObjects(CDWriter **device_list, int list_size);

    // Jacket routines for the virtual functions
    int CloseSession(
      UBYTE datatype, BOOL multisession_flag, BOOL log_flag, BOOL sig_flag);
    int EraseDisc(BOOL full_flag, BOOL log_flag, BOOL sig_flag);
    int FormatDisc(ULONG blocks, ULONG packet_size, BOOL log_flag, BOOL sig_flag);
    int GetCacheFreespace(ULONG *blocksP, BOOL sig_flag);
    int GetNextWritableAddress(UBYTE mode, SLONG *lbaP, BOOL sig_flag);
    int SetTrackBlockLength(UBYTE datatype, UWORD blklen, BOOL sig_flag);
    int SetWriteParameters(UBYTE speed, BOOL test_flag, BOOL sig_flag);
    int WriteDisc(void *bufP, UWORD blklen, UWORD blocks, SLONG lba, BOOL sig_flag);
    int StartDiscAtOnce(DAOCB *daocbP, BOOL sig_flag);
    int FinishDiscAtOnce(DAOCB *daocbP, BOOL sig_flag);
    int StartTrackAtOnce(TAOCB *taocbP, BOOL sig_flag);
    int FinishTrackAtOnce(TAOCB *taocbP, BOOL sig_flag);
    BOOL GetDiscCurrentCapacity(ULONG *blocksP, BOOL sig_flag);
    BOOL GetDiscFreespace(ULONG *blocksP, BOOL sig_flag);
    BOOL GetDiscMaximumCapacity(ULONG *blocksP, BOOL sig_flag);
    BOOL GetDiscStatus(DISCSTATUS *discstatusP, BOOL sig_flag);
    BOOL GetDiscType(int *disctypeP, BOOL sig_flag);
    BOOL GetDiscWriteStatus(int *write_statusP, BOOL sig_flag);

    // Pure virtual functions
    virtual void AbortDiscAtOnce() = 0;
    virtual void AbortTrackAtOnce() = 0;
    virtual int CloseSession(UBYTE datatype, BOOL multisession_flag) = 0;
    virtual int EraseDisc(BOOL full_flag) = 0;
    virtual int FormatDisc(ULONG blocks, ULONG packet_size) = 0;
    virtual int GetCacheFreespace(ULONG *blocksP) = 0;
    virtual int GetNextWritableAddress(UBYTE mode, SLONG *lbaP) = 0;
    virtual int StartDiscAtOnce(DAOCB *daocbP) = 0;
    virtual int FinishDiscAtOnce(DAOCB *daocbP) = 0;
    virtual int StartTrackAtOnce(TAOCB *taocbP) = 0;
    virtual int FinishTrackAtOnce(TAOCB *taocbP) = 0;
    virtual int SetTrackBlockLength(UBYTE datatype, UWORD blklen) = 0;
    virtual int SetWriteParameters(UBYTE speed, BOOL test_flag) = 0;
    virtual int WriteDisc(void *bufP, UWORD blklen, UWORD blocks, SLONG lba) = 0;

    // Virtual functions
    virtual void PreProcessWriteBuffer(void *bufP, UWORD blklen, UWORD blocks);
    virtual int ReadATIP(ATIPTOC *atiptocP);
    virtual BOOL GetDiscCurrentCapacity(ULONG *blocksP);
    virtual BOOL GetDiscMaximumCapacity(ULONG *blocksP);
    virtual BOOL GetDiscFreespace(ULONG *blocksP);
    virtual BOOL GetDiscManufacturerCode(MSF *codeP);
    virtual BOOL GetDiscStatus(DISCSTATUS *discstatusP);
    virtual BOOL GetDiscType(int *disctypeP);
    virtual BOOL GetDiscWriteStatus(int *write_statusP);

    // High level functions
    void ExtractSectorsToRecorder (
      CDReader *cdreaderP, UBYTE datatype, UWORD blklen, SLONG lba, ULONG blkcnt,
      UBYTE jitter_mode, UBYTE retry_count, UBYTE error_mode, BOOL log_flag);

    void RecordDirectoryPaths(
      DIRPATHDESC *pathdesc_vec, int desc_count, const TCHAR *image_filnamP,
      CDFSOPTIONS *cdfs_optionsP, RECORDOPTIONS *recordoptP, BOOL import_flag);

    void RecordDirectoryTree(
      DirectoryTree *dirtreeP, CDFSOPTIONS *cdfs_optionsP, RECORDOPTIONS *recordoptP);

    void RecordImageFile(
      const TCHAR *filnamP, RECORDOPTIONS *recordoptP);

    void RecordDiscAtOnce(const TCHAR *cue_filnamP, RECORDOPTIONS *recordoptP);
    void RecordDiscAtOnce(CueSheet *cuesheetP, RECORDOPTIONS *recordoptP);
    void RecordDiscAtOnce(DAOCB *daocbP);

    void RecordTrackAtOnce(
      const TCHAR *filnamP, UBYTE filetype, UBYTE track_datatype,
      UWORD track_blklen, RECORDOPTIONS *recordoptP);
    void RecordTrackAtOnce(TAOCB *taocbP);

    void RecordGap(UBYTE gaptype, UBYTE datatype, UWORD blklen, ULONG blkcnt, BOOL log_flag);

    void VerifyDisc(const TCHAR *image_filnamP, SLONG start_lba, BOOL mode2_flag, BOOL log_flag);
    void VerifyDisc(DirectoryTree *dirtreeP, BOOL mode2_flag, BOOL log_flag);

    // Flags
    enum CDWriterFlag {
      cwfTrackAtOnce        = 0x0001, // supports track-at-once recording
      cwfDiscAtOnce         = 0x0002, // supports disc-at-once recording
      cwfDiscAtOnceRaw      = 0x0004, // supports raw disc-at-once recording
      cwfDiscAtOnceRawOnly  = 0x0008, // supports only raw disc-at-once recording
      cwfMultiSession       = 0x0010, // supports TAO after DAO
      cwfNoDirectCopy       = 0x0020, // does not support direct CD-to-CD copy
      cwfWriteCDG           = 0x0040, // supports writing of CD+G subcodes
      cwfWriteCDTEXT        = 0x0080, // supports writing of CDTEXT
      cwfUnderrunProtect    = 0x0100, // supports buffer underrun protection
      cwfMultiWriter        = 0x0200  // supported by (our) multi-writer feature
      };

    // Get functions
    ULONG GetWriteFlags()         {return WriteFlags;}
    SLONG GetNextLBA()            {return NextLBA;}
    SLONG GetLastLBA()            {return LastLBA;}
    SLONG GetTrackLBA()           {return TrackLBA;}
    ULONG GetWriteDiscTypeMask()  {return WriteDiscTypeMask;}
    ULONG GetWriteSpeedMask()     {return WriteSpeedMask;}
    ULONG GetWriteSpeedMask(int disctype)
      {return WriteSpeedMasks[disctype];}
    UBYTE GetMinWriteSpeed()      {return MinWriteSpeed;}
    UBYTE GetMaxWriteSpeed()      {return MaxWriteSpeed;}
    UBYTE GetTestMode()           {return TestMode;}
    UBYTE GetCurrentWriteMode()   {return CurrentWriteMode;}
    UBYTE GetCurrentDiscType()    {return CurrentDiscType;}

    // Set functions
    void SetNextLBA(SLONG lba)    {NextLBA  = lba;}
    void SetLastLBA(SLONG lba)    {LastLBA  = lba;}
    void SetTrackLBA(SLONG lba)   {TrackLBA = lba;}

    // Flag functions
    void SetFlag(CDWriterFlag mask) {WriteFlags |= (UWORD)(mask);}
    void ClearFlag(CDWriterFlag mask) {WriteFlags &= ~(UWORD)(mask);}
    BOOL IsFlagSet(CDWriterFlag mask)
      {return ((WriteFlags & (ULONG)(mask)) ? TRUE : FALSE);}
    BOOL IsFlagClear(CDWriterFlag mask)
      {return ((WriteFlags & (ULONG)(mask)) ? FALSE : TRUE);}

  private:
    void RecordFileDAO(FILECB *filecbP, CUEPOINT *cuepoint_vec, BOOL log_flag);
    void RecordFileTAO(FILECB *filecbP, BOOL log_flag);
    void VerifyDirectoryTreeRecurse(DIRNODE *dirnodeP, BOOL log_flag);
    void VerifyOneFile(FILENODE *filenodeP, BOOL log_flag);

  protected:
    SLONG NextLBA;              // Next LBA to be written
    SLONG LastLBA;              // Last LBA to be written
    SLONG TrackLBA;             // First track LBA to be written

    ULONG WriteFlags;           // Misc flags
    ULONG WriteDiscTypeMask;    // Supported disc types
    ULONG WriteSpeedMasks[32];  // Supported writing speeds for each disc type

    UBYTE TestMode;             // Test writing mode?
    UBYTE CurrentWriteMode;     // Current writing mode
    UBYTE CurrentDiscType;      // Current disc type being recorded
    UBYTE Reserved1;

    // Obsolete fields that only apply to CD-R and CD-RW discs.
    ULONG WriteSpeedMask;       // Writing speeds
    UBYTE MinWriteSpeed;        // Minimum writing speed
    UBYTE MaxWriteSpeed;        // Maximum writing speed
    UWORD Reserved2;

  private:
    CDWriter(const CDWriter&);
    CDWriter& operator =(const CDWriter&);
  };

//
// CD-R output buffer
//

class CDWriteBuffer {
  public:
    CDWriteBuffer(UWORD blklen, CDWriter *cdwriterP);
    void WriteBuffer(void *bufP, UWORD buflen);
    void FlushBuffer();
  protected:
    CDWriter *pWriter;
    UWORD nBlockLength;
    UWORD nMaxBufferLength;
    UWORD nBlocksPerIO;
    UWORD nBufferLength;
    UBYTE Buffer[65000];
  };

// Restore previous packing alignment.

#pragma pack(pop)

#endif
