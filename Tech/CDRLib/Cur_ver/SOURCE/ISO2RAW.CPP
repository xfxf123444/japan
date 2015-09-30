#if defined(WIN32)
#include <windows.h>
#include <tchar.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <io.h>
#include <dos.h>
#include <malloc.h>
#include <string.h>

#pragma hdrstop

#include <time.h>

#include "datatype.h"
#include "unicode.h"
#include "doswin.h"
#include "misc.h"
#include "prodver.h"
#include "sector.h"

extern unsigned _stklen = 8192;

void PrintUsage(void);
void ParseCommandLine(int argc, char *argv[]);

// Command line parameters.

static char *isofilnam = NULL;
static char *rawfilnam = NULL;

static UBYTE sectortype = SECTORTYPE_MODE1;

static BOOL scramble_flag = FALSE;
static BOOL postgap_flag = FALSE;

void PrintUsage()
{
  printf ("ISO2RAW.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);
  printf ("Usage: ISO2RAW <isofile> <rawfile> [/POSTGAP] [/SCRAMBLE]\n");
  printf ("isofile   - ISO9660 input file (2048 byte sectors)\n");
  printf ("rawfile   - RAW output file (2352 byte sectors)\n");
  printf ("/MODE2    - Generate MODE2 sectors (default is MODE1)\n");
  printf ("/POSTGAP  - Write 150 sector postgap to output file\n");
  printf ("/SCRAMBLE - Scramble sectors\n");
}

void ParseCommandLine(int argc, char *argv[])
{
  int i;
  char *argP, uparg[80];

  // Print usage?

  if (argc == 1) {
    PrintUsage ();
    exit (0);
    }

  for (i = 1; i < argc; i++)
    {
    argP = argv[i];

    StringCopy (uparg, argP);
    strupr (uparg);

    // Is this a switch?

    if (uparg[0] == '/')
      {
      if (! strcmp (uparg, "/?")) {
        PrintUsage ();
        exit (0);
        }

      else if (! stricmp (uparg, "/MODE2"))
        sectortype = SECTORTYPE_MODE2FORM1;

      else if (! stricmp (uparg, "/POSTGAP"))
        postgap_flag = TRUE;

      else if (! stricmp (uparg, "/SCRAMBLE"))
        scramble_flag = TRUE;

      else {
        fprintf (stderr, "Invalid switch - %s\n", argP);
        exit (1);
        }
      }

    // otherwise, it's probably the input or output filename.

    else
      {
      if (isofilnam == NULL)
        isofilnam = argP;
      else if (rawfilnam == NULL)
        rawfilnam = argP;
      else {
        fprintf (stderr, "Too many parameters - %s\n", argP);
        exit (1);
        }
      }
    }

  // Check for required filenames.

  if (isofilnam == NULL) {
    fprintf (stderr, "Error: Input ISO filename must be specified!\n");
    exit (1);
    }

  if (rawfilnam == NULL) {
    fprintf (stderr, "Error: Output RAW filename must be specified!\n");
    exit (1);
    }
}

main(int argc, char *argv[])
{
  #define SECTOR_MODE1_BLKLEN 2048
  #define SECTOR_RAW_BLKLEN   2352

  ULONG filelen, data_blkcnt, blk;
  UWORD cur_percent = 0;

  FILE *isofile, *rawfile;

  SECTORSUBHEADER subheader;

  // Parse the command line arguments.

  ParseCommandLine (argc, argv);

  // Allocate input and output buffers.

  IOBUF *inbufP  = new IOBUF;
  IOBUF *outbufP = new IOBUF;

  // Open the input ISO file.

  if ((isofile = fopen (isofilnam, "rb")) == NULL) {
    fprintf (stderr, "Error opening \"%s\"\n", isofilnam);
    exit (1);
    }

  // Create the output RAW file.

  if ((rawfile = fopen (rawfilnam, "wb")) == NULL) {
    fprintf (stderr, "Error creating \"%s\"\n", rawfilnam);
    exit (1);
    }

  // Get the length of the input file and make sure that it is
  // a multiple of 2048 bytes.

  filelen = filelength (fileno (isofile));

  if (filelen % SECTOR_MODE1_BLKLEN) {
    fprintf (stderr, "Error: Input file length is not a multiple of 2048!\n");
    exit (1);
    }

  data_blkcnt = filelen / SECTOR_MODE1_BLKLEN;

  printf ("File contains %luMb of data (%lu blocks)\n",
    CDIV (filelen, 0x100000), data_blkcnt);

  // Calculate the I/O lengths (base this on the amount of data
  // that can be written from the output buffer).

  UWORD blocks_per_io = sizeof(outbufP->data) / SECTOR_RAW_BLKLEN;

  // Convert the ISO file to a RAW (2352 byte sector) file.

  SLONG lba = 0;
  ULONG blocks_written = 0;

  printf ("Processing...\n");
  printf (" 0%% completed.\r");

  for (blk = 0; blk < data_blkcnt; blk += blocks_per_io)
    {
    UWORD blocks = MIN (blocks_per_io, data_blkcnt - blk);

    // Read the ISO file.

    if (fread (inbufP->data, SECTOR_MODE1_BLKLEN, blocks, isofile) != blocks) {
      fprintf (stderr, "\nUnexpected error reading input file!\n");
      exit (1);
      }

    // If the sector type is MODE2 FORM1, then generate a default subheader.

    if (sectortype == SECTORTYPE_MODE2FORM1)
      {
      MEMCLEAR (&subheader, sizeof(SECTORSUBHEADER));
      subheader.submode.data_flag = TRUE;
      }

    // Convert the blocks to raw CDROM sectors.

    UBYTE *dataP = (UBYTE *)inbufP->data;
    CDSector *sectorP = (CDSector *)outbufP->data;

    for (UWORD b = 0; b < blocks; b++)
      {
      sectorP->Format (
        sectortype, lba, &subheader, dataP, TRUE, FALSE, scramble_flag);

      dataP += SECTOR_MODE1_BLKLEN; sectorP++; lba++;
      }

    // Write the buffer to the output file.

    if (fwrite (outbufP->data, sizeof(CDSector), blocks, rawfile) != blocks) {
      fprintf (stderr, "\nUnexpected error writing output file!\n");
      exit (1);
      }

    // Increment the number of blocks written.

    blocks_written += blocks;

    // Percent complete status change?

    UWORD percent = (blocks_written * 100) / data_blkcnt;

    if (percent > cur_percent) {
      printf (" %u%% completed.\r", percent);
      cur_percent = percent;
      }
    }

  printf ("Conversion completed!\n");

  // Write postgap?

  if (postgap_flag)
    {
    printf ("Writing POSTGAP (150 blocks)...");

    CDSector *sectorP = (CDSector *)outbufP->data;

    for (int i = 0; i < 150; i++)
      {
      sectorP->Format (
        sectortype, lba++, NULL, NULL, TRUE, FALSE, scramble_flag);

      if (fwrite (sectorP, sizeof(CDSector), 1, rawfile) != 1) {
        fprintf (stderr, "\nUnexpected error writing output file!\n");
        exit (1);
        }
      }

    printf ("\n");
    }

  // Close the files.

  fclose (isofile);
  fclose (rawfile);

  // Free the I/O buffers.

  delete inbufP;
  delete outbufP;

  return (0);
}

