// 08-Sep-04 (sjk) Support new CDFSOPTIONS: iso9660_character_set and iso9660_level.

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

#include <new.h>

#include "datatype.h"
#include "unicode.h"
#include "misc.h"
#include "doswin.h"
#include "dirtree.h"
#include "cdfs.h"
#include "iso9660.h"
#include "event.h"
#include "except.h"
#include "protect.h"
#include "prodver.h"

extern unsigned _stklen = 32768;

// Function prototypes.

void PrintUsage(void);
void ParseCommandLine(int argc, char *argv[]);

// Command line parameters.

static char *pathname = NULL;
static char *image_filnam = NULL;

static char volume_id[64] = {0};

static UBYTE cdfs_type = CDFS_TYPE_ISO9660;
static UBYTE iso9660_level = CDFS_ISO9660_LEVEL1;
static UBYTE iso9660_character_set = CDFS_ISO9660_CHARSET_DOS;

static BOOL archive_only_flag = FALSE;
static BOOL confirm_flag = TRUE;
static BOOL dvdsort_flag = FALSE;
static BOOL fullpath_flag = FALSE;
static BOOL hidden_flag = FALSE;
static BOOL joliet_flag = FALSE;
static BOOL log_flag = TRUE;
static BOOL nounicode_flag = FALSE;
static BOOL noversion_flag = FALSE;
static BOOL raw_flag = FALSE;
static BOOL recurse_flag = FALSE;
static BOOL system_flag = FALSE;

//
// Print command usage.
// 

void PrintUsage()
{
  printf ("MAKEISO.EXE - Version %s (%s)\n", PRODUCT_VERSION, PRODUCT_RELEASE_DATE);
  printf ("%s\n\n", COPYRIGHT_MESSAGE);
  printf ("Usage: MAKEISO <pathname or @listfile> <imagefile> [/BATCH] [/CHARSET=type]\n");
  printf ("         [/DVDSORT] [/FILESYSTEM=type] [/FULLPATH] [/JOLIET] [/LEVEL2]\n");
  printf ("         [/NOCONFIRM] [/NOUNICODE] [/NOVERSION] [/RAW] [/RECURSE]\n");
  printf ("         [/ARCHIVE] [/HIDDEN] [/SYSTEM] [/VOLUME=label]\n");
  printf ("pathname    - Directory containing the files to process\n");
  printf ("@listfile   - File that contains a list of directories to process\n");
  printf ("imagefile   - Output image filename\n");
  printf ("/BATCH      - Disable all messages and confirmation prompts\n");
  printf ("/CHARSET    - Name translation character set (ISO9660, DOS, or ASCII)\n");
  printf ("/DVDSORT    - Sort filenames in DVD-Video compatible order\n");
  printf ("/FILESYSTEM - CDROM filesystem type (ISO9660, UDF, UDFISO)\n");
  printf ("/FULLPATH   - Preserve full directory pathnames (default is FALSE)\n");
  printf ("/JOLIET     - Enable Joliet long filename support (32-bit version only)\n");
  printf ("/LEVEL2     - Enable ISO9660 Level 2 filenames (maximum 31 characters))\n");
  printf ("/NOCONFIRM  - Disable all confirmation prompts\n");
  printf ("/NOUNICODE  - Disable Unicode format for UDF filesystem\n");
  printf ("/NOVERSION  - Disable filename version numbers\n");
  printf ("/RAW        - Generate raw data sectors\n");
  printf ("/RECURSE    - Recurse all subdirectories\n");
  printf ("/ARCHIVE    - Include \"archive\" files only\n");
  printf ("/HIDDEN     - Include \"hidden\" files\n");
  printf ("/SYSTEM     - Include \"system\" files\n");
  printf ("/VOLUME     - Volume label (maximum 32 characters)\n");
  printf ("e.g. MAKEISO C:\\MONDAY\\ TEST.ISO /RECURSE /VOLUME=MY_FILES\n");
  printf ("e.g. MAKEISO C:\\FRIDAY\\ TEST.ISO /JOLIET /FILESYSTEM=UDFISO\n");
  printf ("e.g. MAKEISO @DIRLIST.TXT TEST.ISO /FILESYSTEM=UDF /CHARSET=DOS\n");
}

//
// Parse the command line.
//

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

      else if (! strcmp (uparg, "/BATCH")) {
        log_flag = FALSE;
        confirm_flag = FALSE;
        }

      else if (! strncmp (uparg, "/CHARSET", 8))
        {
        char charset_str[80];

        if (sscanf (uparg, "/CHARSET=%s", &charset_str) != 1) {
          fprintf (stderr, "Error: Illegal /CHARSET format!\n");
          exit (1);
          }

        if (! stricmp(charset_str, "ISO9660"))
          iso9660_character_set = CDFS_ISO9660_CHARSET_STANDARD;
        else if (! stricmp(charset_str, "DOS"))
          iso9660_character_set = CDFS_ISO9660_CHARSET_DOS;
        else if (! stricmp(charset_str, "ASCII1"))
          iso9660_character_set = CDFS_ISO9660_CHARSET_ASCII;
        else
          {
          fprintf (stderr, "Illegal /CHARSET option \"%s\" specified!\n", charset_str);
          exit (1);
          }
        }

      else if (! strncmp (uparg, "/FILESYSTEM", 11))
        {
        char cdfs_str[80];

        if (sscanf (uparg, "/FILESYSTEM=%s", &cdfs_str) != 1) {
          fprintf (stderr, "Error: Illegal /FILESYSTEM format!\n");
          exit (1);
          }

        if (! stricmp(cdfs_str, "ISO9660"))
          cdfs_type = CDFS_TYPE_ISO9660;
        else if (! stricmp(cdfs_str, "UDFISO"))
          cdfs_type = CDFS_TYPE_UDFBRIDGE;
        else if (! stricmp(cdfs_str, "UDF"))
          cdfs_type = CDFS_TYPE_UDF;
        else
          {
          fprintf (stderr, "Illegal /FILESYSTEM option \"%s\" specified!\n", cdfs_str);
          exit (1);
          }
        }

      else if (! strcmp (uparg, "/FULLPATH"))
        fullpath_flag = TRUE;

      else if (! strcmp (uparg, "/JOLIET"))
        {
        #if defined(WIN32)
          joliet_flag = TRUE;
        #else
          fprintf (stderr, "Error: /JOLIET option is not allowed for 16-bit DOS.\n");
          exit (1);
        #endif
        }

      else if (! strcmp (uparg, "/LEVEL2"))
        iso9660_level = CDFS_ISO9660_LEVEL2;

      else if (! strcmp (uparg, "/NOCONFIRM"))
        confirm_flag = FALSE;

      else if (! strcmp (uparg, "/NOUNICODE"))
        nounicode_flag = FALSE;

      else if (! strcmp (uparg, "/NOVERSION"))
        noversion_flag = TRUE;

      else if (! strcmp (uparg, "/RAW"))
        raw_flag = TRUE;

      else if (! strcmp (uparg, "/RECURSE"))
        recurse_flag = TRUE;

      else if (! strcmp (uparg, "/ARCHIVE"))
        archive_only_flag = TRUE;

      else if (! strcmp (uparg, "/HIDDEN"))
        hidden_flag = TRUE;

      else if (! strcmp (uparg, "/SYSTEM"))
        system_flag = TRUE;

      else if (! strcmp (uparg, "/DVDSORT"))
        dvdsort_flag = TRUE;

      else if (! strncmp (uparg, "/VOLUME", 7))
        {
        if (sscanf (uparg, "/VOLUME=%s", volume_id) != 1) {
          fprintf (stderr, "Error: Illegal /VOLUME format!\n");
          exit (1);
          }

        if (strlen(volume_id) > 32) {
          fprintf (stderr, "Error: Volume label is too long!\n");
          exit (1);
          }

        // Uppercase the volume ID.

        strupr (volume_id);
        }

      else {
        fprintf (stderr, "Invalid switch - %s\n", argP);
        exit (1);
        }
      }

    // otherwise, it's probably one of the parameters.

    else
      {
      if (pathname == NULL)
        pathname = argP;
      else if (image_filnam == NULL)
        image_filnam = argP;
      else {
        fprintf (stderr, "Too many parameters - %s\n", argP);
        exit (1);
        }
      }
    }

  // Check for required parameters.

  if (pathname == NULL) {
    fprintf (stderr, "Error: Filename must be specified!\n");
    exit (1);
    }

  if (image_filnam == NULL) {
    fprintf (stderr, "Error: ISO filename must be specified!\n");
    exit (1);
    }
}

//
// Main entry point.
//

main(int argc, char *argv[])
{
  auto_ptr<DirectoryTree> dirtreeP;

  // Set the memory allocation failure handler.

  #if defined(WIN32)
  _set_new_handler (ConsoleNewHandler);
  #endif

  // Enable exception handling.

  EXCEPTION_HANDLER_START

  // Parse the command line arguments.

  ParseCommandLine (argc, argv);

  // Register the event callback function.

  EventRegisterCallback (ConsoleEventCallback);

  // Build the directory tree for the specified pathnames.

  if (pathname[0] != '@')
    dirtreeP.reset(new DirectoryTree (
      pathname, NULL, fullpath_flag, recurse_flag,
      hidden_flag, system_flag, archive_only_flag));
  else
    {
    FILE *file;
    char dirname[256];

    // Open the list file.

    if ((file = fopen (&pathname[1], "r")) == NULL) {
      fprintf (stderr, "Error: Unable to open file \"%s\"\n", &pathname[1]);
      exit (1);
      }

    // Build the directory path descriptor list.

    int dircount = 0;

    DIRPATHDESC *pathdesc_vec = new DIRPATHDESC[100];
    DIRPATHDESC *descP = pathdesc_vec;

    while (fgets(dirname, 256, file) != NULL)
      {
      char dummy[256];

      // Skip blank lines.

      int status = sscanf (dirname, "%s", dummy);
      if ((status == 0) || (status == EOF)) continue;

      // Too many directories?

      if (dircount == 100) {
        fprintf (stderr, "Error: Too many directories/files specified\n");
        exit (1);
        }

      // Strip off the line terminator.

      int dirname_len = strlen(dirname) - 1;
      dirname[dirname_len] = '\0';

      // Build the directory path descriptor.

      char *pathnameP = new char[dirname_len + 1];
      StringCopy (pathnameP, dirname);

      descP->pathnameP = pathnameP;
      descP->out_pathnameP = NULL;
      descP->fullpath_flag = fullpath_flag;
      descP->recurse_flag = recurse_flag;
      descP->hidden_flag = hidden_flag;
      descP->system_flag = system_flag;
      descP->archive_only_flag = archive_only_flag;
      
      dircount++;
      descP++;
      }

    // Close file.

    fclose (file);

    // No directories specified?

    if (dircount == 0) {
      fprintf (stderr, "Error: File contains no directory names\n");
      exit (1);
      }

    // Build the directory tree.

    dirtreeP.reset(new DirectoryTree (pathdesc_vec, dircount));

    // Free the directory path descriptors.

    for (int i = 0; i < dircount; i++)
      delete []((char *)pathdesc_vec[i].pathnameP);

    delete []pathdesc_vec;
    }

  // Display the directory/file statistics?

  if (log_flag)
    {
    printf ("Directory/File Statistics...\n");
    printf ("  Directory Count         = %u\n", dirtreeP->nDirCount);
    printf ("  Maximum Directory Depth = %u\n", dirtreeP->nMaxDirDepth);
    printf ("  File Count              = %u\n", dirtreeP->nFileCount);
    printf ("  Required Disc Space     = %lu sectors\n", dirtreeP->nTotalDiscBlkcnt);
    printf ("\n");
    }
  
  // Prompt user to begin?

  if (confirm_flag)
    {
    printf ("Hit <ENTER> to build image file (or CTRL/C to exit)...");
    getchar();
    printf ("\n");
    }

  // Initialize the CDFS image options.

  CDFSOPTIONS cdfs_options;
  MEMCLEAR (&cdfs_options, sizeof(CDFSOPTIONS));

  cdfs_options.filesystem_type = cdfs_type;
  cdfs_options.iso9660_joliet_flag = joliet_flag;
  cdfs_options.iso9660_no_version_numbers = noversion_flag;
  cdfs_options.iso9660_filename_level = iso9660_level;
  cdfs_options.iso9660_character_set = iso9660_character_set;
  cdfs_options.udf_filesystem_format =
    (nounicode_flag ? CDFS_UDF_FORMAT_ASCII : CDFS_UDF_FORMAT_UNICODE);
  cdfs_options.udf_filesystem_version = CDFS_UDF_VERSION_102;
  cdfs_options.file_sort_option = CDFS_FILESORT_NAME;
  cdfs_options.sort_dvdvideo_compatible = dvdsort_flag;
  cdfs_options.file_date_option = CDFS_FILEDATE_ORIGINAL;

  StringCopy (cdfs_options.volume_id, volume_id);

  // Build the CDFS image file.

  CDFSBuildImageFile (
    image_filnam, dirtreeP.get(), &cdfs_options, 0, raw_flag, TRUE, log_flag);

  // End exception handling.

  EXCEPTION_HANDLER_EXIT

  return (0);
}
