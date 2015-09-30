//#include "time.h"
#include "misc.h"
#include "..\Export\Crypto.h"
#include "des.h"
#include "AES.h"
void Rol(ULONG& dwData,ULONG dwKey,long nBit);
void RoR(ULONG& dwData,ULONG dwKey,long nBit);

long YGAESGenerateKey(UCHAR *pKey,ULONG ulKeyLen,UCHAR *pOutKey)
{
	ULONG i;
	YGECMemset(pOutKey,'0',DEFAULT_AES_KEYLENGTH);
	for (i = 0;i < ulKeyLen;i++)
		pOutKey[i%DEFAULT_AES_KEYLENGTH] += pKey[i];
	return TRUE;
}

long YGAESEncryptData(UCHAR *pKey,ULONG ulKeyLen,UCHAR *pInBuffer,UCHAR *pOutBuffer, ULONG ulDataSize)
{
	ULONG i,ulLeft;
	UCHAR Key[DEFAULT_AES_KEYLENGTH];

	YGAESGenerateKey(pKey,ulKeyLen,Key);
	AESEncryption AESCryptor(Key,DEFAULT_AES_KEYLENGTH);

	ulLeft = ulDataSize % DEFAULT_AES_KEYLENGTH;
	for (i = 0;i+DEFAULT_AES_KEYLENGTH <= ulDataSize;i+=DEFAULT_AES_KEYLENGTH)
		AESCryptor.ProcessBlock(&pInBuffer[i],&pOutBuffer[i]);
	if (ulLeft)
	{
		YGECMemcpy(&pOutBuffer[i],&pInBuffer[i],ulLeft);
		YGNormalEncryWithKey(&pOutBuffer[i],ulLeft,*((ULONG *)Key));
	}

	return TRUE;
}

long YGAESDecryptData(UCHAR *pKey,ULONG ulKeyLen,UCHAR *pInBuffer,UCHAR *pOutBuffer, ULONG ulDataSize)
{
	ULONG i,ulLeft;
	UCHAR Key[DEFAULT_AES_KEYLENGTH];

	YGAESGenerateKey(pKey,ulKeyLen,Key);
	AESDecryption AESDecryptor(Key,DEFAULT_AES_KEYLENGTH);

	ulLeft = ulDataSize % DEFAULT_AES_KEYLENGTH;
	for (i = 0;i+DEFAULT_AES_KEYLENGTH <= ulDataSize;i+=DEFAULT_AES_KEYLENGTH)
		AESDecryptor.ProcessBlock(&pInBuffer[i],&pOutBuffer[i]);
	if (ulLeft)
	{
		YGECMemcpy(&pOutBuffer[i],&pInBuffer[i],ulLeft);
		YGNormalDecryWithKey(&pOutBuffer[i],ulLeft,*((ULONG *)Key));
	}

	return TRUE;
}

long YGDESEncryCode(UCHAR *pKey,UCHAR *pInBuf,UCHAR *pOutBuf,long nLen,long nType)
{
	long i,nBlockSize,nLeft;
	switch (nType)
	{
	case YGDESCRYPTO:  
		{
			DES DesEncrypto(pKey,ENCRYPTION);
			nBlockSize = DesEncrypto.BlockSize();
			nLeft = nLen % nBlockSize;
			for (i = 0;i+nBlockSize <= nLen;i+=nBlockSize)
				DesEncrypto.ProcessBlock(&pInBuf[i],&pOutBuf[i]);
			if (nLeft)
			{
				YGECMemcpy(&pOutBuf[i],&pInBuf[i],nLeft);
				YGNormalEncryCode(&pOutBuf[i],nLeft);
			}
		}
		break;
	case YGDESEDE2:
		{
			DES_EDE2_Encryption DesEncrypto(pKey,0);
			nBlockSize = DesEncrypto.BlockSize();
			nLeft = nLen % nBlockSize;
			for (i = 0;i+nBlockSize <= nLen;i+=nBlockSize)
				DesEncrypto.ProcessBlock(&pInBuf[i],&pOutBuf[i]);
			if (nLeft)
			{
				YGECMemcpy(&pOutBuf[i],&pInBuf[i],nLeft);
				YGNormalEncryCode(&pOutBuf[i],nLeft);
			}
		}
		break;
	case YGDESEDE3:
		{
			DES_EDE3_Encryption DesEncrypto(pKey,0);
			nBlockSize = DesEncrypto.BlockSize();
			nLeft = nLen % nBlockSize;
			for (i = 0;i+nBlockSize <= nLen;i+=nBlockSize)
				DesEncrypto.ProcessBlock(&pInBuf[i],&pOutBuf[i]);
			if (nLeft)
			{
				YGECMemcpy(&pOutBuf[i],&pInBuf[i],nLeft);
				YGNormalEncryCode(&pOutBuf[i],nLeft);
			}
		}
		break;
	case YGDESXEX3:
		{
			DES_XEX3_Encryption DesEncrypto(pKey,0);
			nBlockSize = DesEncrypto.BlockSize();
			nLeft = nLen % nBlockSize;
			for (i = 0;i+nBlockSize <= nLen;i+=nBlockSize)
				DesEncrypto.ProcessBlock(&pInBuf[i],&pOutBuf[i]);
			if (nLeft)
			{
				YGECMemcpy(&pOutBuf[i],&pInBuf[i],nLeft);
				YGNormalEncryCode(&pOutBuf[i],nLeft);
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

long YGDESDecryCode(UCHAR *pKey,UCHAR *pInBuf,UCHAR *pOutBuf,long nLen,long nType)
{
	long i,nBlockSize,nLeft;
	switch (nType)
	{
	case YGDESCRYPTO:  
		{
			DES DesEncrypto(pKey,DECRYPTION);
			nBlockSize = DesEncrypto.BlockSize();
			nLeft = nLen % nBlockSize;
			for (i = 0;i+nBlockSize <= nLen;i+=nBlockSize)
				DesEncrypto.ProcessBlock(&pInBuf[i],&pOutBuf[i]);
			if (nLeft)
			{
				YGECMemcpy(&pOutBuf[i],&pInBuf[i],nLeft);
				YGNormalDecryCode(&pOutBuf[i],nLeft);
			}
		}
		break;
	case YGDESEDE2:
		{
			DES_EDE2_Decryption DesEncrypto(pKey,0);
			nBlockSize = DesEncrypto.BlockSize();
			nLeft = nLen % nBlockSize;
			for (i = 0;i+nBlockSize <= nLen;i+=nBlockSize)
				DesEncrypto.ProcessBlock(&pInBuf[i],&pOutBuf[i]);
			if (nLeft)
			{
				YGECMemcpy(&pOutBuf[i],&pInBuf[i],nLeft);
				YGNormalDecryCode(&pOutBuf[i],nLeft);
			}
		}
		break;
	case YGDESEDE3:
		{
			DES_EDE3_Decryption DesEncrypto(pKey,0);
			nBlockSize = DesEncrypto.BlockSize();
			nLeft = nLen % nBlockSize;
			for (i = 0;i+nBlockSize <= nLen;i+=nBlockSize)
				DesEncrypto.ProcessBlock(&pInBuf[i],&pOutBuf[i]);
			if (nLeft)
			{
				YGECMemcpy(&pOutBuf[i],&pInBuf[i],nLeft);
				YGNormalDecryCode(&pOutBuf[i],nLeft);
			}
		}
		break;
	case YGDESXEX3:
		{
			DES_XEX3_Decryption DesEncrypto(pKey,0);
			nBlockSize = DesEncrypto.BlockSize();
			nLeft = nLen % nBlockSize;
			for (i = 0;i+nBlockSize <= nLen;i+=nBlockSize)
				DesEncrypto.ProcessBlock(&pInBuf[i],&pOutBuf[i]);
			if (nLeft)
			{
				YGECMemcpy(&pOutBuf[i],&pInBuf[i],nLeft);
				YGNormalDecryCode(&pOutBuf[i],nLeft);
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

long YGNormalEncryCode(UCHAR *pBuf,long nLen)
{
	long i;
	for(i=0;i+4 < nLen; i+=4)
	{
		RoR(*(ULONG*)&pBuf[i],MASK_KEY,MOVE_BIT);
	}
	for (;i<nLen;i++) pBuf[i] += 'A';
	return TRUE;
}

long YGNormalDecryCode(UCHAR *pBuf,long nLen)
{
	long  i;
	for(i=0;i+4 < nLen; i+=4)
	{
		Rol(*(ULONG*)&pBuf[i],MASK_KEY,MOVE_BIT);
	}
	for (;i<nLen;i++) pBuf[i] -= 'A';
	return TRUE;
}

long YGNormalEncryWithKey(UCHAR *pBuf,long nLen,ULONG ulKey)
{
	ULONG ulMask = MASK_KEY;
	long i;

	ulMask += ulKey;
	for(i=0;i+4 < nLen; i+=4)
	{
		RoR(*(ULONG*)&pBuf[i],ulMask,MOVE_BIT);
	}
	for (;i<nLen;i++) pBuf[i] += 'A';
	return TRUE;
}

long YGNormalDecryWithKey(UCHAR *pBuf,long nLen,ULONG ulKey)
{
	ULONG ulMask = MASK_KEY;
	long  i;

	ulMask += ulKey;
	for(i=0;i+4 < nLen; i+=4)
	{
		Rol(*(ULONG*)&pBuf[i],ulMask,MOVE_BIT);
	}
	for (;i<nLen;i++) pBuf[i] -= 'A';
	return TRUE;
}

/*
long YGGetRandom(UCHAR * pRandom,long nRandomLen)
{
   long  nData,nPos;
   USHORT *pBuf;
   time_t lTime;
   pBuf = (USHORT *)pRandom;
   if (!nRandomLen) return FALSE;
   for (nPos = 0;nPos < nRandomLen/2;nPos++)
   {
	   time(&lTime);
	   lTime  = lTime*nPos*0x1111;
	   srand((unsigned)lTime);
	   nData = rand();
	   pBuf[nPos] = (USHORT)(nData&0xFFFF);	
   }
   if (nPos * 2 != nRandomLen) pRandom[nRandomLen] = 'Y';
   return TRUE;
}
*/
void Rol(ULONG& dwData,ULONG dwKey,long nBit)
{
	dwData ^= dwKey;
	ULONG dwData1,dwData2;
	dwData1 = dwData << nBit;
	dwData2 = dwData >> (32 - nBit);
	dwData = (dwData1 | dwData2);
}

void RoR(ULONG& dwData,ULONG dwKey,long nBit)
{
	ULONG dwData1,dwData2;
	dwData1 = dwData >> nBit;
	dwData2 = dwData << (32 - nBit);
	dwData = (dwData1 | dwData2);
	dwData ^= dwKey;
}
long YGGenerate3Key(ULONG dwBase1,ULONG dwBase2,UCHAR * btEcyKey)
{
	char  szText[12];
	YGECMemcpy(btEcyKey,(UCHAR *)&dwBase1,4);
	YGECMemcpy(&btEcyKey[4],(UCHAR *)&dwBase2,4);
	YGECSprintf(szText,"%10.10u",dwBase1);
	YGECMemcpy(&btEcyKey[8],(UCHAR *)&szText,8);
	YGECSprintf(szText,"%10.10u",dwBase2);
	YGECMemcpy(&btEcyKey[16],(UCHAR *)&szText,8);
	YGNormalEncryCode(btEcyKey,24);
	return TRUE;
}
