#include "Main.h"
#include ".\export\zlib.h"                                                
                                                
void main(void)
{        
    int  hFile,nReturn;                      
    long   lFileLen;
    DWORD  dwSize = 65535; 
    BYTE *pBuf1,*pBuf2; 
    if( (hFile = _open( "c:\\Test.dat",_O_RDWR  | _O_BINARY)) == -1 )
    {
	    printf( "open failed on input file" );
    	return;
    }               
    lFileLen = _filelength(hFile); 
    pBuf1 = (BYTE *)malloc((unsigned int)lFileLen);
    pBuf2 = (BYTE *)malloc(65500);              
    nReturn = _read( hFile,pBuf1,(unsigned)lFileLen);
    if (nReturn == -1 )
    {
      	printf("Error Read data file " );
       	_close( hFile );
        return;
    }             
    nReturn = uncompress(pBuf2,&dwSize,pBuf1,lFileLen);
    if (Z_OK != nReturn) printf("Error uncompress data" );
    _close( hFile );
    free(pBuf1);
    free(pBuf2);
}                        
       
