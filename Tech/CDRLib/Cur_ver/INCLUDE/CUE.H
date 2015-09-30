#if !defined(CDRLIB_CUE_H)
#define CDRLIB_CUE_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

// "Easy DAO" descriptor blocks.

struct EASYDAOTRACK {
  const TCHAR *filnamP;       // filename
  const TCHAR *titleP;        // track title (CD-TEXT)
  ULONG pregap_blkcnt;        // pregap_blkcnt
  ULONG postgap_blkcnt;       // postgap blkcnt
  UWORD blklen;               // block length
  UBYTE filetype;             // filetype
  UBYTE datatype;             // datatype
  BOOL copy_permitted;        // digital copy permitted flag
  BOOL four_channel;          // four channel audio flag
  BOOL pre_emphasis;          // pre-emphasis flag
  BOOL scms;                  // serial copy management system flag
  BOOL filename_is_title;     // use filename for track title (CD-TEXT)
  BOOL isrc_valid;            // ISRC is valid
  ISRC isrc;                  // ISRC
  };

struct EASYDAO {
  const TCHAR *titleP;        // disc title (CD-TEXT)
  BOOL mcn_valid;             // MCN is valid
  MCN mcn;                    // media catalog number
  UBYTE first_track_number;   // first track number
  UBYTE track_count;          // track count
  UBYTE reserved[1];
  EASYDAOTRACK tracks[100];   // track descriptors
  };

//
// CueSheet class definition.
//

class CueSheet {
  public:
    CueSheet::CueSheet(const TCHAR *filnamP, BOOL log_flag = FALSE);
    CueSheet::CueSheet(EASYDAO *easydaoP, BOOL log_flag = FALSE);
    ~CueSheet();
    QUE *BuildTree();
  public:
    DiscLayout *disclayoutP;
    FILECB *filecb_vec;
    UWORD file_count;
    UWORD track_count;
    UWORD cuepoint_count;
    UWORD reserved;
    ULONG total_file_datalen;
    SLONG disc_high_lba;
  };

//
// Cuesheet tree (used by cuesheet editor)
//

class CueSheetTree {
  public:
    // Default constructor
    CueSheetTree();
  
    // Construct a cuesheet tree from a flat cuesheet
    CueSheetTree(CueSheet *cuesheetP);

    // Destructor.
    ~CueSheetTree();

    int GetFileCount() {return ABSQ_COUNT(&CueFileQhd);}

  private:
    void Cleanup();
    void Initialize();

  public:
    QUE CueFileQhd;             // file node queuehead
    int nFileCount;             // file count

    int nGenreCode;             // CD-TEXT genre code
    int nCharacterCode;         // CD-TEXT character code
    int nLanguageCode;          // CD-TEXT language code
    char *pDiscId;              // CD-TEXT disc identification string
    char *pGenre;               // CD-TEXT genre description string

    CDTextTrack cdtexttrack;    // disc CD-TEXT strings (track zero)
  };

class CueFileNode {
  public:
    CueFileNode(const TCHAR *filnamP, UBYTE filetype);
    ~CueFileNode();

    void Remove();
    int GetTrackCount() {return ABSQ_COUNT(&CueTrackQhd);}

  public:
    QUE NextFileNode;           // next queue entry

    QUE CueTrackQhd;            // track node queuehead
    int nTrackCount;            // track count

    TCHAR *pFilename;           // pointer to filename
    UBYTE nFileType;            // filetype
    UBYTE nReserved[3];
    ULONG nDataOffset;          // data offset in bytes
    ULONG nDataLength;          // data length in bytes
  };

class CueTrackNode {
  public:
    CueTrackNode();
    ~CueTrackNode();

    void Remove();
    int GetIndexCount() {return ABSQ_COUNT(&CueIndexQhd);}

  public:
    QUE NextTrackNode;          // next queue entry

    QUE CueIndexQhd;            // index node queuehead
    int nIndexCount;            // index count

    CDTextTrack cdtexttrack;

    ULONG nDataBlkcnt;          // total data block count
    ULONG nPregapBlkcnt;        // pregap_blkcnt
    ULONG nPostgapBlkcnt;       // postgap blkcnt
    UBYTE nTrack;               // track number
    UBYTE nDatatype;            // datatype
    UWORD nBlklen;              // block length
    BOOL bDataFlag;             // data track
    BOOL bCopyPermitted;        // copy permitted
    BOOL bFourChannel;          // four channel audio
    BOOL bPreEmphasis;          // pre-emphasis flag
    BOOL bIsrcValid;            // ISRC is valid
    ISRC Isrc;                  // ISRC
  };

class CueIndexNode {
  public:
    CueIndexNode(UBYTE index, UBYTE minutes, UBYTE seconds, UBYTE frames);
    ~CueIndexNode();

  void Remove();

  public:
    QUE NextIndexNode;
    UBYTE nIndex;
    UBYTE nMinutes;
    UBYTE nSeconds;
    UBYTE nFrames;
  };

// Restore previous packing alignment.

#pragma pack(pop)

#endif
