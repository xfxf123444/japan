#if !defined(CDRLIB_CDTEXT_H)
#define CDRLIB_CDTEXT_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

#define CDTEXT_PACKTYPE_TITLE           0x80
#define CDTEXT_PACKTYPE_PERFORMER       0x81
#define CDTEXT_PACKTYPE_SONGWRITER      0x82
#define CDTEXT_PACKTYPE_COMPOSER        0x83
#define CDTEXT_PACKTYPE_ARRANGER        0x84
#define CDTEXT_PACKTYPE_MESSAGE         0x85
#define CDTEXT_PACKTYPE_DISCID          0x86
#define CDTEXT_PACKTYPE_GENRE           0x87
#define CDTEXT_PACKTYPE_TOC1            0x88
#define CDTEXT_PACKTYPE_TOC2            0x89
#define CDTEXT_PACKTYPE_RESERVED1       0x8A
#define CDTEXT_PACKTYPE_RESERVED2       0x8B
#define CDTEXT_PACKTYPE_RESERVED3       0x8C
#define CDTEXT_PACKTYPE_RESERVED4       0x8D
#define CDTEXT_PACKTYPE_UPCISRC         0x8E
#define CDTEXT_PACKTYPE_SIZEINFO        0x8F

#define CDTEXT_CHARACTER_8859           0
#define CDTEXT_CHARACTER_ASCII          1
#define CDTEXT_CHARACTER_MSJIS          2

#define CDTEXT_COPYPROTECT_OFF          0
#define CDTEXT_COPYPROTECT_ON           3

#define CDTEXT_GENRE_NOTUSED            0
#define CDTEXT_GENRE_UNDEFINED          1
#define CDTEXT_GENRE_ADULT_CONTEMP      2
#define CDTEXT_GENRE_ALTERNATIVE_ROCK   3
#define CDTEXT_GENRE_CHILDRENS          4
#define CDTEXT_GENRE_CLASSICAL          5
#define CDTEXT_GENRE_CONTEMP_CHRISTIAN  6
#define CDTEXT_GENRE_COUNTRY            7
#define CDTEXT_GENRE_DANCE              8
#define CDTEXT_GENRE_EASY_LISTENING     9
#define CDTEXT_GENRE_EROTIC             10
#define CDTEXT_GENRE_FOLK               11
#define CDTEXT_GENRE_GOSPEL             12
#define CDTEXT_GENRE_HIPHOP             13
#define CDTEXT_GENRE_JAZZ               14
#define CDTEXT_GENRE_LATIN              15
#define CDTEXT_GENRE_MUSICAL            16
#define CDTEXT_GENRE_NEWAGE             17
#define CDTEXT_GENRE_OPERA              18
#define CDTEXT_GENRE_OPERETTA           19
#define CDTEXT_GENRE_POP                20
#define CDTEXT_GENRE_RAP                21
#define CDTEXT_GENRE_REGGAE             22
#define CDTEXT_GENRE_ROCK               23
#define CDTEXT_GENRE_RHYTHM_EFFECTS     24
#define CDTEXT_GENRE_SOUND_EFFECTS      25
#define CDTEXT_GENRE_SOUNDTRACK         26
#define CDTEXT_GENRE_SPOKEN_WORD        27
#define CDTEXT_GENRE_WORLD_MUSIC        28

#define CDTEXT_LANGUAGE_UNDEFINED       0
#define CDTEXT_LANGUAGE_CZECH           6
#define CDTEXT_LANGUAGE_DANISH          7
#define CDTEXT_LANGUAGE_GERMAN          8
#define CDTEXT_LANGUAGE_ENGLISH         9
#define CDTEXT_LANGUAGE_SPANISH         10
#define CDTEXT_LANGUAGE_FRENCH          15
#define CDTEXT_LANGUAGE_ITALIAN         21
#define CDTEXT_LANGUAGE_HUNGARIAN       27
#define CDTEXT_LANGUAGE_DUTCH           29
#define CDTEXT_LANGUAGE_NORWEGIEN       30
#define CDTEXT_LANGUAGE_POLISH          32
#define CDTEXT_LANGUAGE_PORTUGUESE      33
#define CDTEXT_LANGUAGE_SLOVENE         38
#define CDTEXT_LANGUAGE_FINNISH         39
#define CDTEXT_LANGUAGE_SWEDISH         40
#define CDTEXT_LANGUAGE_RUSSIAN         86
#define CDTEXT_LANGUAGE_KOREAN          101
#define CDTEXT_LANGUAGE_JAPANESE        105
#define CDTEXT_LANGUAGE_GREEK           112
#define CDTEXT_LANGUAGE_CHINESE         117

// CD-TEXT pack

struct CDTEXTPACK {
  UBYTE type;
  UBYTE track;
  UBYTE seqnum;
  BFLAG charpos : 4;
  BFLAG blocknum : 3;
  BFLAG dbcc : 1;
  char text[12];
  UWORD crc;
  };

//
// CD-TEXT track information class
//

class CDTextTrack {
  public:
    CDTextTrack();
    ~CDTextTrack();
    const CDTextTrack& operator=(const CDTextTrack&);
    void Cleanup();

  public:
    char *titleP;
    char *performerP;
    char *songwriterP;
    char *composerP;
    char *arrangerP;
    char *messageP;
    char *upcisrcP;
  };

//
// CD-TEXT information class
//

class CDTextInfo {
  public:
    CDTextInfo();
    CDTextInfo(const TCHAR *filnamP);
    CDTextInfo(CDTEXTPACK *packvecP, int pack_count);

    ~CDTextInfo();

    UWORD ComputePackCount();
    UWORD Encode(CDTEXTPACK *packvecP);
    void Save(const TCHAR *filnamP);
    void Truncate(int trackcount);

  public:
    int nFirstTrack;
    int nLastTrack;
    int nTrackCount;
    int nGenreCode;
    int nCharacterCode;
    int nLanguageCode;
    char *pDiscId;
    char *pGenre;
    CDTextTrack cdtexttrack[100];

  private:
    void Initialize();
    void Decode(CDTEXTPACK *packvecP, int pack_count);
  };

// CD-TEXT subcode

struct SYMBOLGROUP {
  BFLAG x1 : 6;
  BFLAG : 2;
  BFLAG y1 : 4;
  BFLAG x2 : 2;
  BFLAG : 2;
  BFLAG z1 : 2;
  BFLAG y2 : 4;
  BFLAG : 2;
  BFLAG z2 : 6;
  BFLAG : 2;
  };

struct CDTEXTSUBCODE {
  struct SYMBOLGROUP symbolgroup[6];
  };

// Misc prototypes.

UWORD ComputeCDTextCRC(CDTEXTPACK *packP);
void InitCDTextSubcode(CDTEXTSUBCODE *subcodeP, CDTEXTPACK *packP);
void DumpCDTextPacks(CDTEXTPACK *packvecP, int pack_count);
BOOL ValidateCDTextPacks(CDTEXTPACK *packvecP, int pack_count);

// Restore previous packing alignment.

#pragma pack(pop)

#endif
