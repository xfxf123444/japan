/*
 * YGdesc.c
 * 
 * des algorithm implemented in C
 * Weiqi
 * 2002-10-15 15:54
 */

#include "..\..\..\..\..\include\cur_ver\basedef.h"
#include "..\export\crypto.h"
#include "..\export\YGdesc.h"
#include "d3des.h"

BOOL InitDesInfo(LPDES_INFO lpDesInfo, LPBYTE szKey, WORD wKeyLength, WORD wAlg, BOOL bEncOrDec)
{
	short	mode = bEncOrDec ? EN0 : DE1;
	
	if (!lpDesInfo ||!szKey)
		return FALSE;
	
	memset(lpDesInfo, 0, sizeof(DES_INFO));
	if (wAlg == YGDESEDE3 || wAlg == YGDESXEX3)
	{
		make3key(lpDesInfo, szKey, wKeyLength);
		des3key(lpDesInfo, NULL, mode);
	}
	else if (wAlg == YGDESEDE2)
	{
		make2key(lpDesInfo, szKey, wKeyLength);
		des2key(lpDesInfo, NULL, mode);
	}
	else if (wAlg == YGDESCRYPTO)
	{
		makekey(lpDesInfo, szKey, wKeyLength);
		deskey(lpDesInfo, NULL, mode);
	}
	else
		return FALSE;
	
	return TRUE;
}

BOOL YGDes(LPDES_INFO lpDesInfo, LPBYTE lpbtIn, DWORD dwSize, LPBYTE lpbtOut)
{
	register long nBlock;

	if (!lpDesInfo || !lpbtIn || !lpbtOut || (dwSize % 8))
		return FALSE;
	if (dwSize)
	{
		for (nBlock = dwSize / 8; nBlock > 0 ; nBlock--)
		{
			if (lpDesInfo->Type == D2_DES || lpDesInfo->Type == D3_DES)
				Ddes(lpDesInfo,lpbtIn,lpbtOut);
			else if (lpDesInfo->Type == D1_DES)
				des(lpDesInfo,lpbtIn,lpbtOut);
			else
				return FALSE;
							
			lpbtIn += 8;
			lpbtOut+= 8;
		}
	}
	
	return TRUE;
}