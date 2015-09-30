#if !defined(CDRLIB_ELTORITO_H)
#define CDRLIB_ELTORITO_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

#define ELTORITO_NON_BOOTABLE       0x00
#define ELTORITO_BOOTABLE           0x88

#define ELTORITO_BOOTSYSTEM_ID      "EL TORITO SPECIFICATION"

#define ELTORITO_PLATFORM_80X86     0x00
#define ELTORITO_PLATFORM_POWERPC   0x01
#define ELTORITO_PLATFORM_MAC       0x02

#define ELTORITO_DEF_LOAD_SEGMENT   0x07C0

// Boot Catalog - Validation Entry

struct BOOTCATALOG_VE {
  UBYTE header;           // must be 01h
  UBYTE platform;
  UWORD mbz;
  char developer[24];
  UWORD checksum;
  UBYTE keybyte1;         // must be 55h
  UBYTE keybyte2;         // must be AAh
  };

// Boot Catalog - Initial/Default Entry

struct BOOTCATALOG_IDE {
  UBYTE boot_indicator;
  UBYTE media_type;
  UWORD load_segment;
  UBYTE system_type;
  UBYTE mbz1;
  UWORD load_sector_count;
  SLONG load_lba;
  UBYTE mbz2[20];
  };

// Boot Catalog - Section Header Entry

struct BOOTCATALOG_SHE {
  UBYTE mbz[32];
  };

// Boot Catalog - Section Entry

struct BOOTCATALOG_SE {
  UBYTE mbz[32];
  };

// Boot Catalog - Section Entry Extension

struct BOOTCATALOG_SEE {
  UBYTE mbz[32];
  };

// Boot Catalog Entry

struct BOOTCATALOG {
  union {
    struct BOOTCATALOG_VE ve;
    struct BOOTCATALOG_IDE ide;
    struct BOOTCATALOG_SHE she;
    struct BOOTCATALOG_SE se;
    struct BOOTCATALOG_SEE see;
    };
  };

// Restore previous packing alignment.

#pragma pack(pop)

#endif
