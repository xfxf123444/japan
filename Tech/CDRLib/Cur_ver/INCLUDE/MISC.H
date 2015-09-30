#if !defined(CDRLIB_MISC_H)
#define CDRLIB_MISC_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

// Misc macros.

#define LOOP while(1)

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

#define RANGE(value, min, max) \
  (((value) < (min)) ? (min) : (((value) > (max)) ? (max) : (value)))

#define ROUNDUP(value, x) (CDIV(value, x) * (x))
#define ROUNDDOWN(value, x) (((value) / (x)) * (x))

#define PADDING(value, x) ((CDIV(value, x) * (x)) - value)

#define EVEN(len) (! ((len) & 1))
#define ODD(len)  ((len) & 1)

#define BIT(field, mask) ((field & mask) != 0)
#define SETBIT(field, mask) field |= (mask)
#define CLRBIT(field, mask) field &= (~(mask))

#define INTCMP(x1, x2) ((x1 < x2) ? -1 : ((x1 > x2) ? 1 : 0))

// Flip a 16-bit value.

#define FLIP16(x) (((UWORD)(x) << 8) | ((UWORD)(x) >> 8))

// Flip a 24-bit value.

#define FLIP24(x) (                   \
  (((ULONG)(x) << 16) & 0x00FF0000) | \
  (((ULONG)(x))       & 0x0000FF00) | \
  (((ULONG)(x) >> 16) & 0x000000FF))

// Flip a 32-bit value.

#define FLIP32(x) (                   \
  (((ULONG)(x) << 24) & 0xFF000000) | \
  (((ULONG)(x) <<  8) & 0x00FF0000) | \
  (((ULONG)(x) >>  8) & 0x0000FF00) | \
  (((ULONG)(x) >> 24) & 0x000000FF))

// Swap the high/low order words of a longword.

#define SWAPW(x) (((ULONG)(x) << 16) | ((ULONG)(x) >> 16))

// Rotate a longword right/left.

#define ROTR(x,r) (((ULONG)(x) >> (r)) | ((ULONG)(x) << (32 - (r))))
#define ROTL(x,l) (((ULONG)(x) << (l)) | ((ULONG)(x) >> (32 - (l))))

// Memory allocation macros.

#define MALLOC(count,type) (type *)malloc(count * sizeof(type))
#define CALLOC(count,type) (type *)calloc(count, sizeof(type))

// Clear a block of memory.

#define MEMCLEAR(target, length) memset(target, 0, length)

// Span over a structure.

#define SPANSTRUCT(s) (s + 1)

// Filetypes.

#define FILETYPE_UNKNOWN        0
#define FILETYPE_AUTOMATIC      1
#define FILETYPE_BINARY_LSB     2
#define FILETYPE_BINARY_MSB     3
#define FILETYPE_WAVE_STEREO    4
#define FILETYPE_WAVE_MONO      5
#define FILETYPE_AIFF_STEREO    6
#define FILETYPE_AIFF_MONO      7
#define FILETYPE_MP3            8

// WAVE file header

struct WAVEFMTCHUNK {
  UWORD format;
  UWORD channels;
  ULONG samples_per_sec;
  ULONG avg_samples_per_sec;
  UWORD alignment;
  UWORD bits_per_sample;
  };

struct WAVEHEADER {
  ULONG RIFF;
  ULONG length;
  ULONG WAVE;
  ULONG fmttag;
  ULONG fmtlen;
  WAVEFMTCHUNK fmtchunk;
  ULONG datatag;
  ULONG datalen;
  };

// AIFF file header

#define IEEE_44100  "\x40\x0E\xAC\x44\x00\x00\x00\x00\x00\x00"

struct AIFFCOMMCHUNK {
  UWORD channels;
  ULONG frames;
  UWORD bits_per_sample;
  UBYTE samples_per_sec[10];
  };

struct AIFFHEADER {
  ULONG FORM;
  ULONG length;
  ULONG AIFF;
  ULONG commtag;
  ULONG commlen;
  AIFFCOMMCHUNK commchunk;
  ULONG ssndtag;
  ULONG ssndlen;
  ULONG offset;
  ULONG block_size;
  };

// I/O Buffer

struct IOBUF {
  UBYTE data[65000];
  };

// I/O buffer class.
// 32-bit implementaion uses "GlobalAlloc" to insure 8-byte buffer alignment.

#if defined(WIN32)
class IOBuffer {
  public:
    IOBuffer() {handle = NULL;}
    IOBuffer(ULONG buflen, BOOL zero_flag = FALSE) {Allocate(buflen, zero_flag);}
    ~IOBuffer() {Deallocate();}
    void *GetHandle() {return(handle);}
    void *Allocate(ULONG buflen, BOOL zero_flag = FALSE)
      {return (handle = GlobalAlloc((zero_flag ? GPTR : GMEM_FIXED), buflen));}
    void Deallocate() {if (handle != NULL) {GlobalFree(handle); handle = NULL;}}
  private:
    void *handle;
  };
#else
class IOBuffer {
  public:
    IOBuffer() {handle = NULL;}
    IOBuffer(ULONG buflen, BOOL zero_flag = FALSE) {Allocate(buflen, zero_flag);}
    ~IOBuffer() {Deallocate();}
    void *GetHandle() {return(handle);}
    void *Allocate(ULONG buflen, BOOL zero_flag = FALSE)
      {
      handle = new UBYTE[buflen];
      if ((handle != NULL) && zero_flag) memset(handle, 0, buflen);
      return (handle);
      }
    void Deallocate() {if (handle != NULL) {delete []handle; handle = NULL;}}
  private:
    UBYTE *handle;
  };
#endif

// Automatic pointer class template.

#if !defined(WIN32)
#define explicit
#endif

template <class T>
class auto_ptr {
  public:
    explicit auto_ptr(T *p = NULL) : pointee(p) {}
    auto_ptr(auto_ptr<T>& rhs) : pointee(rhs.release()) {}
    ~auto_ptr() {delete pointee;}

    auto_ptr<T>& operator=(auto_ptr<T>& rhs)
      {
      if (this != &rhs) reset(rhs.release());
      return *this;
      }

    T& operator*() const {return *pointee;}
    T* operator->() const {return pointee;}

    T* get() const {return pointee;}

    T* release()
      {
      T *oldpointee = pointee;
      pointee = 0;
      return oldpointee;
      }  

    void reset(T *p = 0)
      {
      if (pointee != p)
        {
        delete pointee;
        pointee = p;
        }
      }

  private:
    T *pointee;
  };

template <class T>
class auto_array_ptr {
  public:
    explicit auto_array_ptr(T *p = 0) : pointee(p) {}
    auto_array_ptr(auto_array_ptr<T>& rhs) : pointee(rhs.release()) {}
    ~auto_array_ptr() {delete []pointee;}

    auto_array_ptr<T>& operator=(auto_array_ptr<T>& rhs)
      {
      if (this != &rhs) reset(rhs.release());
      return *this;
      }

    T& operator*() const {return *pointee;}
    T* operator->() const {return pointee;}
    T& operator[](int nIndex) const {return pointee[nIndex];}

    T* get() const {return pointee;}

    T* release()
      {
      T *oldpointee = pointee;
      pointee = 0;
      return oldpointee;
      }  

    void reset(T *p = 0)
      {
      if (pointee != p)
        {
        delete []pointee;
        pointee = p;
        }
      }

  private:
    T *pointee;
  };

//
// Function prototypes.
//

int ConsoleNewHandler(size_t size);

void GetEnvironmentVariables(void);

UWORD ComputeCRC16x4(
  const UWORD *tableP, const void *bufferP, UWORD buflen, UWORD initial_crc = 0, UWORD final_xor = 0,
  BOOL table_reflected = FALSE, BOOL reflect_buffer = FALSE, BOOL reflect_crc = FALSE);

UWORD ComputeCRC16x8(
  const UWORD *tableP, const void *bufferP, UWORD buflen, UWORD initial_crc = 0, UWORD final_xor = 0,
  BOOL table_reflected = FALSE, BOOL reflect_buffer = FALSE, BOOL reflect_crc = FALSE);

ULONG ComputeCRC32x4(
  const ULONG *tableP, const void *bufferP, UWORD buflen, ULONG initial_crc = 0, ULONG final_xor = 0,
  BOOL table_reflected = FALSE, BOOL reflect_buffer = FALSE, BOOL reflect_crc = FALSE);

ULONG ComputeCRC32x8(
  const ULONG *tableP, const void *bufferP, UWORD buflen, ULONG initial_crc = 0, ULONG final_xor = 0,
  BOOL table_reflected = FALSE, BOOL reflect_buffer = FALSE, BOOL reflect_crc = FALSE);

void GenerateCRC16x4Table(UWORD poly, BOOL reflect_flag, UWORD *tableP);
void GenerateCRC16x8Table(UWORD poly, BOOL reflect_flag, UWORD *tableP);

void GenerateCRC32x4Table(ULONG poly, BOOL reflect_flag, ULONG *tableP);
void GenerateCRC32x8Table(ULONG poly, BOOL reflect_flag, ULONG *tableP);

void CorruptDataBuffer(void *bufP, UWORD blklen, UWORD blocks);

UBYTE BCD2Binary(UBYTE bcd);
UBYTE Binary2BCD(UBYTE num);

void *MemAllocate(UWORD size, BOOL fill_flag = FALSE);

BOOL FileWriteZeros(FILEHANDLE handle, UWORD blklen, ULONG blocks);
TCHAR *FileAddExtension(TCHAR *filenameP, const TCHAR *extensionP, BOOL replace_flag);

int GetFilenameType(
  const TCHAR *filenameP, int default_filetype, BOOL unknown_allowed = FALSE);

int AnalyzeImageFile(
  const TCHAR *filnamP, int filetype, UQUAD *data_lengthP, ULONG *data_offsetP);

void InitFileHeaderAIFF(AIFFHEADER *aiffheader, ULONG datalen);
void InitFileHeaderWAVE(WAVEHEADER *waveheader, ULONG datalen);

BOOL ParseFileHeaderAIFF(FILEHANDLE file, ULONG *offset, ULONG *datalen);
BOOL ParseFileHeaderWAVE(FILEHANDLE file, ULONG *offset, ULONG *datalen);

void *MemCopy(void *targetP, const void *sourceP, int source_len);
void *MemFill(void *targetP, int target_len, UBYTE fill);

void FlipByteOrder16(void *bufP, ULONG word_count);
void FlipByteOrder32(void *bufP, ULONG long_count);

void ReflectByteBuffer(void *bufP, ULONG buflen);
void ReflectWordBuffer(void *bufP, ULONG buflen);
void ReflectLongBuffer(void *bufP, ULONG buflen);

UBYTE Reflect8(UBYTE value);
UWORD Reflect16(UWORD value);
ULONG Reflect32(ULONG value);

SWORD Checksum16(void *bufP, ULONG buflen);
SLONG Checksum32(void *bufP, ULONG buflen);

UWORD RandomNumber(UWORD min, UWORD max);

void DumpBufferBinary (FILE *file, void *bufP, UWORD buflen);

void MultiplySigned32x32To64 (
  SLONG multiplier, SLONG multiplicand, SQUAD *productP);

void MultiplyUnsigned32x32To64 (
  ULONG multiplier, ULONG multiplicand, UQUAD *productP);

void Divide64By32To32(
  UQUAD dividend, ULONG divisor, ULONG *quotientP, ULONG *remainderP);

ULONG CDIV(ULONG value, ULONG x);
ULONG CDIV64(UQUAD value, ULONG x);

int CompareValues(ULONG v1, ULONG v2);

inline void ConvertWordToLE(UWORD value, UWORD *ptr)
{
  *ptr = value;
}

inline void ConvertWordToBE(UWORD value, UWORD *ptr)
{
  *ptr = (UWORD)FLIP16(value);
}

inline void ConvertWordToLEBE(UWORD value, UWORD *ptr)
{
  ptr[0] = value;
  ptr[1] = (UWORD)FLIP16(value);
}

inline void ConvertLongToLE(ULONG value, ULONG *ptr)
{
  *ptr = value;
}

inline void ConvertLongToBE(ULONG value, ULONG *ptr)
{
  *ptr = FLIP32(value);
}

inline void ConvertLongToLEBE(ULONG value, ULONG *ptr)
{
  ptr[0] = value;
  ptr[1] = FLIP32(value);
}

// Restore previous packing alignment.

#pragma pack(pop)

#endif
