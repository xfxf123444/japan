// 13-sep-04 (sjk) UNICODE/MBCS image and cdtext filenames.

#if !defined(CDRLIB_LAYOUT_H)
#define CDRLIB_LAYOUT_H

// Copy disc instruction

#define CI_READ_SECTORS       0
#define CI_GENERATE_SECTORS   1
#define CI_GENERATE_PREGAP    2
#define CI_GENERATE_PREGAP2   3
#define CI_END                0xFF

struct COPYINSTR {
  UBYTE opcode;
  UBYTE datatype;
  UBYTE track_index;
  UBYTE track_count;
  SLONG lba;
  SLONG blkcnt;
  };

// Track information block

struct TRACKINFO {
  SLONG lba;                    // logical block address
  ULONG data_blkcnt;            // total data block count
  ULONG unread_blkcnt;          // unreadable block count
  ULONG part1_pregap_blkcnt;    // part-one pregap block count
  UWORD part1_pregap_blklen;    // part-one pregap block length
  UBYTE part1_pregap_datatype;  // part-one pregap datatype
  UBYTE reserved1;
  ULONG pregap_blkcnt;          // pregap_blkcnt
  ULONG postgap_blkcnt;         // postgap blkcnt
  UWORD blklen;                 // block length
  UBYTE session;                // session number
  UBYTE track;                  // track number
  UBYTE datatype;               // datatype
  UBYTE tracktype;              // tracktype
  UBYTE sectormode;             // sector mode
  UBYTE reserved2;
  BOOL data_flag;               // data track flag
  BOOL copy_permitted;          // digital copy permitted flag
  BOOL four_channel;            // four channel audio flag
  BOOL pre_emphasis;            // pre-emphasis flag
  BOOL scms;                    // serial copy management system flag
  BOOL isrc_valid;              // ISRC is valid
  ISRC isrc;                    // ISRC
  };

// typedef for read data callback

typedef BOOL (READDATACALLBACK)(void *bufP, ULONG length);

// File context block

struct FILECB {
  TCHAR *filnamP;               // pointer to filename

  UBYTE filetype;               // filetype
  UBYTE reserved1[3];

  UQUAD data_length;            // data length in bytes
  ULONG data_offset;            // data offset from beginning of file (in bytes)

  TRACKINFO *trackinfoP;        // pointer to first TRACKINFO block
  UBYTE track_count;            // # of tracks
  UBYTE reserved2[3];

  READDATACALLBACK *callback;   // client callback function
  };

// Cuepoint

struct CUEPOINT {
  UBYTE datatype;   // datatype
  UBYTE track;      // track number (1-99)
  UBYTE index;      // index number (0-99)
  UBYTE minutes;    // minutes      (0-79)
  UBYTE seconds;    // seconds      (0-59)
  UBYTE frames;     // frames       (0-74)
  };

//
// Disc information class
//

class DiscInfo {
  public:
    DiscInfo();

    DiscInfo::DiscInfo(
      CDReader *cdreaderP,
      BOOL data_mode_flag = FALSE,
      BOOL mcn_isrc_flag = FALSE,
      BOOL cdtext_flag = FALSE);

    ~DiscInfo();

    ULONG ComputeDatabaseDiscId();

  public:
    UBYTE m_nFirstTrack;
    UBYTE m_nLastTrack;
    UBYTE m_nTrackCount;
    UBYTE m_nSessionCount;
    SLONG m_nLeadoutLBA;
    ULONG m_nDatabaseDiscId;  // CD Database Disc ID
    BOOL m_bValidMCN;
    MCN m_MCN;
    UBYTE m_nDatabaseGenre;   // CD Database Genre Code
    UWORD m_nCDTextPackCount;
    CDTEXTPACK *m_pCDTextPackVec;
    CDTextInfo *m_pCDTextInfo;
    TRACKINFO m_TrackInfo[100];
  };

//
// Disc layout class
//

class DiscLayout {
  public:
    DiscLayout();
    DiscLayout(
      CDReader *cdreaderP, UBYTE speed, UBYTE scan_subcode_mode,
      BOOL mcn_isrc_flag, BOOL cdtext_flag, BOOL log_flag);
    ~DiscLayout();

    BOOL Check();
    void Display(const TCHAR *filnamP, BOOL prompt_flag, BOOL warn_flag);
    void GenerateCuesheetFile(
      const TCHAR *cue_filnamP, const TCHAR *image_filnamP, const TCHAR *cdtext_filnamP);

  private:
    void AnalyzeAudioToMode1(CDReader *cdreaderP, UBYTE audio_index, UBYTE mode1_index);
    void AnalyzeAudioToMode2(CDReader *cdreaderP, UBYTE audio_index, UBYTE mode2_index);
    void AnalyzeAudioToLeadout(CDReader *cdreaderP, UBYTE audio_index, UBYTE leadout_index);

    void AnalyzeMode1ToAudio(CDReader *cdreaderP, UBYTE mode1_index, UBYTE audio_index);
    void AnalyzeMode1ToMode2(CDReader *cdreaderP, UBYTE mode1_index, UBYTE mode2_index);
    void AnalyzeMode1ToLeadout(CDReader *cdreaderP, UBYTE mode1_index, UBYTE leadout_index);

    void AnalyzeMode2ToAudio(CDReader *cdreaderP, UBYTE mode2_index, UBYTE audio_index);
    void AnalyzeMode2ToMode1(CDReader *cdreaderP, UBYTE mode2_index, UBYTE mode1_index);
    void AnalyzeMode2ToLeadout(CDReader *cdreaderP, UBYTE mode2_index, UBYTE leadout_index);

    void GenerateCuepoints(
      CDReader *cdreaderP, UBYTE speed, UBYTE scan_subcode_mode, BOOL log_flag);

    void AddAudioCuepoints(
      CDReader *cdreaderP, UBYTE track_index, UBYTE track_count,
      UBYTE speed, UBYTE scan_subcode_mode,  BOOL log_flag);

    void AddDataCuepoints(UBYTE track_index, UBYTE track_count);

    void FindPregapCuepoint(
      CDReader *cdreaderP, IOBUF *iobufP, UBYTE track_index, BOOL log_flag);
    void FindSubindexCuepoints(
      CDReader *cdreaderP, IOBUF *iobufP, UBYTE track_index, BOOL log_flag);

  public:
    DiscInfo *m_pDiscInfo;
    SLONG m_nLastLBA;
    UBYTE m_nAudioTrackCount;
    UBYTE m_nMode1TrackCount;
    UBYTE m_nMode2TrackCount;
    UBYTE m_nReserved;
    UWORD m_nCuepointCount;
    UWORD m_nCopyInstrCount;
    CUEPOINT m_Cuepoint[1250];
    COPYINSTR m_CopyInstr[300];
  };

// Misc prototypes.

void InitCuePoint(
  CUEPOINT *cuepointP, UBYTE track, UBYTE index,
  UBYTE minutes, UBYTE seconds, UBYTE frames);

void InitCuePoint(
  CUEPOINT *cuepointP, UBYTE track, UBYTE index, SLONG lba);

ULONG CuePointLength(CUEPOINT *cuepoint1P, CUEPOINT *cuepoint2P);
  
CUEPOINT *FindCuePoint(CUEPOINT *cuepoint_vec, UBYTE track);

UBYTE BuildControlFlags(TRACKINFO *trackinfoP);

#endif
