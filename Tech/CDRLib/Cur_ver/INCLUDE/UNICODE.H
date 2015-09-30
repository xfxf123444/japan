#if !defined(CDRLIB_UNICODE_H)
#define CDRLIB_UNICODE_H

#if defined(WIN32)
#include <mbstring.h>
#endif

#if defined(WIN32) && defined(_UNICODE)

#define StrCpy      wcscpy
#define StrCat      wcscat
#define StrLen      wcslen
#define StrSpn      wcsspn
#define StrUpr      _wcsupr
#define StrPrintf   swprintf
#define StrScanf    swscanf
#define SplitPath   _wsplitpath
#define MakePath    _wmakepath
#define FOpen       _wfopen
#define FPrintf     fwprintf
#define FGets       fgetws
#define ToUpper     towupper
#define IntToStr    _itow

#define StrCmp      wcscmp
#define StrICmp     _wcsicmp
#define StrNICmp    _wcsnicmp
#define StrChr      wcschr
#define StrRChr     wcsrchr
#define StrNextChr  _wcsnextc
#define StrInc      _wcsinc
#define StrDec      _wcsdec
#define ChrLen(x)   1

#else

#define StrCpy      strcpy
#define StrCat      strcat
#define StrLen      strlen
#define StrSpn      strspn
#define StrUpr      strupr
#define StrPrintf   sprintf
#define StrScanf    sscanf
#define SplitPath   _splitpath
#define MakePath    _makepath
#define FOpen       fopen
#define FPrintf     fprintf
#define FGets       fgets
#define ToUpper     toupper
#define IntToStr    itoa

#if defined(WIN32)
#define StrCmp(x1,x2)       _mbscmp((const unsigned char *)(x1),(const unsigned char *)(x2))
#define StrICmp(x1,x2)      _mbsicmp((const unsigned char *)(x1),(const unsigned char *)(x2))
#define StrNICmp(x1,x2,x3)  _mbsnicmp((const unsigned char *)(x1),(const unsigned char *)(x2), x3)
#define StrChr(x1,x2)       (TCHAR *)_mbschr((const unsigned char *)(x1),x2)
#define StrRChr(x1,x2)      (TCHAR *)_mbsrchr((unsigned char *)(x1),x2)
#define StrNextChr(x)       (unsigned int)_mbsnextc((const unsigned char *)(x))
#define StrInc(x)           (TCHAR *)_mbsinc((const unsigned char *)(x))
#define StrDec(x1,x2)       (TCHAR *)_mbsdec((const unsigned char *)(x1), (const unsigned char *)(x2))
#define ChrLen(x)           _mbclen((const unsigned char *)x);
#else
#define StrCmp              strcmp
#define StrICmp             stricmp
#define StrNICmp            strnicmp
#define StrChr              strchr
#define StrRChr             strrchr
#define StrNextChr(x)       (*(unsigned char *)(x))
#define StrInc(x)           ((char *)(x) + 1)
#define StrDec(x1,x2)       ((x2 > x1) ? ((char *)(x2) - 1) : NULL)
#define ChrLen(x)           1
#endif

#endif

inline unsigned int CharLength(const WCHAR *curP)
  {return 1;}

inline WCHAR StringNextChar(const WCHAR *curP)
  {return *curP;}

inline WCHAR *StringIncrement(const WCHAR *curP)
  {return ((WCHAR *)curP + 1);}

inline WCHAR *StringDecrement(const WCHAR *stringP, const WCHAR *curP)
  {return ((curP > stringP) ? (WCHAR *)curP - 1 : NULL);}

inline unsigned int CharLength(const char *curP)
{
  #if defined(WIN32)
    return _mbclen((const unsigned char *)curP);
  #else
    return 1;
  #endif
}

inline unsigned int StringNextChar(const char *curP)
{
  #if defined(WIN32)
    return _mbsnextc((const unsigned char *)curP);
  #else
    return *(const unsigned char *)curP;
  #endif
}

inline char *StringIncrement(const char *curP)
{
  #if defined(WIN32)
    return (char *)_mbsinc((const unsigned char *)(curP));
  #else
    return ((char *)curP + 1);
  #endif
}

inline char *StringDecrement(const char *stringP, const char *curP)
{
  #if defined(WIN32)
    return (char *)_mbsdec((const unsigned char *)stringP, (const unsigned char *)(curP));
  #else
    return ((curP > stringP) ? (char *)curP - 1 : NULL);
  #endif
}

char *StringCopy(char *targetP, const char *sourceP, int source_len = -1);

char *StringCopy(
  char *targetP, int target_len, const char *sourceP, int source_len,
  char fill, BOOL terminator_flag);

char *StringCopy(
  char *targetP, const WCHAR *sourceP, int source_len = -1, BOOL big_endian_flag = FALSE);

char *StringCopy(
  char *targetP, int target_len, const WCHAR *sourceP, int source_len,
  WCHAR fill, BOOL big_endian_flag, BOOL terminator_flag);

WCHAR *StringCopy(
  WCHAR *targetP, const WCHAR *sourceP, int source_len = -1, BOOL big_endian_flag = FALSE);

WCHAR *StringCopy(
  WCHAR *targetP, int target_len, const WCHAR *sourceP, int source_len,
  WCHAR fill, BOOL big_endian_flag, BOOL terminator_flag);

WCHAR *StringCopy(
  WCHAR *targetP, const char *sourceP, int source_len = -1, BOOL big_endian_flag = FALSE);

WCHAR *StringCopy(
  WCHAR *targetP, int target_len, const char *sourceP, int source_len,
  WCHAR fill, BOOL big_endian_flag, BOOL terminator_flag);

int StringLength(const char *stringP);
int StringLength(const WCHAR *stringP);

int StringCharCount(const char *stringP);
int StringCharCount(const WCHAR *stringP);

void StringFill(char *targetP, int count, char fill);
void StringFill(WCHAR *targetP, int count, WCHAR fill);

void StringTrimLeading(char *stringP);
void StringTrimTrailing(char *stringP);
int StringTrimLeading(char *stringP, char c);
int StringTrimTrailing(char *stringP, char c);
void StringTrimGarbage(char *stringP);

void StringTrimLeading(WCHAR *stringP);
void StringTrimTrailing(WCHAR *stringP);
int StringTrimLeading(WCHAR *stringP, WCHAR c);
int StringTrimTrailing(WCHAR *stringP, WCHAR c);
void StringTrimGarbage(WCHAR *stringP);

int StringRemoveChar(char *stringP, char c, char *protectChP = NULL);
int StringRemoveChar(WCHAR *stringP, WCHAR c, WCHAR *protectChP = NULL);

void StringRemoveDuplicateSpaces(char *stringP);
void StringRemoveDuplicateSpaces(WCHAR *stringP);

BOOL StringTableLookup(const char *string, char *table[], BOOL case_sensitive = FALSE);
BOOL StringTableLookup(const WCHAR *string, WCHAR *table[], BOOL case_sensitive = FALSE);

BOOL StringContainsUnicode(const WCHAR *stringP);

void StringFlipUnicode(WCHAR *stringP, int string_len = -1);

void StringAnsiToOem(TCHAR *targetP, const TCHAR *sourceP);

int CharCountToLength (const char *stringP, int char_count);
int CharCountToLength (const WCHAR *stringP, int char_count);

char *HeapString(const char *stack_stringP);
WCHAR *HeapString(const WCHAR *stack_stringP);

TCHAR *CloneString(const TCHAR *stringP, BOOL clone_empty_string = FALSE);

BOOL LastCharacterMatches(const TCHAR *stringP, const TCHAR ch);

#endif
