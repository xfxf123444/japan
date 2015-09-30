/*
 *  Shared MACRO
 *
 */
#ifndef SHARE_H_
#define SHARE_H_

typedef unsigned int size_t;

int __cdecl memcmp(const void *, const void *,size_t);
void * __cdecl memcpy(void *, const void *,size_t);
void * __cdecl memset(void *, int, size_t);
char * __cdecl strcat(char *, const char *);
int __cdecl strcmp(const char *, const char *);
char * __cdecl strcpy(char *, const char *);
size_t __cdecl strlen(const char *);



//#include <string.h>

#pragma intrinsic(memcmp, memcpy, memset, strcat, strcmp, strcpy, strlen)


#ifdef	 DEBUG
    #define ASSERT(e)	if(!(e)){Debug_Printf("Assertion failure in "__FILE__",line %d:" #e "\r\n",__LINE__);\
			_asm	int 1	\
			}
#else
    #define ASSERT(e)
#endif

#ifdef	DEBUG
    #define TRAP	if(TRUE){Debug_Printf("TRAP in "__FILE__",line %d:\r\n",__LINE__);\
			_asm	int 3	\
			}
#else
    #define TRAP
#endif

#ifndef MAKELONG
    #define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#endif

#pragma warning(disable:4035)

#ifndef NAKED
    #define NAKED __declspec(naked)
#endif

#ifndef arraysize
    #define arraysize(p) (sizeof(p)/sizeof((p)[0]))
#endif


typedef struct     
{
	BYTE	BootFlag;   
	BYTE	StartOfPartition[3];
	BYTE	SystemFlag;
	BYTE	EndOfPartition[3];
	DWORD	StartSector;
	DWORD	SectorsInPartition;
} PARTITION_ENTRY,*PPARTITION_ENTRY;

/*
 *  Shared Data Structure
 */

typedef struct	tagPartiEntry
    {
	BYTE	btBootFlag;
	BYTE	btStartOfPartition[3];
	BYTE	btSystemFlag;
	BYTE	btEndOfPartition[3];
	DWORD	dwStartSector;
	DWORD	dwSectorsInPartition;
    } PartiEntry,*PPartiEntry;


#define MAX_PAR_NUM	10

typedef struct	tagLogicParInfo
    {
	PartiEntry  peCurParInfo;
	PartiEntry  peNxtParInfo;
    } LogicParInfo,*PLogicParInfo;


typedef struct	tagParInfoOnHardDisk
    {
	WORD		wNumOfPri;
	WORD		wNumOfLogic;
	PartiEntry	pePriParInfo[4];
	LogicParInfo	lpiLogParInfo[MAX_PAR_NUM];
    } ParInfoOnHardDisk,*PParInfoOnHardDisk;

#define RESIZE_PRIMARY_PARTITION				1
#define RESIZE_NORMAL_1ST_LOGIC_PARTITION_NO_NULL_PARTITION	2
#define RESIZE_NORMAL_1ST_LOGIC_PARTITION_CREATE_NULL_PARTITION 3
#define RESIZE_SPECIAL_1ST_LOGIC_PARTITION_REMOVE_NULL_PARTITION    4
#define RESIZE_SPECIAL_1ST_LOGIC_PARTITION_RESERVE_NULL_PARTITION   5
#define RESIZE_NON_1ST_LOGIC_PARTITION				6

typedef struct	tagMonParInfo
    {
	BYTE		    btResizeType;
	BYTE		    btReserved[3];  //	padding
	PartiEntry	    peVirtualPar;
	ParInfoOnHardDisk   piohdTotalParInfo;
    } MonParInfo,*PMonParInfo;


typedef struct	tagVirtualPartition
    {
	BYTE		    btResizeType;
	BYTE		    btIndex;
	BYTE		    btReserved[2];  //	padding
	DWORD		    dwSectorOfPrevPartiTbl;
	DWORD		    dwStartingSectorOfVP;
	DWORD		    dwEndingSectorOfVP;
    } VirtualPartition,*PVirtualPartition;


typedef struct	tagDiskParams
    {
	DWORD	dwCylinders;
	DWORD	dwHeads;
	DWORD	dwSecPerTrack;
    } DiskParams,*PDiskParams;

#endif	//  SHARE_H_
