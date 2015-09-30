#if !defined(CDRLIB_COPYREC_H)
#define CDRLIB_COPYREC_H

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

struct COPYDISC {
  CDReader *cdreaderP;          // pointer to CDROM device object
  DiscLayout *disclayoutP;      // pointer to disc layout block
  const TCHAR *image_filnamP;   // pointer to image filename
  const TCHAR *cue_filnamP;     // pointer to cuesheet filename
  const TCHAR *cdtext_filnamP;  // pointer to CD-TEXT filename
  UWORD audio_blklen;           // audio block length
  UWORD mode1_blklen;           // mode1 data block length
  UWORD mode2_blklen;           // mode2 data block length
  UBYTE jitter_mode;            // jitter correction mode
  UBYTE image_filetype;         // image filetype (not supported)
  UBYTE audio_speed;            // audio reading speed
  UBYTE data_speed;             // data reading speed
  UBYTE error_mode;             // read error recovery mode
  UBYTE retry_count;            // read error retry count
  BOOL log_flag;                // log flag
  BOOL overwrite_flag;          // overwrite existing files
  };

struct COPYTRACKS {
  CDReader *cdreaderP;          // pointer to CDROM device object
  const TCHAR *image_filnamP;   // pointer to image filename
  UWORD audio_blklen;           // audio block length
  UWORD mode1_blklen;           // mode1 block length
  UWORD mode2_blklen;           // mode2 block length
  UWORD reserved1;
  UBYTE jitter_mode;            // jitter correction mode
  UBYTE scan_subcode_mode;      // subcode scanning mode
  UBYTE image_filetype;         // image filetype
  UBYTE audio_speed;            // audio reading speed
  UBYTE data_speed;             // data reading speed
  UBYTE error_mode;             // read error recovery mode
  UBYTE retry_count;            // read error retry count
  UBYTE reserved2;
  BOOL log_flag;                // log flag
  BOOL seqnum_file_flag;        // sequentially number files
  BOOL overwrite_flag;          // overwrite existing files
  UBYTE tracks[100];            // tracks to copy (add bitmap in future)
  };

struct COPYSECTORS {
  CDReader *cdreaderP;          // pointer to CDROM device object
  const TCHAR *image_filnamP;   // pointer to image filename
  SLONG start_lba;              // starting logical block address
  SLONG end_lba;                // ending logical block address
  UWORD blklen;                 // block length
  UBYTE datatype;               // datatype
  UBYTE jitter_mode;            // jitter correction mode
  UBYTE image_filetype;         // image filetype
  UBYTE audio_speed;            // audio reading speed
  UBYTE data_speed;             // data reading speed
  UBYTE error_mode;             // read error recovery mode
  UBYTE retry_count;            // read error retry count
  UBYTE reserved[3];
  BOOL log_flag;                // log flag
  BOOL overwrite_flag;          // overwrite existing files
  };

// Copy disc-to-disc (for Windows)

struct DISC2DISC {
  DiscLayout *disclayoutP;
  CDReader *cdreaderP;
  CDWriter *cdwriterP;
  UWORD audio_blklen;           // audio block length
  UWORD mode1_blklen;           // mode1 data block length
  UWORD mode2_blklen;           // mode2 data block length
  UBYTE jitter_mode;            // jitter correction mode
  UBYTE audio_speed;            // audio reading speed
  UBYTE data_speed;             // data reading speed
  UBYTE record_speed;           // recording speed
  UBYTE copy_count;             // number of copies
  UBYTE error_mode;             // read error recovery mode
  UBYTE retry_count;            // read error retry count
  UBYTE transporter_type;       // transporter type (SCSI or SERIAL)
  UBYTE transporter_commport;   // transporter comm port (serial type only)
  UBYTE reserved;
  ULONG transporter_baudrate;   // transporter baud rate (serial type only)
  BOOL beep_flag;               // beep when done?
  BOOL eject_flag;              // eject when done?
  BOOL log_flag;                // log progress?
  BOOL multisession_flag;       // open new session?
  BOOL test_flag;               // test write?
  BOOL transporter_flag;        // use disc transporter?
  BOOL underrun_protect_flag;   // enable buffer underrun protection?
  };

// Record disc (for Windows)

struct RECORDDISC {
  void *cuesheetP;
  CDWriter *cdwriterP;
  UBYTE record_speed;
  UBYTE copy_count;
  UBYTE transporter_type;
  UBYTE transporter_commport;
  ULONG transporter_baudrate;
  BOOL raw_flag;
  BOOL beep_flag;
  BOOL eject_flag;
  BOOL log_flag;
  BOOL multisession_flag;
  BOOL test_flag;
  BOOL transporter_flag;
  BOOL underrun_protect_flag;
  };

// Misc prototypes.

int CopyDiscAll(COPYDISC *copydiscP);
int CopyDiscTracks(COPYTRACKS *copytracksP);
int CopyDiscSectors(COPYSECTORS *copysectorsP);
void RecordDisc(RECORDDISC *recorddiscP);

void CopyDiscToDisc(DISC2DISC *disc2discP);

// Restore previous packing alignment.

#pragma pack(pop)

#endif
