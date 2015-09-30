#if !defined(CDRLIB_DATATYPE_H)
#define CDRLIB_DATATYPE_H

// Datatypes.

#define FALSE 0
#define TRUE  1

typedef signed char     SBYTE;
typedef unsigned char   UBYTE;

typedef signed short    SWORD;
typedef unsigned short  UWORD;

typedef signed long     SLONG;
typedef unsigned long   ULONG;

typedef int             BOOL;
typedef int             FLAGS;

typedef unsigned char   BFLAG;
typedef unsigned short  WFLAG;
typedef unsigned long   LFLAG;

typedef void            *FILEHANDLE;

struct UQUAD {
  ULONG low;
  ULONG high;
  };

struct SQUAD {
  SLONG low;
  SLONG high;
  };

#if !defined(WIN32)
typedef char            TCHAR;
typedef unsigned short  WCHAR;
#define _T(x) x
#endif

#endif
