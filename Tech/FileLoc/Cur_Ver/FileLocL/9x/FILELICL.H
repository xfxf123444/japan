#ifndef     SECTOR_SIZE
#define     SECTOR_SIZE 	    512
#endif

#define SIGN_LEN	496
#define SIGN_HEAD_LEN	SECTOR_SIZE - SIGN_LEN

typedef struct tagLOC_INFO
{
    union
    {
	struct
	{
	    PQWORD  pqwPhysSector;
	    DWORD   dwLogicSecNum;
	    DWORD   dwReserve;
	    CHAR    LocSign[SIGN_LEN];
	};
	BYTE	Dum[SECTOR_SIZE];
    };
} LOC_INFO,*PLOC_INFO;
