//#ifndef _COMM_H
//#define _COMM_H

#define VOID			void
#ifndef FAR
#define FAR 			_far
#endif
#define NEAR			_near

/*=======================================================
 * Macros to make the programming like Win32
 *=====================================================*/
typedef  int               INT   ;
typedef  unsigned char     BYTE  ;
typedef  unsigned short    WORD  ;
typedef  unsigned long     DWORD ;
typedef  int               BOOL  ;
typedef  unsigned  int     UINT  ;
typedef  void *            PVOID ;
typedef  long              LONG  ;

#define  FALSE  0
#define  TRUE   1
#define  LOWORD(l)           ((WORD)(l))
#define  HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define  LOBYTE(w)           ((BYTE)(w))
#define  HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#define  MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define  MAKEDWORD(a, b)     ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))

#define  LINEARADDR(p)       (((((DWORD)(p))&0xffff0000)>>12)+ LOWORD((DWORD)(p))) 

typedef BYTE*				PBYTE;
typedef BYTE FAR*			LPBYTE;
typedef WORD FAR*			LPWORD;
typedef DWORD FAR*			LPDWORD;
typedef void FAR*			LPVOID;

#ifndef SECTOR_SIZE
#define SECTOR_SIZE			0x200
#endif
#ifndef MAX_RW_SECTORS
#define MAX_RW_SECTORS		8
#endif
#define MAX_RW_SIZE (MAX_RW_SECTORS*SECTOR_SIZE) 

#define   INT13h         0x13 
#define   MAX_PATH       256 
#define SECTORSIZE  	SECTOR_SIZE 

#ifndef DISKBASE
#define DISKBASE			0x80
#endif


