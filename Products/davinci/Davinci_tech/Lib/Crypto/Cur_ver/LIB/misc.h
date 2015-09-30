#ifndef CRYPTOPP_MISC_H
#define CRYPTOPP_MISC_H

typedef unsigned char       UCHAR;
typedef unsigned long		ULONG;
typedef unsigned short		USHORT;       

#define FALSE   0
#define TRUE    1

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif // NULL

#define GETBYTE(x, y) (unsigned int)(((x)>>(8*(y)))&255)

#define SecAlloc(type, number) (new type[(number)])
#define SecFree(ptr, number) (delete [] (ptr))

inline UCHAR *YGECMemset(UCHAR *ptr,long data,long size)
{
	long i;
	for (i = 0;i < size;i++)
	{
		ptr[i] = (UCHAR)data;
	}
	return ptr;
}

inline UCHAR *YGECMemcpy(UCHAR *ptr1,UCHAR *ptr2,long size)
{
	long i;
	for (i = 0;i < size;i++)
	{
		ptr1[i] = ptr2[i];
	}
	return ptr1;
}                                                          

inline long YGECMemcmp(UCHAR *ptr1,UCHAR *ptr2,long size)
{
	long i;
	for (i = 0;i < size;i++)
	{
		if (ptr1[i] != ptr2[i]) return ptr1[i] - ptr2[i];
	}
	return 0;
}


inline void YGECSprintf(char *ptr,char *ptr2,ULONG dwBase)
{
	long i,lnTemp;
	char szFormat[12] = "%10.10u";
	if (YGECMemcmp((UCHAR *)szFormat,(UCHAR *)ptr2,8)) return;
	for (i = 0;i < 10;i++)
	{
		lnTemp = dwBase % 10;
		ptr[9-i] = (char)lnTemp+'0';
		dwBase /= 10;
	}
	return;
}

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define ROTATE_RIGHT(x, n) (((x) >> (n)) | ((x) << (32-(n))))

const ULONG WORD_SIZE = sizeof(USHORT);

// ************** misc functions ***************

inline void XorWords(USHORT *r, const USHORT *a, const USHORT *b, ULONG n)
{
	for (ULONG i=0; i<n; i++)
		r[i] = a[i] ^ b[i];
}

inline void XorWords(USHORT *r, const USHORT *a, ULONG n)
{
	for (ULONG i=0; i<n; i++)
		r[i] ^= a[i];
}

void xorbuf(UCHAR *buf, const UCHAR *mask, ULONG count);
void xorbuf(UCHAR *output, const UCHAR *input, const UCHAR *mask, ULONG count);


// ************** rotate functions ***************

inline UCHAR rotlFixed(UCHAR x, ULONG y)
{
	return (x<<y) | (x>>(sizeof(UCHAR)*8-y));
}

inline UCHAR rotrFixed(UCHAR x, ULONG y)
{
	return (x>>y) | (x<<(sizeof(UCHAR)*8-y));
}

inline USHORT rotlFixed(USHORT x, ULONG y)
{
	return (x<<y) | (x>>(sizeof(USHORT)*8-y));
}

inline USHORT rotrFixed(USHORT x, ULONG y)
{
	return (x>>y) | (x<<(sizeof(USHORT)*8-y));
}

inline ULONG rotlFixed(ULONG x, ULONG y)
{
	return y ? ROTATE_LEFT(x, y) : x;
}

inline ULONG rotrFixed(ULONG x, ULONG y)
{
	return y ? ROTATE_RIGHT(x, y) : x;
}

// ************** endian reversal ***************

inline USHORT byteReverse(USHORT value)
{
	return rotlFixed(value, 8U);
}

inline ULONG byteReverse(ULONG value)
{
	return (rotrFixed(value, 8U) & 0xff00ff00) | (rotlFixed(value, 8U) & 0x00ff00ff);
}

inline UCHAR bitReverse(UCHAR value)
{
	value = ((value & 0xAA) >> 1) | ((value & 0x55) << 1);
	value = ((value & 0xCC) >> 2) | ((value & 0x33) << 2);
	return rotlFixed(value, 4);
}

inline USHORT bitReverse(USHORT value)
{
	value = ((value & 0xAAAA) >> 1) | ((value & 0x5555) << 1);
	value = ((value & 0xCCCC) >> 2) | ((value & 0x3333) << 2);
	value = ((value & 0xF0F0) >> 4) | ((value & 0x0F0F) << 4);
	return byteReverse(value);
}

inline ULONG bitReverse(ULONG value)
{
	value = ((value & 0xAAAAAAAA) >> 1) | ((value & 0x55555555) << 1);
	value = ((value & 0xCCCCCCCC) >> 2) | ((value & 0x33333333) << 2);
	value = ((value & 0xF0F0F0F0) >> 4) | ((value & 0x0F0F0F0F) << 4);
	return byteReverse(value);
}

template <class T>
void byteReverse(T *out, const T *in, unsigned int byteCount)
{
	unsigned int count = (byteCount+sizeof(T)-1)/sizeof(T);
	for (unsigned int i=0; i<count; i++)
		out[i] = byteReverse(in[i]);
}

template <class T>
inline void GetUserKeyLittleEndian(T *out, unsigned int outlen, const UCHAR *in, unsigned int inlen)
{
	const unsigned int U = sizeof(T);
	YGECMemcpy((UCHAR *)out,(UCHAR *)in, inlen);
	YGECMemset((UCHAR *)out+inlen, 0, outlen*U-inlen);
	byteReverse(out, out, inlen);
}

template <class T>
inline void GetUserKeyBigEndian(T *out, unsigned int outlen, const UCHAR *in, unsigned int inlen)
{
	const unsigned int U = sizeof(T);
	YGECMemcpy((UCHAR *)out,(UCHAR *)in, inlen);
	YGECMemset((UCHAR *)out+inlen, 0, outlen*U-inlen);
}

inline void GetBlockBigEndian(const UCHAR *block, ULONG &a, ULONG &b)
{
	a = byteReverse(((ULONG *)block)[0]);
	b = byteReverse(((ULONG *)block)[1]);
}

inline void PutBlockBigEndian(UCHAR *block, ULONG a, ULONG b)
{
	((ULONG *)block)[0] = byteReverse(a);
	((ULONG *)block)[1] = byteReverse(b);
}

template <class T>
inline void GetBlockLittleEndian(const UCHAR *block, T &a, T &b, T &c, T &d)
{
	a = byteReverse(((T *)block)[0]);
	b = byteReverse(((T *)block)[1]);
	c = byteReverse(((T *)block)[2]);
	d = byteReverse(((T *)block)[3]);
}

// Put 4 words back into user's buffer in LITTLE-endian order
template <class T>
inline void PutBlockLittleEndian(UCHAR *block, T a, T b, T c, T d)
{
	((T *)block)[0] = byteReverse(a);
	((T *)block)[1] = byteReverse(b);
	((T *)block)[2] = byteReverse(c);
	((T *)block)[3] = byteReverse(d);
}

class BlockTransformation
{
public:
	///
	virtual ~BlockTransformation() {}

	/// encrypt or decrypt one block in place
	//* Precondition: size of inoutBlock == BlockSize().
	virtual void ProcessBlock(UCHAR *inoutBlock) const =0;

	/// encrypt or decrypt one block, may assume inBlock != outBlock
	//* Precondition: size of inBlock and outBlock == BlockSize().
	virtual void ProcessBlock(const UCHAR *inBlock, UCHAR *outBlock) const =0;

	/// block size of the cipher in bytes
	virtual unsigned int BlockSize() const =0;
};

template <class T> struct SecBlock
{
	explicit SecBlock(unsigned int size=0)
		: size(size) {ptr = SecAlloc(T, size);}
	SecBlock(const SecBlock<T> &t)
		: size(t.size) {ptr = SecAlloc(T, size); memcpy(ptr, t.ptr, size*sizeof(T));}
	SecBlock(const T *t, unsigned int len)
		: size(len) {ptr = SecAlloc(T, len); memcpy(ptr, t, len*sizeof(T));}
	~SecBlock()
		{SecFree(ptr, size);}

	operator const T *() const
		{return ptr;}
	operator T *()
		{return ptr;}

// CodeWarrior defines _MSC_VER
#if !defined(_MSC_VER) || defined(__MWERKS__)
	T *operator +(unsigned int offset)
		{return ptr+offset;}
	const T *operator +(unsigned int offset) const
		{return ptr+offset;}
	T& operator[](unsigned int index)
		{assert(index<size); return ptr[index];}
	const T& operator[](unsigned int index) const
		{assert(index<size); return ptr[index];}
#endif

	const T* Begin() const
		{return ptr;}
	T* Begin()
		{return ptr;}
	const T* End() const
		{return ptr+size;}
	T* End()
		{return ptr+size;}

	void Assign(const T *t, unsigned int len)
	{
		New(len);
		memcpy(ptr, t, len*sizeof(T));
	}

	void Assign(const SecBlock<T> &t)
	{
		New(t.size);
		memcpy(ptr, t.ptr, size*sizeof(T));
	}

	SecBlock& operator=(const SecBlock<T> &t)
	{
		Assign(t);
		return *this;
	}

	bool operator==(const SecBlock<T> &t) const
	{
		return size == t.size && memcmp(ptr, t.ptr, size*sizeof(T)) == 0;
	}

	bool operator!=(const SecBlock<T> &t) const
	{
		return !operator==(t);
	}

	void New(unsigned int newSize)
	{
		if (newSize != size)
		{
			T *newPtr = SecAlloc(T, newSize);
			SecFree(ptr, size);
			ptr = newPtr;
			size = newSize;
		}
	}

	void CleanNew(unsigned int newSize)
	{
		if (newSize != size)
		{
			T *newPtr = SecAlloc(T, newSize);
			SecFree(ptr, size);
			ptr = newPtr;
			size = newSize;
		}
		memset(ptr, 0, size*sizeof(T));
	}

	void Grow(unsigned int newSize)
	{
		if (newSize > size)
		{
			T *newPtr = SecAlloc(T, newSize);
			memcpy(newPtr, ptr, size*sizeof(T));
			SecFree(ptr, size);
			ptr = newPtr;
			size = newSize;
		}
	}

	void CleanGrow(unsigned int newSize)
	{
		if (newSize > size)
		{
			T *newPtr = SecAlloc(T, newSize);
			memcpy(newPtr, ptr, size*sizeof(T));
			memset(newPtr+size, 0, (newSize-size)*sizeof(T));
			SecFree(ptr, size);
			ptr = newPtr;
			size = newSize;
		}
	}

	void Resize(unsigned int newSize)
	{
		if (newSize != size)
		{
			T *newPtr = SecAlloc(T, newSize);
			memcpy(newPtr, ptr, STDMIN(newSize, size)*sizeof(T));
			SecFree(ptr, size);
			ptr = newPtr;
			size = newSize;
		}
	}

	void swap(SecBlock<T> &b);

	unsigned int size;
	T *ptr;
};

// ************** secure memory allocation ***************
#endif // MISC_H
