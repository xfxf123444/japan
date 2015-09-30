#ifndef CRYPTOPP_ZDEFLATE_H
#define CRYPTOPP_ZDEFLATE_H

#include "cryptlib.h"
#include "misc.h"
#include "filters.h"
#include "ztrees.h"

NAMESPACE_BEGIN(CryptoPP)

class Deflator : public CodeTree
{
public:
	// deflateLevel can be from 0 to 9, 0 being store, 1 being fastest, 9 being most compression
	// default for the gzip program is 6
	// log2WindowSize controls how much memory is used, can be from 8 to 15
	enum {DEFAULT_DEFLATE_LEVEL = 6, DEFAULT_LOG2_WINDOW_SIZE = 15};
	Deflator(BufferedTransformation *outQ=NULL, unsigned int deflateLevel=DEFAULT_DEFLATE_LEVEL, unsigned int log2WindowSize=DEFAULT_LOG2_WINDOW_SIZE);

	void Put(byte inByte)
		{Deflator::Put(&inByte, 1);}
	void Put(const byte *inString, unsigned int length);

	void Flush(bool completeFlush, int propagation=-1);
	void MessageEnd(int propagation=-1);

	unsigned int GetLog2WindowSize() const {return HASH_BITS;}
	unsigned int GetDeflateLevel() const {return deflate_level;}

private:
	const unsigned int HASH_BITS, HASH_SIZE, HASH_MASK, WINDOW_SIZE, WMASK;

	enum {NIL = 0,  // Tail of hash chains
		  // Matches of length 3 are discarded if their distance exceeds TOO_FAR
		  TOO_FAR = 4096};

	struct config
	{
	   word16 good_length; /* reduce lazy search above this match length */
	   word16 max_lazy;    /* do not perform lazy search above this match length */
	   word16 nice_length; /* quit search above this match length */
	   word16 max_chain;
	};

	static const config configuration_table[10];

	typedef word16 Pos;
	typedef unsigned IPos;

	SecByteBlock window;
	SecBlock<Pos> prev, head;

	unsigned fill_window (const byte*, unsigned);
	void     init_hash   ();

	int longest_match (IPos cur_match);

	int fast_deflate(const byte *buffer, unsigned int length);
	int lazy_deflate(const byte *buffer, unsigned int length);

	unsigned ins_h;  /* hash index of string to be inserted */
	char uptodate;   /* hash preparation flag */

	unsigned int prev_length;
	/* Length of the best match at previous step. Matches not greater than this
	 * are discarded. This is used in the lazy match evaluation. */

	unsigned match_start; /* start of matching string */
	unsigned lookahead;   /* number of valid bytes ahead in window */
	unsigned minlookahead;

	unsigned max_chain_length;
	/* To speed up deflation, hash chains are never searched beyond this length.
	 * A higher limit improves compression ratio but degrades the speed. */

	unsigned int max_lazy_match;
	/* Attempt to find a better match only when the current match is strictly
	 * smaller than this value. This mechanism is used only for compression
	 * levels >= 4. */

	unsigned good_match;
	/* Use a faster search when the previous match is longer than this */
	int nice_match; /* Stop searching when current match exceeds this */

	/* A block of local deflate process data to be saved between
	 * sequential calls to deflate functions */
	int match_available; /* set if previous match exists */
	unsigned match_length; /* length of best match */

	bool m_eof;
};

NAMESPACE_END

#endif
