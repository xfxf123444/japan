
#ifndef YGCRYPTOAPI_H
#define YGCRYPTOAPI_H

#define MASK_KEY    0x4813a7e9
#define MOVE_BIT    3
#define DEFAULT_AES_KEYLENGTH 16

#define YGNOCRYPTO      0x00 
#define YGNORMALCRYPTO  0x100
#define YGDESCRYPTO     0x200
#define YGAESCRYPTO     0x300
#define YGRSACRYPTO     0x400

#define YGSTATICCRYPTO  0x101

#define YGDESEDE2       0x201
#define YGDESEDE3       0x202
#define YGDESXEX3       0x203


// RSA key lengths.
 
#define MIN_RSA_MODULUS_BITS 508
#define MAX_RSA_MODULUS_BITS 1024
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)

// Error codes.
 
#define RE_CONTENT_ENCODING 0x0400
#define RE_DATA 0x0401
#define RE_DIGEST_ALGORITHM 0x0402
#define RE_ENCODING 0x0403
#define RE_KEY 0x0404
#define RE_KEY_ENCODING 0x0405
#define RE_LEN 0x0406
#define RE_MODULUS_LEN 0x0407
#define RE_NEED_RANDOM 0x0408
#define RE_PRIVATE_KEY 0x0409
#define RE_PUBLIC_KEY 0x040a
#define RE_SIGNATURE 0x040b
#define RE_SIGNATURE_ENCODING 0x040c
#define RE_ENCRYPTION_ALGORITHM 0x040d

// RSA public and private key.
#pragma	pack(1)	//	align to byte 
 
typedef struct 
{
  ULONG bits;                           // length in bits of modulus 
  UCHAR modulus[MAX_RSA_MODULUS_LEN];                    // modulus 
  UCHAR exponent[MAX_RSA_MODULUS_LEN];           // public exponent 
} R_RSA_PUBLIC_KEY;

typedef struct 
{
  ULONG bits;                           // length in bits of modulus 
  UCHAR modulus[MAX_RSA_MODULUS_LEN];                    // modulus 
  UCHAR publicExponent[MAX_RSA_MODULUS_LEN];     // public exponent 
  UCHAR exponent[MAX_RSA_MODULUS_LEN];          // private exponent 
  UCHAR prime[2][MAX_RSA_PRIME_LEN];               // prime factors 
  UCHAR primeExponent[2][MAX_RSA_PRIME_LEN];   // exponents for CRT 
  UCHAR coefficient[MAX_RSA_PRIME_LEN];          // CRT coefficient 
} R_RSA_PRIVATE_KEY;
#pragma	pack()

#ifdef __cplusplus
extern "C"
{
#endif

long YGRSAGeneratePEMKeys(R_RSA_PUBLIC_KEY *pPbulicKey, R_RSA_PRIVATE_KEY *pPrivateKey, ULONG ulKeyLen);
long YGRSAPublicEncrypt(UCHAR *output,ULONG *outputLen,UCHAR *input,ULONG inputLen,R_RSA_PUBLIC_KEY *publicKey);
long YGRSAPrivateEncrypt (UCHAR *output,ULONG *outputLen,UCHAR *input,ULONG inputLen,R_RSA_PRIVATE_KEY *privateKey);
long YGRSAPublicDecrypt(UCHAR *output,ULONG *outputLen,UCHAR *input,ULONG inputLen,R_RSA_PUBLIC_KEY *publicKey);
long YGRSAPrivateDecrypt (UCHAR *output,ULONG *outputLen,UCHAR *input,ULONG inputLen,R_RSA_PRIVATE_KEY *privateKey);
ULONG YGRSAGetOrgiBuffLen(ULONG ulKeyLen,ULONG ulEcyLen);
ULONG YGRSAGetEcyBuffLen(ULONG ulKeyLen,ULONG ulOrgiLen);
long  YGDESEncryCode(UCHAR *pKey,UCHAR *pInBuf,UCHAR *pOutBuf,long nLen,long nType);//keylength depend on ntype
long  YGDESDecryCode(UCHAR *pKey,UCHAR *pInBuf,UCHAR *pOutBuf,long nLen,long nType);//YGDESCRYPTO 8 YGDESEDE2 16 YGDESEDE3 24 YGDESXEX3  24

long  YGNormalEncryCode(UCHAR *pBuf,long nLen);
long  YGNormalDecryCode(UCHAR *pBuf,long nLen);
long YGNormalEncryWithKey(UCHAR *pBuf,long nLen,ULONG ulKey);
long YGNormalDecryWithKey(UCHAR *pBuf,long nLen,ULONG ulKey);
//long YGGetRandom(UCHAR * pRandom,long nRandomLen);
long YGGenerate3Key(ULONG dwBase1,ULONG dwBase2,UCHAR * btEcyKey);
long YGAESDecryptData(UCHAR *pKey,ULONG ulKeyLen,UCHAR *pInBuffer,UCHAR *pOutBuffer, ULONG ulDataSize);
long YGAESEncryptData(UCHAR *pKey,ULONG ulKeyLen,UCHAR *pInBuffer,UCHAR *pOutBuffer, ULONG ulDataSize);

#ifdef __cplusplus
}
#endif

#endif
