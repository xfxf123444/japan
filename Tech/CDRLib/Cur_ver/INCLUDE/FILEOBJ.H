#if !defined(CDRLIB_FILEOBJ_H)
#define CDRLIB_FILEOBJ_H

// Xaudio headers.
//#if defined(WIN32) && defined(_MP3)
//#include "decoder.h"
//#endif

//
// Image file object base class
//

class ImageFile {
  public:
    ImageFile(const TCHAR *filnamP, UQUAD datalen, ULONG offset);
    virtual ~ImageFile();

    static ImageFile *Open(const TCHAR *filnamP, UBYTE filetype, UQUAD datalen, ULONG offset);

    virtual BOOL Read(void *bufP, ULONG buflen);
    virtual BOOL Close();
    virtual BOOL SeekBegin(SLONG offset);
    virtual BOOL SeekCurrent(SLONG offset);
    virtual BOOL SeekEnd(SLONG offset);
    virtual ULONG GetLength();

    FILEHANDLE GetHandle() {return (Handle);}
    TCHAR *GetFilename() {return (Filename);}

  protected:
    FILEHANDLE Handle;
    TCHAR Filename[_MAX_PATH + 1];
  };

#endif
