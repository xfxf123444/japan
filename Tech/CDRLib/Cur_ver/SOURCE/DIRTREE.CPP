// 06-sep-04 (sjk) Changes.

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

#include <ctype.h>

#include "datatype.h"
#include "unicode.h"
#include "dirtree.h"
#include "misc.h"
#include "except.h"
#include "event.h"
#include "cdrlib.h"
#include "udf.h"
#include "iso9660.h"

// 1 byte alignment throughout this file.

#pragma pack(push, 1)

// Statics.

static int gSortFileField = CDFS_FILESORT_NAME;

static BOOL gbSortCaseInsensitive = FALSE;
static BOOL gbSortDescending = FALSE;
static BOOL gbSortDirsFirst = FALSE;
static BOOL gbSortDVDVideoCompatible = FALSE;

static BOOL gbSortLongNames = FALSE;
static BOOL gbSortDirNodesOnly = FALSE;

// Local prototypes.

static void GenerateUniqueISO9660Name(
  DIRNODE *dirP, FILENODE *filenodeP, TCHAR *outnameP,
  int iso9660_level, int iso9660_character_set);

static void GenerateUniqueJolietName(
  DIRNODE *dirP, FILENODE *filenodeP, BOOL allow_max_filename_len, TCHAR *outnameP);

static BOOL IsUniqueISO9660Name(DIRNODE *dirP, FILENODE *filenodeP, const TCHAR *nameP);
static BOOL IsUniqueJolietName(DIRNODE *dirP, FILENODE *filenodeP, const TCHAR *nameP);

static TCHAR **GetCharTranslationTable(int iso9660_level, int iso9660_character_set);

static BOOL TranslateName(
  const TCHAR *in_nameP, TCHAR *out_nameP, TCHAR **xlatePAP);

static int IsReplaceable(
  FILENODE *hd_fileP, FILENODE *cd_fileP, int duplicate_file_opt);

static int CompareNames(const TCHAR *name1, const TCHAR *name2);

static int CompareNamesDVD(
  const TCHAR *name1, const TCHAR *name2, const TCHAR *ext1, const TCHAR *ext2);

static int CompareDirectoryNodes(const void *rr, const void *ll);
static int CompareFileNodes(const void *rr, const void *ll);

static void SortSubdirQueue(QUE *subdir_qhd);
static void SortFileQueue(QUE *file_qhd);

static const TCHAR *SplitDevPath(const TCHAR *dirP);

// --------------------------------------------------------------------------------------------
//
// Directory tree constructors and destructor
//
// --------------------------------------------------------------------------------------------

DirectoryTree::DirectoryTree()
{
  Initialize();
}

DirectoryTree::DirectoryTree(DIRPATHDESC *pathdesc_vec, int desc_count)
{
  // Build the directory tree.
  // If an exception occurs, then cleanup and rethrow the exception.

  try
    {
    Build (pathdesc_vec, desc_count);
    }
  catch (...)
    {
    Cleanup();
    throw;
    }
}

DirectoryTree::DirectoryTree(
  TCHAR *pathnameP, TCHAR *out_pathnameP, BOOL fullpath_flag,
  BOOL recurse_flag, BOOL hidden_flag, BOOL system_flag, BOOL archive_only_flag)
{
  DIRPATHDESC pathdesc;

  // Initialize a directory path descriptor.

  pathdesc.pathnameP = pathnameP;
  pathdesc.out_pathnameP = out_pathnameP;
  pathdesc.fullpath_flag = fullpath_flag; 
  pathdesc.recurse_flag = recurse_flag;
  pathdesc.hidden_flag = hidden_flag;
  pathdesc.system_flag = system_flag;
  pathdesc.archive_only_flag = archive_only_flag;

  // Build the directory tree.
  // If an exception occurs, then cleanup and rethrow the exception.

  try
    {
    Build (&pathdesc, 1);
    }
  catch (...)
    {
    Cleanup();
    throw;
    }
}

DirectoryTree::~DirectoryTree()
{
  Cleanup();
}

//
// Directory tree initialization
//

void DirectoryTree::Initialize()
{
  // Dummy queues in DirectoryTree contain only one entry each - for the root directory.

  ABSQ_INIT (&DummySubdirQhd);
  ABSQ_INIT (&DummyFileQhd);

  // Initialize the pathname pointer vector.

  pPathnameVec = new TCHAR*[256];
  nPathnameVecCountMax = 256;
  nPathnameVecCountCur = 0;

  // Initialize the statistics.

  nDirCount = 0;
  nFileCount = 0;
  
  nMaxDirDepth = 0;

  nTotalNewDiscBlkcnt = 0;
  nTotalImportDiscBlkcnt = 0;
  nTotalDiscBlkcnt = 0;

  // Allocate the root file and directory nodes (we need to build the root file
  // node by hand since the file doesn't really exist on disk).

  FILEINFO finfo;
  MEMCLEAR (&finfo, sizeof(FILEINFO));

  SYSDATETIME datetime;
  GetDateTime (&datetime);

  finfo.creation_datetime = datetime;
  finfo.modified_datetime = datetime;
  finfo.accessed_datetime = datetime;

  finfo.dir_flag = TRUE;
  StrCpy (finfo.name, _T("."));

  FILENODE *filenodeP = new FILENODE(AddPathname(_T("")), &finfo, &DummyFileQhd);

  pRootDirNode = new DIRNODE (NULL, filenodeP, 1, &DummySubdirQhd);
}

//
// DirectoryTree cleanup function
//

void DirectoryTree::Cleanup()
{
  // Recursively delete root DIRNODE and its children DIRNODEs and FILENODEs.
  delete pRootDirNode;

  // Delete all cached pathname strings.
  for (UWORD i = 0; i < nPathnameVecCountCur; i++)
    delete []pPathnameVec[i];

  // Delete the pathname pointer vector itself.
  delete []pPathnameVec;

  // Set the pathname vector count to zero.
  nPathnameVecCountCur = 0;
}

//
// Build a directory tree from one or more input directory descriptors.
//

void DirectoryTree::Build(DIRPATHDESC *pathdesc_vec, int desc_count)
{
  // Initialize the tree object.

  Initialize();

  // Loop through the directory descriptors...
  // If this is the first directory descriptor, then build our own directory tree.
  // Otherwise, build a temporary tree and merge it with our existing tree.

  for (int i = 0; i < desc_count; i++)
    {
    if (i == 0)
      Build(&pathdesc_vec[i]);
    else
      {
      DirectoryTree *temptreeP = new DirectoryTree (&pathdesc_vec[i], 1);
      Merge (temptreeP, DIRECTORY_MERGE, FILE_REPLACE_CHANGED, FALSE);
      delete temptreeP;
      }
    }

  // Collect statistics on the tree.

  CollectStatistics();
}

//
// Build one directory tree, given a set of input parameters.
//

void DirectoryTree::Build(DIRPATHDESC *descP)
{
  TCHAR full_pathname[_MAX_PATH + 1];
  TCHAR drivedir[_MAX_DIR + 1];
  TCHAR filename[_MAX_FNAME + 1];

  // Make sure we have an absolute pathname.

  if (! FullPath (descP->pathnameP, full_pathname, _MAX_PATH + 1))
    SignalException (E_BadPathname, 1, 0, 0, HeapString(descP->pathnameP));

  // If the pathname ends with a backslash, then add "*.*" to it.

  if (LastCharacterMatches (full_pathname, '\\')) StrCat (full_pathname, _T("*.*"));
  
  // Do we have a valid pathname?

  switch (PathnameExists(full_pathname))
    {
    case PATH_IS_FILE:
      {
      // Build separate directory and filename strings.

      TCHAR *lastbsP = StrRChr(full_pathname, '\\');

      if (lastbsP == NULL)
        SignalException (E_BadPathname, 1, 0, 0, HeapString(full_pathname));

      // Copy the dev+dir path, including the backslash.
      memcpy (drivedir, full_pathname, (lastbsP - full_pathname + 1) * sizeof(TCHAR));
      drivedir[lastbsP - full_pathname + 1] = '\0';

      // Copy the filename+ext (skip over the backslash).
      StrCpy (filename, lastbsP + 1);

      break;
      }

    case PATH_IS_DIR:
      {
      // Add a trailing backslash to the directory pathname.
      StrCpy (drivedir, full_pathname);
      StrCat (drivedir, _T("\\"));

      // Process all files in the directory.
      StrCpy (filename, _T("*.*"));

      break;
      }

    case PATH_IS_BAD:
      {
      // Pathname doesn't exist.
      SignalException (E_BadPathname, 1, 0, 0, HeapString(descP->pathnameP));
      break;
      }
    }

  // If we are preserving the full directory path or a new output directory path has
  // been specified, then create directory nodes for each component of the path.

  DIRNODE *dirnodeP;

  if (descP->out_pathnameP != NULL)
    {
    TCHAR fakepath[_MAX_PATH + 1];

    // If the new output pathname doesn't end with a backslash, then add one.

    StrCpy (fakepath, descP->out_pathnameP);
    if (! LastCharacterMatches(fakepath, '\\') ) StrCat(fakepath, _T("\\"));

    dirnodeP = AddDirectoryPathNodes (fakepath, FALSE);
    }
  else if (descP->fullpath_flag)
    dirnodeP = AddDirectoryPathNodes (drivedir, TRUE);
  else
    dirnodeP = pRootDirNode;

  // Call the recursive tree builder.

  BuildRecurse (
    dirnodeP, drivedir, filename, descP->recurse_flag,
    descP->hidden_flag, descP->system_flag, descP->archive_only_flag);
}

//
// Recursive tree builder
//

void DirectoryTree::BuildRecurse(
  DIRNODE *dirnodeP, const TCHAR *pathnameP, const TCHAR *filenameP,
  BOOL recurse_flag, BOOL hidden_flag, BOOL system_flag, BOOL archive_only_flag)
{
  int status;

  auto_ptr<FILEINFO> finfoP(new FILEINFO);
  auto_ptr<PATHNAME> full_pathnameP(new PATHNAME);

  // Add the pathname to the cache.

  TCHAR *cached_pathP = AddPathname (pathnameP);

  // Construct the full path plus filename string.

  StrCpy (full_pathnameP->name, cached_pathP);
  StrCat (full_pathnameP->name, filenameP);

  // Start the file search (if this fails, then there are no files to process).

  void *handleP = FileFindFirst (full_pathnameP->name, finfoP.get());
  if (handleP == NULL) return;

  // Process all files in the directory...

  do
    {
    // Skip all "." and ".." entries.

    if (StrICmp(finfoP->name, _T(".")) && StrICmp(finfoP->name, _T("..")))
      {
      FILENODE *filenodeP;

      // Insert this file into the tree?

      if (((! finfoP->dir_flag) || recurse_flag) &&
          ((! finfoP->hidden_flag) || hidden_flag) &&
          ((! finfoP->system_flag) || system_flag) &&
          (finfoP->dir_flag || finfoP->archive_flag || (! archive_only_flag)))
        {
        filenodeP = new FILENODE(cached_pathP, finfoP.get(), &dirnodeP->file_qhd);

        // A visible directory, and we're recursing?

        if (finfoP->dir_flag && recurse_flag)
          {
          // Allocate a new directory node and insert it into the subdirectory queue.

          DIRNODE *newdirP = new DIRNODE (
            dirnodeP, filenodeP, (UBYTE)(dirnodeP->depth + 1), &dirnodeP->subdir_qhd);

          // Build the new pathname (based on the shortname of the directory) and recurse.

          auto_ptr<PATHNAME> newpathP(new PATHNAME);

          StrCpy (newpathP->name, cached_pathP);
          StrCat (newpathP->name, finfoP->name);
          StrCat (newpathP->name, _T("\\"));

          BuildRecurse (
            newdirP, newpathP->name, _T("*.*"), recurse_flag,
            hidden_flag, system_flag, archive_only_flag);
          }
        }
      }

    // Find the next file in the directory.

    status = FileFindNext (handleP, finfoP.get());
    }
  while (status);

  // End the file search.

  FileFindDone (handleP);
}

//
// Add or find a cached pathname string
//

TCHAR *DirectoryTree::AddPathname(const TCHAR *pathnameP)
{
  // See if the pathname is already in the cache.

  for (UWORD i = 0; i < nPathnameVecCountCur; i++)
    {if (StrICmp(pathnameP, pPathnameVec[i]) == 0) return (pPathnameVec[i]);}

  // The pathname was not found, so we need to add it to the table.
  // If necessary, make the table larger.

  if (nPathnameVecCountCur == nPathnameVecCountMax)
    {
    TCHAR **new_pathvecPP = new TCHAR*[nPathnameVecCountMax * 2];
    memcpy (new_pathvecPP, pPathnameVec, sizeof(TCHAR *) * nPathnameVecCountMax);
    delete []pPathnameVec;
    pPathnameVec = new_pathvecPP;
    nPathnameVecCountMax *= 2;
    }

  // Add a clone of the pathname to the table and return a pointer to it.

  TCHAR *newpathP = CloneString (pathnameP, TRUE);
  pPathnameVec[nPathnameVecCountCur++] = newpathP;

  return (newpathP);
}

//
// Returns a pointer to the leaf directory node for the specified path.
// Adds directory nodes for each component of the pathname, as needed.
// The input pathname MUST end with a backslash.
// Consider at least these cases:
//
//    "c:\"
//    "\\server\c\"
//
//    "c:\foo\bar\"
//    "\\server\c\foo\bar\"
//

DIRNODE *DirectoryTree::AddDirectoryPathNodes(const TCHAR *pathnameP, BOOL real_path_flag)
{
  // Find the first character (not backslash) of the directory path.

  const TCHAR *dirP = SplitDevPath(pathnameP);                                

  // Create entries for each directory in the path (if they don't already exist).

  DIRNODE *dirnodeP = pRootDirNode;             // start with the root directory node

  LOOP
    {
    if (dirP == NULL) break;                    // could not parse original path

    const TCHAR *nextbsP = StrChr (dirP, '\\'); // Search for the next backslash character
    if (nextbsP == NULL) break;                 // no more backslashes - we're done

    // Get File Info about this subdirectory file.
    // If the path should actually exist, then lookup the real file information.
    // otherwise, just make up the information about this directory file.

    FILEINFO finfo;

    if (real_path_flag)
      {
      TCHAR curpath[_MAX_PATH + 1];

      // Make a copy of the path up to this point, without the trailing backslash.
      // (This is the filename of the directory file.)  Get real File Info.

      memcpy (curpath, pathnameP, (nextbsP - pathnameP) * sizeof(TCHAR));
      curpath[nextbsP - pathnameP] = '\0';

      void *handleP = FileFindFirst (curpath, &finfo);
      FileFindDone (handleP);

      if ((handleP == NULL) || (! finfo.dir_flag))
        SignalException (E_BadPathname, 1, 0, 0, HeapString(curpath));
      }
    else
      {
      TCHAR dirname[_MAX_PATH + 1];
      SYSDATETIME datetime;

      MEMCLEAR (&finfo, sizeof(FILEINFO));

      // Make a copy of the current directory name between the two backslashes.

      memcpy (dirname, dirP, (nextbsP - dirP) * sizeof(TCHAR));   // copy bytes
      dirname[nextbsP - dirP] = '\0';                             // Z-terminate (TCHAR)

      StrCpy (finfo.name, dirname);

      // Set the directory flag
      finfo.dir_flag = TRUE;

      // Set the timestamps.
      GetDateTime (&datetime);
      finfo.creation_datetime = datetime;
      finfo.modified_datetime = datetime;
      finfo.accessed_datetime = datetime;
      }

    // If this directory node doesn't exist in the tree, then add it...

    DIRNODE *nextdirP = NULL;

    ABSQ_LOOP (&dirnodeP->subdir_qhd, dup_dirnodeP, DIRNODE, nextdir)
      {
      if (StrICmp (dup_dirnodeP->parent_filenodeP->src_nameP, finfo.name) == 0)
        {
        nextdirP = dup_dirnodeP;
        break;
        }
      }
    ABSQ_POOL

    if (nextdirP == NULL)
      {
      TCHAR curpath[_MAX_PATH + 1];

      // Make of copy of the full path to this point (including trailing backslash).
      // Add it to the pathname cache and create the new subdirectory node.

      memcpy (curpath, pathnameP, (nextbsP - pathnameP + 1) * sizeof(TCHAR));
      curpath[nextbsP - pathnameP] = '\0';

      // Create the new file node.

      FILENODE *new_fileP = new FILENODE (AddPathname(curpath), &finfo, &dirnodeP->file_qhd);

      // Create the new subdirectory node and point to it.

      nextdirP = new DIRNODE (
        dirnodeP, new_fileP, (UBYTE)(dirnodeP->depth + 1), &dirnodeP->subdir_qhd);
      }

    dirnodeP = nextdirP;        // point to the current directory node
    dirP = nextbsP + 1;         // nextbsP was pointing to a backslash - skip over it (MBCS-safe)
    }

  // Return a pointer to the terminal directory node.

  return (dirnodeP);
}

// --------------------------------------------------------------------------------------------
//
// Generate Destination Names
//
// --------------------------------------------------------------------------------------------

void DirectoryTree::GenerateDestinationNames(
  int iso9660_level, int iso9660_character_set, BOOL allow_max_filename_len, BOOL joliet_flag)
{
  GenerateDestinationNamesRecurse (
    pRootDirNode, iso9660_level, iso9660_character_set, allow_max_filename_len, joliet_flag);
}

void DirectoryTree::GenerateDestinationNamesRecurse(
  DIRNODE *dirnodeP, int iso9660_level, int iso9660_character_set, BOOL allow_max_filename_len, BOOL joliet_flag)
{
  // Loop through the files...

  ABSQ_LOOP (&dirnodeP->file_qhd, filenodeP, FILENODE, nextfile)
    {
    TCHAR iso9660_name[_MAX_PATH + 1];
    TCHAR joliet_name[_MAX_PATH + 1];

    // If necessary, generate a unique Joliet name for the file.
    // If the name is different from the original, then save a copy.

    if (joliet_flag)
      {
      GenerateUniqueJolietName(dirnodeP, filenodeP, allow_max_filename_len, joliet_name);

      if (StrCmp (joliet_name, filenodeP->GetSourceFileName()) != 0)
        filenodeP->dst_joliet_nameP = CloneString (joliet_name);
      }

    // Generate a unique ISO9660 name for the file.
    // If the name is different from the original, then save a copy.

    GenerateUniqueISO9660Name (
      dirnodeP, filenodeP, iso9660_name, iso9660_level, iso9660_character_set);

    if (StrCmp (iso9660_name, filenodeP->GetSourceFileName(TRUE)) != 0)
      filenodeP->dst_iso9660_nameP = CloneString (iso9660_name);

    // Dump the filenames.

    #if 0
    FILE *foo = fopen ("C:\\NAMES.OUT", "a");
    fprintf (foo, "Source  = %s\n", filenodeP->GetSourceFileName());

    if (filenodeP->dst_iso9660_nameP)
      fprintf (foo, "ISO9660 = %s\n", filenodeP->dst_iso9660_nameP);

    if (filenodeP->dst_joliet_nameP)
      fprintf (foo, "Joliet  = %s\n", filenodeP->dst_joliet_nameP);

    fprintf (foo, "\n");

    fclose (foo);
    #endif
    }
  ABSQ_POOL
  
  // Recurse through the subdirectories...

  ABSQ_LOOP (&dirnodeP->subdir_qhd, dirP, DIRNODE, nextdir)
    GenerateDestinationNamesRecurse (
      dirP, iso9660_level, iso9660_character_set, allow_max_filename_len, joliet_flag);
  ABSQ_POOL
}

//
// Generate a unique ISO9660 name for a file
//

static void GenerateUniqueISO9660Name(
  DIRNODE *dirP, FILENODE *filenodeP, TCHAR *outnameP,
  int iso9660_level, int iso9660_character_set)
{
  TCHAR tempname[_MAX_PATH + 1];
  TCHAR xlatename[(_MAX_PATH * 2) + 1];

  // Assume the long name is a good short name. If this is determined not
  // to be the case, then reset the flag to force tilde naming.

  BOOL all_legal_chars = TRUE;

  // Make a temporary working copy of the original long name.

  StringCopy (tempname, filenodeP->GetSourceFileName());

  // If this is a directory name and we're using the ISO9660 character set,
  // then remove all dots from the name.

  if (filenodeP->dir_flag && (iso9660_character_set == CDFS_ISO9660_CHARSET_STANDARD))
    {
    if (StringRemoveChar(tempname, '.')) all_legal_chars = FALSE;
    }

  // otherwise, remove all dots except for a legal extension.

  else
    {
    // All trailing dots must be removed (e.g. "FOO.BAR.GRONK.." => "FOO.BAR.GRONK")

    if (StringTrimTrailing (tempname, '.')) all_legal_chars = FALSE;

    // Point to the last dot (if any), so we can protect it from being removed.

    TCHAR *lastdotP = StrRChr(tempname, '.');

    // Remove all remaining dots except for the protected one.

    if (StringRemoveChar (tempname, '.', lastdotP)) all_legal_chars = FALSE;

    // If we're not using the ISO9660 character set and the name begins with a dot
    // (i.e. a name with just an extension), then remove that dot also.

    if (iso9660_character_set != CDFS_ISO9660_CHARSET_STANDARD)
      {if (StringTrimLeading (tempname, '.')) all_legal_chars = FALSE;}
    }

  // Get a pointer to the required character translation table.

  TCHAR **xlatePAP = GetCharTranslationTable (iso9660_level, iso9660_character_set);

  // Translate the name according to the supplied table. The resulting string is ASCII
  // and does *not* contain any MBCS or UNICODE characters.

  if (! TranslateName (tempname, xlatename, xlatePAP)) all_legal_chars = FALSE;

  // Find the extension and the end of the string.

  TCHAR *chP, *extensionP = NULL;
  for (chP = xlatename; *chP != '\0'; chP++) if (*chP == '.') extensionP = chP;

  // Compute the length of the entire translated name (in TCHARs).

  int xlatename_len = chP - xlatename;

  // Compute the length of the name and the extension (including the dot).

  int nameportion_len = (extensionP ? extensionP - xlatename : xlatename_len);
  int extportion_len =  (extensionP ? xlatename_len - nameportion_len : 0);

  // See if we already have a valid translated name.

  if ((all_legal_chars) ||
      (iso9660_level == CDFS_ISO9660_LEVEL2) ||
      (iso9660_character_set == CDFS_ISO9660_CHARSET_STANDARD))
    {
    // If the name has a valid length and it's already unique, then we're done.

    if (((iso9660_level == CDFS_ISO9660_LEVEL1) && (nameportion_len <= 8) && (extportion_len <= 4)) ||
        ((iso9660_level == CDFS_ISO9660_LEVEL2) && (xlatename_len <= ISO9660_MAX_LEVEL2_NAME_LENGTH)))
      {
      if (IsUniqueISO9660Name (dirP, filenodeP, xlatename))
        {
        StrCpy (outnameP, xlatename);
        return;
        }
      }
    }

  // A new name must be constructed.
  // Keep generating and testing names until we get a unique one...

  TCHAR unique_name[ISO9660_MAX_LEVEL2_NAME_LENGTH + 1];

  switch (iso9660_level)
    {
    // ISO9660 "Level 1" names (8.3 format)

    case CDFS_ISO9660_LEVEL1:
      {
      if (iso9660_character_set == CDFS_ISO9660_CHARSET_STANDARD)
        {
        int unique_num = 1;       // start with "1", but it won't actually be seen, since...
        int unique_num_len = 0;   // no number insertion the first time through
        int next_limit = 1;

        // Make sure that the name is restricted to the 8.3 format.

        if (nameportion_len > 8) nameportion_len = 8;
        if (extportion_len > 4) extportion_len = 4;     // includes dot

        LOOP
          {
          TCHAR unique_num_str[16];

          // Convert the number to a string.

          IntToStr (unique_num, unique_num_str, 10);

          // Generate a name and check if it's unique.

          StrPrintf (
            unique_name, _T("%.*s%.*s%.*s"), nameportion_len, xlatename,
            unique_num_len, unique_num_str, extportion_len, extensionP);

          if (IsUniqueISO9660Name (dirP, filenodeP, unique_name)) break;

          // Increment the unique number and try again.

          unique_num++;

          if (unique_num >= next_limit)
            {
            unique_num_len++;
            next_limit *= 10;

            if ((nameportion_len + unique_num_len) > 8) nameportion_len = 8 - unique_num_len;
            }
          }
        }
      else
        {
        int unique_num = 1;     // start with "~1"
        int unique_num_len = 2; // includes the tilde character
        int next_limit = 10;

        if (nameportion_len + unique_num_len > 8) nameportion_len = 8 - unique_num_len;
        if (extportion_len > 4) extportion_len = 4;

        LOOP
          {
          StrPrintf (unique_name, _T("%.*s~%d%.*s"),
            nameportion_len, xlatename, unique_num, extportion_len, extensionP);

          if (IsUniqueISO9660Name (dirP, filenodeP, unique_name)) break;

          unique_num++;

          if (unique_num == next_limit)
            {
            unique_num_len++;
            next_limit *= 10;

            if ((nameportion_len + unique_num_len) > 8) nameportion_len = 8 - unique_num_len;
            }
          }
        }

      break;
      }

    // ISO9660 "Level 2" names (maximum 31 characters)

    case CDFS_ISO9660_LEVEL2:
      {
      int unique_num = 1;       // start with "1", but it won't actually be seen, since...
      int unique_num_len = 0;   // no number insertion the first time through
      int next_limit = 1;

      // If the name is too long, then cut back the extension to at most 3 characters
      // plus the dot. Then take as much of the name as possible, but still fitting
      // within the allowed maximum length.

      if ((nameportion_len + extportion_len) > ISO9660_MAX_LEVEL2_NAME_LENGTH)
        {
        if (extportion_len > 4) extportion_len = 4;

        if ((nameportion_len + extportion_len) > ISO9660_MAX_LEVEL2_NAME_LENGTH)
          nameportion_len = ISO9660_MAX_LEVEL2_NAME_LENGTH - extportion_len;
        }

      LOOP
        {
        TCHAR unique_num_str[16];

        IntToStr (unique_num, unique_num_str, 10);

        StrPrintf (
          unique_name, _T("%.*s%.*s%.*s"), nameportion_len, xlatename,
          unique_num_len, unique_num_str, extportion_len, extensionP);

        if (IsUniqueISO9660Name (dirP, filenodeP, unique_name)) break;

        unique_num++;

        if (unique_num >= next_limit)
          {
          unique_num_len++;
          next_limit *= 10;

          if ((nameportion_len + unique_num_len + extportion_len) > ISO9660_MAX_LEVEL2_NAME_LENGTH)
            nameportion_len = ISO9660_MAX_LEVEL2_NAME_LENGTH - unique_num_len - extportion_len;
          }
        }

      break;
      }

    // Bugcheck.

    default: SignalException (E_BugCheck);
    }

  // Return the unique name to the caller...

  StrCpy (outnameP, unique_name);
}

//
// Generate a unique Joliet name for a file
//

static void GenerateUniqueJolietName(
  DIRNODE *dirP, FILENODE *filenodeP, BOOL allow_max_filename_len, TCHAR *outnameP)
{
  unsigned int ch;
  TCHAR joliet_name[_MAX_PATH + 1];

  // Determine the maximum filename length allowed.

  int max_name_len = (allow_max_filename_len ? 107 : ISO9660_MAX_JOLIET_NAME_LENGTH);

  // Make a working copy of the source filename.

  StrCpy (joliet_name, filenodeP->GetSourceFileName());

  // Compute the number of characters in the name (could contain multi-byte chars).
  // Find the extension (if any) and the number of chars that it contains.
  // Replace all semicolons (not allowed) with underscores.

  int total_name_charcnt = 0;
  int extportion_charcnt = 0;

  TCHAR *extensionP = NULL;

  for (TCHAR *chP = joliet_name; ((ch = StrNextChr(chP)) != '\0'); chP = StrInc(chP))
    {
    // Keep track of the total number of characters in the name.
    total_name_charcnt++;

    // Replace all semicolons with underscores.
    if (ch == ';') *chP = '_';

    // Start of extension?
    if (ch == '.')
      {
      extensionP = chP;
      extportion_charcnt = 0;
      }

    // If necessary, keep track of the extension's character count (including the dot).
    if (extensionP) extportion_charcnt++;
    }

  // See if we already have a valid and unique name.

  if (total_name_charcnt <= max_name_len)
    {
    if (IsUniqueJolietName(dirP, filenodeP, joliet_name))
      {
      StrCpy (outnameP, joliet_name);
      return;
      }
    }

  // A new name must be constructed.
  // Keep generating and testing names until we get a unique one...

  TCHAR unique_name[_MAX_PATH + 1];

  int unique_num = 1;           // start with "1", but it won't actually be seen, since...
  int unique_num_charcnt = 0;   // no number insertion the first time through
  int next_limit = 1;

  // Compute the number of characters in the name before the extension (if any).

  int nameportion_charcnt = total_name_charcnt - extportion_charcnt;

  // If the name is too long, then cut back the extension to at most 3 characters
  // plus the dot. Then take as much of the name as possible, but still fitting
  // within the allowed maximum length.

  if (total_name_charcnt > max_name_len)
    {
    if (extportion_charcnt > 4) extportion_charcnt = 4;

    if ((nameportion_charcnt + extportion_charcnt) > max_name_len)
      nameportion_charcnt = max_name_len - extportion_charcnt;
    }

  // Compute the number of TCHARs required for the name and extension.

  int nameportion_len = CharCountToLength (joliet_name, nameportion_charcnt);
  int extportion_len = CharCountToLength (extensionP, extportion_charcnt);

  LOOP
    {
    TCHAR unique_num_str[16];

    // Convert the number to a string.

    IntToStr (unique_num, unique_num_str, 10);

    // Generate a name and check if it's unique.

    StrPrintf (
      unique_name, _T("%.*s%.*s%.*s"), nameportion_len, joliet_name,
      unique_num_charcnt, unique_num_str, extportion_len, extensionP);

    if (IsUniqueJolietName (dirP, filenodeP, unique_name)) break;

    // Increment the unique number and try again.

    unique_num++;

    if (unique_num >= next_limit)
      {
      unique_num_charcnt++;
      next_limit *= 10;

      // If the overall name is too long, then cut back the name portion.

      if ((nameportion_charcnt + unique_num_charcnt + extportion_charcnt) > max_name_len)
        {
        nameportion_charcnt = max_name_len - unique_num_charcnt - extportion_charcnt;
        nameportion_len = CharCountToLength (joliet_name, nameportion_charcnt);
        }
      }
    }

  // Return the unique name to the caller.

  StrCpy (outnameP, unique_name);
}

// 
// Check if an ISO9660 name is unique within a directory
//

static BOOL IsUniqueISO9660Name(DIRNODE *dirP, FILENODE *filenodeP, const TCHAR *nameP)
{
  // Loop through all the FILENODEs...

  ABSQ_LOOP (&dirP->file_qhd, loc_filenodeP, FILENODE, nextfile)
    {
    // Check the short name of all nodes until we reach the one passed in.

    if (loc_filenodeP == filenodeP) break;

    if (StrICmp (loc_filenodeP->GetDestFileName(FALSE), nameP) == 0)
      return (FALSE);
    }
  ABSQ_POOL

  // Loop through all the FILENODEs...

  ABSQ_LOOP (&dirP->file_qhd, loc_filenodeP, FILENODE, nextfile)
    {
    // Check the long name for all nodes except for the one passed in.

    if (loc_filenodeP != filenodeP)
      {
      if (StrICmp (loc_filenodeP->GetDestFileName(TRUE), nameP) == 0)
        return (FALSE);
      }
    }
  ABSQ_POOL

  // The name is unique.

  return (TRUE);
}

// 
// Check if a JOLIET name is unique within a directory
//

static BOOL IsUniqueJolietName(DIRNODE *dirP, FILENODE *filenodeP, const TCHAR *nameP)
{
  // Loop through all the FILENODEs...

  ABSQ_LOOP (&dirP->file_qhd, loc_filenodeP, FILENODE, nextfile)
    {
    // Check the long name for all nodes except for the one passed in.

    if (loc_filenodeP != filenodeP)
      {
      if (StrICmp (loc_filenodeP->GetDestFileName(TRUE), nameP) == 0)
        return (FALSE);
      }
    }
  ABSQ_POOL

  // The name is unique.

  return (TRUE);
}

// in_nameP points to UNICODE, MBCS, or ASCII input string (null terminated).
// out_nameP points to the buffer to receive the translated characters (never double-byte MBCS), null terminated.
// the output buffer must be large enough (maximum twice the number of input characters + null terminator).
// xlatePAP is a pointer to an array of 256 pointers (or nulls) to the ASCII translation strings.
// the function returns FALSE if any non-legal input characters were seen, TRUE otherwise.

static BOOL TranslateName(const TCHAR *in_nameP, TCHAR *out_nameP, TCHAR **xlatePAP)
{
  const TCHAR *inChP, *transChP;
  unsigned int ch;

  // Assume success to start.

  BOOL all_legal_chars = TRUE;

  // Step through the input string character-by-character. For each character,
  // substitute the character(s) from the translation table. Translate all illegal
  // characters to underscores.

  TCHAR *outChP = out_nameP;

  for (inChP = in_nameP; ((ch = StrNextChr(inChP)) != '\0'); inChP = StrInc(inChP))
    {
    if ((ch < 256) && (((transChP = xlatePAP[ch])) != NULL))
      {
      // Copy the translation character(s) from the table.
      while (*transChP) *outChP++ = *transChP++;
      }
    else
      {
      // Illegal characters are translated to an underscore.
      *outChP++ = '_';
      all_legal_chars = FALSE;
      }
    }

  // Add the string terminator.

  *outChP = '\0';

  // Return the final status.

  return (all_legal_chars);
}

//
// Get a pointer to a character translation table
//

static TCHAR **GetCharTranslationTable(int iso9660_level, int iso9660_character_set)
{
  extern TCHAR *gISO9660CharTransTable[];
  extern TCHAR *gDOSCharTransTable[];
  extern TCHAR *gASCIICharTransTable[];

  TCHAR **tableP;

  switch (iso9660_level)
    {
    case CDFS_ISO9660_LEVEL1:
      {
      switch (iso9660_character_set)
        {
        case CDFS_ISO9660_CHARSET_DOS: tableP = gDOSCharTransTable; break;
        case CDFS_ISO9660_CHARSET_ASCII: tableP = gASCIICharTransTable; break;
        default: tableP = gISO9660CharTransTable;
        }
      break;
      }

    case CDFS_ISO9660_LEVEL2:
      {
      switch (iso9660_character_set)
        {
        case CDFS_ISO9660_CHARSET_DOS: tableP = gDOSCharTransTable; break;
        case CDFS_ISO9660_CHARSET_ASCII: tableP = gASCIICharTransTable; break;
        default: tableP = gISO9660CharTransTable;
        }
      break;
      }

    default: tableP = gISO9660CharTransTable;
    }

  // Return a pointer to the translation table.

  return tableP;
}

// --------------------------------------------------------------------------------------------
//
// Sort a directory tree
//
// --------------------------------------------------------------------------------------------

void DirectoryTree::Sort(
  int file_field, BOOL descending_flag, BOOL dirs_first_flag,
  BOOL case_insensitive_flag, BOOL dvdvideo_compatible, BOOL long_names_flag)
{
  gSortFileField = file_field;

  gbSortLongNames = long_names_flag;
  gbSortDirNodesOnly = FALSE;

  gbSortDescending = descending_flag;
  gbSortDirsFirst = dirs_first_flag;
  gbSortCaseInsensitive = case_insensitive_flag;
  gbSortDVDVideoCompatible = dvdvideo_compatible;

  SortRecurse (pRootDirNode);
}

void DirectoryTree::Sort(BOOL long_names_flag, BOOL dirnodes_only_flag)
{
  gSortFileField = CDFS_FILESORT_NAME;

  gbSortLongNames = long_names_flag;
  gbSortDirNodesOnly = dirnodes_only_flag;

  gbSortDescending = FALSE;
  gbSortDirsFirst = FALSE;
  gbSortCaseInsensitive = FALSE;
  gbSortDVDVideoCompatible = FALSE;

  SortRecurse (pRootDirNode);
}

void DirectoryTree::SortRecurse(DIRNODE *dirnodeP)
{
  // Sort the subdirectory queue.

  SortSubdirQueue (&dirnodeP->subdir_qhd);

  // If necessary, sort the filename queue.

  if (! gbSortDirNodesOnly) SortFileQueue (&dirnodeP->file_qhd);

  // Recurse through the rest of the tree...

  ABSQ_LOOP (&dirnodeP->subdir_qhd, dirP, DIRNODE, nextdir)
    SortRecurse (dirP);
  ABSQ_POOL
}

//
// Compare two file or directory names.
//

static int CompareNames(const TCHAR *name1, const TCHAR *name2)
{
  int status;

  TCHAR name_portion1[_MAX_PATH + 1];
  TCHAR ext_portion1[_MAX_PATH + 1];

  TCHAR name_portion2[_MAX_PATH + 1];
  TCHAR ext_portion2[_MAX_PATH + 1];

  // Separate the first name into two parts.

  const TCHAR *lastdot1P = StrRChr (name1, '.');

  if (lastdot1P == NULL)
    {
    StringCopy (name_portion1, name1);
    StringCopy (ext_portion1, _T(""));
    }
  else
    {
    int len = lastdot1P - name1;
    StringCopy (name_portion1, name1, len);
    StringCopy (ext_portion1, lastdot1P);
    }

  // Separate the second name into two parts.

  const TCHAR *lastdot2P = StrRChr (name2, '.');

  if (lastdot2P == NULL)
    {
    StringCopy (name_portion2, name2);
    StringCopy (ext_portion2, _T(""));
    }
  else
    {
    int len = lastdot2P - name2;
    StringCopy (name_portion2, name2, len);
    StringCopy (ext_portion2, lastdot2P);
    }
  
  // Compare the names.

  if (gbSortDVDVideoCompatible)
    {
    status = CompareNamesDVD (
      name_portion1, name_portion2, ext_portion1, ext_portion2);
    }
  else
    {
    if (gbSortCaseInsensitive)
      {
      status = StrICmp (name_portion1, name_portion2);

      if (status == 0)
        status = StrICmp (ext_portion1, ext_portion2);
      }
    else
      {
      status = StrCmp (name_portion1, name_portion2);

      if (status == 0)
        status = StrCmp (ext_portion1, ext_portion2);
      }
    }

  // Return the final status.

  return (status);
}

//
// Compare two file or directory names.
// Sort the names in DVD-Video compatible order.
//

static int CompareNamesDVD(
  const TCHAR *name1, const TCHAR *name2, const TCHAR *ext1, const TCHAR *ext2)
{
  // DVD-Video File Sort Order...
  //
  // VIDEO_TS.IFO
  // VIDEO_TS.VOB
  // VIDEO_TS.BUP
  // VTS_01_0.IFO
  // VTS_01_0.VOB
  //    :      :
  // VTS_01_9.VOB
  // VTS_01_0.BUP
  // VTS_02_0.IFO
  // VTS_02_0.VOB
  //    :      :
  // VTS_02_9.VOB
  // VTS_02_0.BUP

  static TCHAR *ExtensionTable[] = {_T(".IFO"), _T(".VOB"), _T(".BUP"), _T("")};

  int status;

  // Special case for the directory names "VIDEO_TS" and "AUDIO_TS".
  // VIDEO_TS sorts first and AUDIO_TS sorts second.

  if ((StrLen(ext1) == 0) && (StrLen(ext2) == 0))
    {
    // VIDEO_TS comes before all other directories.

    if (StrICmp(name1, _T("VIDEO_TS")) == 0) return (-1);
    if (StrICmp(name2, _T("VIDEO_TS")) == 0) return (1);

    // AUDIO_TS comes after VIDEO_TS, but before all other directories.

    if (StrICmp(name1, _T("AUDIO_TS")) == 0) return (-1);
    if (StrICmp(name2, _T("AUDIO_TS")) == 0) return (1);
    }

  // Special case for the filenames VIDEO_TS.*. These files are sorted by their
  // extensions in the order ".IFO", ".VOB", ".BUP".
  
  if ((StrICmp(name1, _T("VIDEO_TS")) == 0) && (StrICmp(name2, _T("VIDEO_TS")) == 0))
    {
    int lookup1 = StringTableLookup (ext1, ExtensionTable);
    int lookup2 = StringTableLookup (ext2, ExtensionTable);

    if ((lookup1 >= 0) && (lookup2 >= 0))
      return (INTCMP (lookup1, lookup2));
    }

  // If both names begin with "VTS_", then group the names in the correct order.

  if ((StrNICmp(name1, _T("VTS_"), 4) == 0) && (StrNICmp(name2, _T("VTS_"), 4) == 0))
    {
    // If the names match in the first 6 characters ("VTS_nn"), then they are sorted by
    // extension first (in the order ".IFO", ".VOB", ".BUP") and then by name.
  
    if (StrNICmp(name1, name2, 6) == 0)
      {
      int lookup1 = StringTableLookup (ext1, ExtensionTable);
      int lookup2 = StringTableLookup (ext2, ExtensionTable);

      if ((lookup1 >= 0) && (lookup2 >= 0))
        {
        // Compare the extensions.
        status = INTCMP (lookup1, lookup2);
        if (status != 0) return (status);

        // The extensions are equal, so compare the names.
        return (StrICmp (name1, name2));
        }
      }
    }

  // No special cases, so compare the names normally.

  status = StrICmp (name1, name2);

  if (status == 0)
    status = StrICmp (ext1, ext2);

  // Return the final status.

  return (status);
}

//
// Compare two file or directory names.
//

#if 0
static int CompareNames(const TCHAR *name1, const TCHAR *name2)
{
  #if defined(WIN32)
    unsigned int ch1, ch2;
  #else
    unsigned char ch1, ch2;
  #endif

  // Initialize pointers to the names.

  const TCHAR *ptr1 = name1;
  const TCHAR *ptr2 = name2;

  // Get a pointer to the last extension in each name (if any).

  const TCHAR *lastdot1 = StrRChr(ptr1, '.');
  const TCHAR *lastdot2 = StrRChr(ptr2, '.');

  // Compare the names...

  LOOP
    {
    // Get the next character in the names.

    ch1 = StrNextChr(ptr1);
    ch2 = StrNextChr(ptr2);

    // Have we reached the end of either name?

    if ((ch1 == '\0') || (ch2 == '\0')) break;

    // Special case for the extension.
    // If we see one extension before the other, then the longer name sorts high.

    if ((ptr1 != lastdot1) && (ptr2 == lastdot2)) return 1;
    if ((ptr1 == lastdot1) && (ptr2 != lastdot2)) return -1;

    // If we're doing a case in-sensitive comparison, then uppercase the chars.

    if (gbSortCaseInsensitive)
      {
      ch1 = ToUpper(ch1);
      ch2 = ToUpper(ch2);
      }

    // Compare the chars.

    if (ch1 > ch2) return 1;
    if (ch1 < ch2) return -1;

    // The characters are equal...
    // Increment the name pointers and continue looping.

    ptr1 = StrInc(ptr1);
    ptr2 = StrInc(ptr2);
    }

  // Reached end of name(s). The longer name sorts high.

  if (ch1 != '\0') return 1;
  if (ch2 != '\0') return -1;

  return 0;
}
#endif

//
// qsort callback - Compare two directory nodes
//

static int CompareDirectoryNodes(const void *rr, const void *ll)
{
  DIRNODE *dirnode1P = *(DIRNODE **)rr;
  DIRNODE *dirnode2P = *(DIRNODE **)ll;

  FILENODE *filenode1P = dirnode1P->parent_filenodeP;
  FILENODE *filenode2P = dirnode2P->parent_filenodeP;

  // Are we sorting by the short or long names?

  const TCHAR *name1 = filenode1P->GetDestFileName (gbSortLongNames);
  const TCHAR *name2 = filenode2P->GetDestFileName (gbSortLongNames);

  // Compare the names.

  return CompareNames(name1, name2);
}

//
// qsort callback - Compare two filenodes
//

static int CompareFileNodes(const void *rr, const void *ll)
{
  int status;

  FILENODE *filenode1 = *(FILENODE **)rr;
  FILENODE *filenode2 = *(FILENODE **)ll;

  // Are we placing directory names before file names?

  if (gbSortDirsFirst || gbSortDVDVideoCompatible)
    {
    if (filenode1->dir_flag && (! filenode2->dir_flag)) return -1;
    if (filenode2->dir_flag && (! filenode1->dir_flag)) return 1;
    }

  // Sort by the selected field...

  switch (gSortFileField)
    {
    case CDFS_FILESORT_NAME:
      {
      // Are we sorting by the short or long names?
      const TCHAR *name1 = filenode1->GetDestFileName (gbSortLongNames);
      const TCHAR *name2 = filenode2->GetDestFileName (gbSortLongNames);

      // Compare the names.
      status = CompareNames(name1, name2);

      break;
      }

    case CDFS_FILESORT_SIZE:
      status = CompareValues (filenode1->size, filenode2->size);
      break;
      
    case CDFS_FILESORT_CREATION_DATE:
      status = CompareDateTime (
        &filenode1->creation_datetime, &filenode2->creation_datetime);
      break;

    case CDFS_FILESORT_MODIFICATION_DATE:
      status = CompareDateTime (
        &filenode1->modified_datetime, &filenode2->modified_datetime);
      break;

    case CDFS_FILESORT_LASTACCESS_DATE:
      status = CompareDateTime (
        &filenode1->accessed_datetime, &filenode2->accessed_datetime);
      break;

    default:
      SignalException (E_BugCheck);
    }

  // If we're sorting in descending order, then flip the status.

  if (gbSortDescending) status = (-status);

  // Return the final status.

  return (status);
}

//
// Sort a queue of subdirectory blocks.
//

static void SortSubdirQueue(QUE *subdir_qhd)
{
  // Get the number of queue entries.

  int count = ABSQ_COUNT (subdir_qhd);

  // If there is nothing to sort, then return.

  if (count < 2) return;

  // Check for a count that would exceed a 64K byte sort vector.

  if (count > 16000) SignalException (E_MaxFileSort);

  // Build a sort vector.

  int c = 0;

  DIRNODE **sortlist = new DIRNODE*[count];

  ABSQ_LOOP (subdir_qhd, dirnodeP, DIRNODE, nextdir)
    sortlist[c++] = dirnodeP;
  ABSQ_POOL

  // Sort the list.

  qsort (sortlist, count, sizeof(DIRNODE *), CompareDirectoryNodes);

  // Rebuild the queue in sorted order.

  ABSQ_INIT (subdir_qhd);

  for (int i = 0; i < count; i++)
    ABSQ_INSERT_TAIL (subdir_qhd, sortlist[i], DIRNODE, nextdir);

  // Free the sort vector.

  delete []sortlist;
}

//
// Sort a queue of file blocks.
//

static void SortFileQueue(QUE *file_qhd)
{
  // Get the number of queue entries.

  int count = ABSQ_COUNT (file_qhd);

  // If there is nothing to sort, simply return.

  if (count < 2) return;

  // Check for a count that would exceed a 64K byte sort vector.

  if (count > 16000) SignalException (E_MaxFileSort);

  // Allocate a sort vector.

  FILENODE **sortlist = new FILENODE*[count];

  // Build the sort vector.

  int c = 0;

  ABSQ_LOOP (file_qhd, filenodeP, FILENODE, nextfile)
    sortlist[c++] = filenodeP;
  ABSQ_POOL

  // Sort the list.

  qsort (sortlist, count, sizeof(FILENODE *), CompareFileNodes);

  // Rebuild the queue in sorted order.

  ABSQ_INIT (file_qhd);

  for (int i = 0; i < count; i++)
    ABSQ_INSERT_TAIL (file_qhd, sortlist[i], FILENODE, nextfile);

  // Free the sort vector.

  delete []sortlist;
}

// --------------------------------------------------------------------------------------------
//
// Directory Tree Statistics
//
// --------------------------------------------------------------------------------------------

void DirectoryTree::CollectStatistics()
{
  // Reset all statistics.
  nDirCount = 0;
  nFileCount = 0;
  
  nMaxDirDepth = 0;

  nTotalNewDiscBlkcnt = 0;
  nTotalImportDiscBlkcnt = 0;
  nTotalDiscBlkcnt = 0;

  // Collect new statistics.
  CollectStatisticsRecurse (pRootDirNode);
}

void DirectoryTree::CollectStatisticsRecurse(DIRNODE *dirnodeP)
{
  // Increment the number of directories.

  nDirCount++;

  // Keep track of the maximum directory depth.

  nMaxDirDepth = MAX (nMaxDirDepth, dirnodeP->depth);

  // Loop through the files...

  ABSQ_LOOP (&dirnodeP->file_qhd, filenodeP, FILENODE, nextfile)
    {
    if (! filenodeP->dir_flag)
      {
      // Increment the number of files.

      nFileCount++;

      // Update the disc block totals.

      ULONG blkcnt = CDIV (filenodeP->size, 2048);

      nTotalDiscBlkcnt += blkcnt;

      if (filenodeP->import_flag)
        nTotalImportDiscBlkcnt += blkcnt;
      else
        nTotalNewDiscBlkcnt += blkcnt;
      }
    }
  ABSQ_POOL
  
  // Recurse through the subdirectories...

  ABSQ_LOOP (&dirnodeP->subdir_qhd, dirP, DIRNODE, nextdir)
    CollectStatisticsRecurse (dirP);
  ABSQ_POOL
}

//
// Compute the maximum depth of a tree
//

int DirectoryTree::ComputeDepth()
{
  int depth = 1;
  ComputeDepthRecurse (pRootDirNode, &depth);

  return (depth);
}

void DirectoryTree::ComputeDepthRecurse(DIRNODE *dirnodeP, int *depthP)
{
  // Keep track of the maximum directory depth.

  if (dirnodeP->depth > *depthP) *depthP = dirnodeP->depth;

  // Recurse through the subdirectories...

  ABSQ_LOOP (&dirnodeP->subdir_qhd, dirP, DIRNODE, nextdir)
    ComputeDepthRecurse (dirP, depthP);
  ABSQ_POOL
}

// --------------------------------------------------------------------------------------------
//
// Dump the directory tree.
//
// --------------------------------------------------------------------------------------------

void DirectoryTree::Dump(const TCHAR *filnamP)
{
  FILE *file;

  if (filnamP == NULL)
    file = stdout;
  else
    {
    if ((file = FOpen (filnamP, _T("wb"))) == NULL)
      SignalException (E_CreateFile, 1, 0, 0, HeapString(filnamP));
    }

  DumpRecurse (pRootDirNode, file);

  if (filnamP != NULL) fclose (file);
}

void DirectoryTree::DumpRecurse(DIRNODE *dirnodeP, FILE *file)
{
  UBYTE depth = dirnodeP->depth;

  for (int i = 0; i < depth - 1; i++) fprintf (file, "  ");

  fprintf (file, "LBA=%lu blkcnt=%u recnum=%u\n",
    dirnodeP->iso9660_lba, dirnodeP->iso9660_blkcnt,
    dirnodeP->pathtable_recnum);

  ABSQ_LOOP (&dirnodeP->file_qhd, filenodeP, FILENODE, nextfile)
    {
    if (! filenodeP->dir_flag)
      {
      for (int i = 0; i < depth; i++) fprintf (file, "  ");

      fprintf (file, "\"%s\" LBA=%lu blkcnt=%lu\n",
        filenodeP->src_nameP,
        filenodeP->lba, CDIV (filenodeP->size, 2048));
      }
    }
  ABSQ_POOL
  
  ABSQ_LOOP (&dirnodeP->subdir_qhd, dirP, DIRNODE, nextdir)
    DumpRecurse (dirP, file);
  ABSQ_POOL
}

// --------------------------------------------------------------------------------------------
//
// DIRNODE Constructor and Destructor
//
// --------------------------------------------------------------------------------------------

// Constructor

DIRNODE::DIRNODE(
  DIRNODE *parent_dirnodeP, FILENODE *parent_filenodeP, UBYTE depth, QUE *qhdP)
{
  MEMCLEAR (this, sizeof(DIRNODE));

  // Initialize the file and subdirectory queues.

  ABSQ_INIT (&file_qhd);
  ABSQ_INIT (&subdir_qhd);

  // Root directory node points to self as parent.
  // Save a pointer to the FILENODE corresponding to this DIRNODE.

  this->parent_dirnodeP = ((parent_dirnodeP == NULL) ? this : parent_dirnodeP);
  this->parent_filenodeP = parent_filenodeP;

  this->depth = depth;                              // Initialize the directory depth.
                                                                  
  ABSQ_INSERT_TAIL (qhdP, this, DIRNODE, nextdir)   // link into parent queue
}                                                                 

// Destructor

DIRNODE::~DIRNODE()                                               
{
  // Recursively delete all of our subdirectories.
                                                          
  ABSQ_LOOP (&this->subdir_qhd, dirnodeP, DIRNODE, nextdir)
    delete dirnodeP;
  ABSQ_POOL

  // Delete all of our files

  ABSQ_LOOP (&this->file_qhd, filenodeP, FILENODE, nextfile)
    delete filenodeP;
  ABSQ_POOL

  delete this->parent_filenodeP;            // free our corresponding FILENODE
  ABSQ_REMOVE (this, DIRNODE, nextdir)      // unlink from parent queue
}

// Count the number of files and/or directories in a given subdirectory.

int DIRNODE::GetFileCount(BOOL include_files_flag, BOOL include_dirs_flag)
{
  int count = 0;

  ABSQ_LOOP (&this->file_qhd, filenodeP, FILENODE, nextfile)
    {
    if (((! filenodeP->dir_flag) && include_files_flag) ||
        ((  filenodeP->dir_flag) && include_dirs_flag ))
      count++;
    }
  ABSQ_POOL

  return (count);
}

// --------------------------------------------------------------------------------------------
//
// FILENODE Constructor and Destructor
//
// --------------------------------------------------------------------------------------------

FILENODE::FILENODE(TCHAR *pathnameP, FILEINFO *finfoP, QUE *qhdP)
{
  // Save a pointer to the already-cached pathname.

  this->src_pathnameP = pathnameP;

  // Clone the filenames.

  this->src_nameP = CloneString(finfoP->name);
  this->src_alt_nameP = CloneString(finfoP->alternative_name);

  // Clear the destination names.

  this->dst_iso9660_nameP = NULL;
  this->dst_joliet_nameP = NULL;
  
  // Copy the file size (in bytes).

  this->size = finfoP->size.low;

  if (! finfoP->dir_flag)
    {
    this->iso9660_size = finfoP->size.low;
    this->joliet_size = finfoP->size.low;
    this->udf_size = finfoP->size.low;
    }
  else
    {
    this->iso9660_size = 0;
    this->joliet_size = 0;
    this->udf_size = 0;
    }

  // Copy the date and timestamp.

  this->creation_datetime = finfoP->creation_datetime;
  this->modified_datetime = finfoP->modified_datetime;
  this->accessed_datetime = finfoP->accessed_datetime;

  // Copy the attribute flags.

  this->archive_flag = finfoP->archive_flag;
  this->dir_flag = finfoP->dir_flag;
  this->hidden_flag = finfoP->hidden_flag;
  this->readonly_flag = finfoP->readonly_flag;
  this->system_flag = finfoP->system_flag;

  this->import_flag = FALSE;
  this->reserved_flags = 0;
  this->lba = 0;
  this->iso9660_lba = 0;
  this->joliet_lba = 0;
  this->udf_lba = 0;

  // Add to the file queue.

  ABSQ_INSERT_TAIL (qhdP, this, FILENODE, nextfile)
}

//
// FILENODE constructor that clones information from another FILENODE block
//

FILENODE::FILENODE(TCHAR *pathnameP, FILENODE *fileP, QUE *qhdP)
{
  // Save a pointer to the already-cached pathname.

  this->src_pathnameP = pathnameP;

  // Clone the filenames.

  this->src_nameP = CloneString(fileP->src_nameP);
  this->src_alt_nameP = CloneString(fileP->src_alt_nameP);

  // Clear the destination names.

  this->dst_iso9660_nameP = NULL;
  this->dst_joliet_nameP = NULL;
                                                              
  // Copy values directly from the supplied FILENODE.

  this->size = fileP->size;
  this->iso9660_size = fileP->size;
  this->joliet_size = fileP->size;
  this->udf_size = fileP->size;
  this->creation_datetime = fileP->creation_datetime;
  this->modified_datetime = fileP->modified_datetime;
  this->accessed_datetime = fileP->accessed_datetime;
  this->archive_flag = fileP->archive_flag;
  this->dir_flag = fileP->dir_flag;
  this->hidden_flag = fileP->hidden_flag;
  this->readonly_flag = fileP->readonly_flag;
  this->system_flag = fileP->system_flag;

  this->import_flag = FALSE;
  this->reserved_flags = 0;
  this->lba = 0;
  this->iso9660_lba = 0;
  this->joliet_lba = 0;
  this->udf_lba = 0;

  // Link into the queue.

  ABSQ_INSERT_TAIL (qhdP, this, FILENODE, nextfile)
}

//
// FILENODE Destructor
//

FILENODE::~FILENODE()
{
  // Unlink from the queue.
  ABSQ_REMOVE (this, FILENODE, nextfile)

  // Free all name strings.
  delete []src_nameP;
  delete []src_alt_nameP;
  delete []dst_iso9660_nameP;
  delete []dst_joliet_nameP;

  src_nameP = NULL;
  src_alt_nameP = NULL;
  dst_iso9660_nameP = NULL;
  dst_joliet_nameP = NULL;
}

//
// Return a file's destination name
//

const TCHAR *FILENODE::GetDestFileName(BOOL long_names_flag)
{
  // New destination Joliet filename.
  if (long_names_flag && (dst_joliet_nameP != NULL)) return (dst_joliet_nameP);
  
  // Original filename.
  if (long_names_flag && (src_nameP != NULL)) return (src_nameP);

  // New destination ISO9660 filename.
  if (dst_iso9660_nameP != NULL) return (dst_iso9660_nameP);

  // Original alternative filename (short).
  if (src_alt_nameP != NULL) return (src_alt_nameP);

  // Default to the original source filename...
  return (src_nameP);
}

// --------------------------------------------------------------------------------------------
//
// MERGE TREES
//
// --------------------------------------------------------------------------------------------

// Test for permission to replace CD file when merging equal named files.

static int IsReplaceable(FILENODE *hd_fileP, FILENODE *cd_fileP, int duplicate_file_opt)
{
  if (duplicate_file_opt == FILE_REPLACE_ALWAYS) return TRUE;   // use HD file
  if (duplicate_file_opt == FILE_REPLACE_NEVER) return FALSE;   // keep CD file

  // duplicate_file_opt == FILE_REPLACE_CHANGED

  if ((CompareDateTime (&hd_fileP->modified_datetime, &cd_fileP->modified_datetime) == 1) ||
      (hd_fileP->size != cd_fileP->size))
    return TRUE;    // HD is newer (or different size) - use it
  else
    return FALSE;   // CD is newer - keep it
}
 
// Insert (or replace) a single entry in a directory.

void DirectoryTree::MergeFile(
  DIRNODE *dirP, FILENODE *fileP, int duplicate_dir_opt, int duplicate_file_opt)
{
  // Search CD directory for a FILENODE whose name matches the new HD file.

  ABSQ_LOOP (&dirP->file_qhd, dup_fileP, FILENODE, nextfile)
    {
    if (StrICmp (dup_fileP->src_nameP, fileP->src_nameP) == 0)
      {
      // HD dir vs. CD dir:
      //   DIRECTORY_MERGE    directories will be merged, copy attributes
      //   DIRECTORY_REPLACE  delete CD directory
      // HD dir vs. CD file:  if replaceable, then delete CD file
      // HD file vs. CD dir:  if replaceable, then delete CD directory
      // HD file vs. CD file: if replaceable, then delete CD file

      BOOL hd_dir_flag = fileP->dir_flag;
      BOOL cd_dir_flag = dup_fileP->dir_flag;

      // If merging two directories, update attributes and we're done.

      if (hd_dir_flag && cd_dir_flag && (duplicate_dir_opt == DIRECTORY_MERGE))
        {
        dup_fileP->modified_datetime = fileP->modified_datetime;
        dup_fileP->hidden_flag = fileP->hidden_flag;
        return;
        }

      // Check "IsReplaceable" only if one or both of the files is not a directory.

      if ( ((! hd_dir_flag) || (! cd_dir_flag))
        && (! IsReplaceable (fileP, dup_fileP, duplicate_file_opt))) return;

      // Delete the old CD file or directory node and continue on to create a new one.
      // Deleting the directory node automatically deletes the corresponding file node.

      if (cd_dir_flag)
        {
        ABSQ_LOOP (&dirP->subdir_qhd, dup_dirnodeP, DIRNODE, nextdir)
          if (dup_dirnodeP->parent_filenodeP == dup_fileP)
            {
            delete dup_dirnodeP;
            break;
            }
        ABSQ_POOL
        }
      else
        {
        delete dup_fileP;
        }

      break;
      }
    }
  ABSQ_POOL

  // Add a new FILENODE and possibly DIRNODE to the CD directory tree.

  // Insert a new FILENODE into the tree.
  // Make our own cached copy of the pathname.
  // Copy attributes from the HD FILEINFO block.

  FILENODE *new_fileP = new FILENODE (
    AddPathname(fileP->src_pathnameP), fileP, &dirP->file_qhd);

  // Create new directory node, if necessary.

  if (fileP->dir_flag)
    new DIRNODE (dirP, new_fileP, (UBYTE)(dirP->depth + 1), &dirP->subdir_qhd);
}

//
// Recursively merge two directory subtrees.
//

void DirectoryTree::MergeRecurse(
  DIRNODE *cd_dirP, DIRNODE *hd_dirP, int duplicate_dir_opt, int duplicate_file_opt)
{
  // Walk HD directory, merging each file into the CD directory.

  ABSQ_LOOP (&hd_dirP->file_qhd, hd_fileP, FILENODE, nextfile)
    MergeFile (cd_dirP, hd_fileP, duplicate_dir_opt, duplicate_file_opt);
  ABSQ_POOL

  // Recurse through HD subdirectories, merging into corresponding CD subtrees.

  ABSQ_LOOP (&hd_dirP->subdir_qhd, hd_subdirP, DIRNODE, nextdir)
    {
    // Search for a subdirectory DIRNODE whose name matches the supplied name.

    DIRNODE *cd_subdirP = NULL;

    ABSQ_LOOP (&cd_dirP->subdir_qhd, dirnodeP, DIRNODE, nextdir)
      {
      if (StrICmp (
            dirnodeP->parent_filenodeP->src_nameP,
            hd_subdirP->parent_filenodeP->src_nameP) == 0)
        cd_subdirP = dirnodeP;
      }
    ABSQ_POOL

    MergeRecurse (cd_subdirP, hd_subdirP, duplicate_dir_opt, duplicate_file_opt);
    }
  ABSQ_POOL
}

//
// Merge entries from a HD directory tree into a CD directory tree.
//

void DirectoryTree::Merge(
  DirectoryTree *hd_dirtreeP, int duplicate_dir_opt, int duplicate_file_opt, BOOL statistics_flag)
{
  // Recursively merge trees, starting at the root nodes.

  MergeRecurse (
    this->pRootDirNode, hd_dirtreeP->pRootDirNode, duplicate_dir_opt, duplicate_file_opt);

  // Collect statistics on the merged tree?

  if (statistics_flag) CollectStatistics();
}

//
// Given a full pathname (with device name), find the start of the directory path.
//

static const TCHAR *SplitDevPath(const TCHAR *dirP);

// Invalid or incomplete paths may return NULL.  Valid paths never return NULL.
// Handles both local and network paths.  For example,
//
//    "c:\"                   returns pointer to end-of-string
//    "\\server\c\"           returns pointer to end-of-string
//
//    "c:\foo\bar\"           returns pointer to "foo\bar"
//    "\\server\c\foo\bar\"   returns pointer to "foo\bar"

static const TCHAR *SplitDevPath(const TCHAR *dirP)
{
  if ( (dirP = StrChr (dirP, '\\')) == NULL ) return NULL;  // find the first backslash
  dirP++;                                                   // skip the first backslash (UNICODE and MBCS safe)
  if ( *dirP != '\\' ) return dirP;                         // if not followed by another backslash, we're done
  dirP++;                                                   // skip second backslash (UNICODE and MBCS safe)
  if ( (dirP = StrChr (dirP, '\\')) == NULL ) return NULL;  // skip server name
  dirP++;                                                   // skip backslash (UNICODE and MBCS safe)
  if ( (dirP = StrChr (dirP, '\\')) == NULL ) return NULL;  // skip share name
  dirP++;                                                   // skip backslash (UNICODE and MBCS safe)

  return dirP;
}

// Restore previous packing alignment.

#pragma pack(pop)
