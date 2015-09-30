/*
 * YGdesc.h
 * 
 * des algorithm implemented in C
 * Weiqi
 * 2002-10-15 15:54
 */

#ifndef EXPROT_HEADER_YG_DESC
#define EXPROT_HEADER_YG_DESC

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

typedef struct 
{
	long	Type, Mode;
	unsigned char Key[24];
	unsigned long KnL[32];
	unsigned long KnR[32];
	unsigned long Kn3[32];
}DES_INFO, * PDES_INFO, FAR * LPDES_INFO, NEAR * NPDES_INFO;

#pragma pack()

//des algorithm
BOOL InitDesInfo(LPDES_INFO lpDesInfo, LPBYTE szKey, WORD wKeyLength, WORD wAlg, BOOL bEncOrDec);
BOOL YGDes(LPDES_INFO lpDesInfo, LPBYTE lpbtIn, DWORD dwSize, LPBYTE lpbtOut);

#ifdef __cplusplus
}
#endif

#endif
