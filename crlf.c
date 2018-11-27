// CRLF.C - Copyright (c) 2003,2004 by Bob Frazier and S.F.T. Inc.
//
// Distribution and use of this source file and the compiled binary version
// are subject to the BSD-STYLE LICENSE (see below).  Other portions of this
// distribution may be covered by the 'GNU General Public License', which is
// included in the 'COPYING' file and also available at the following internet
// address:  http://www.gnu.org/copyleft/gpl.html
//
// Specifically, this file, the various text files (README, COPYING, ChangeLog),
// the manual pages (crlf.1 and crlf.interix), and the configuration scripts
// 'Makefile.am' and 'configure.in' are covered by this BSD-STYLE license.
// Other files may be covered by the GPL, such as the various files associated
// with the 'autotools' packages.  For any file not specifically covered by the
// GPL, the 'BSD-STYLE LICENSE' applies.
//
// See the included file 'README' for additional information
// See the included file 'COPYING' for specific license information and a copy
// of the GNU PUBLIC LICENSE (GPL).
//
//                         BSD-STYLE LICENSE
//
// Copyright (c) 2003,2004, Bob Frazier and S.F.T. Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the original author, nor the names of its contri-
//    butors may be used to endorse or promote products derived from this
//    software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSE-
// QUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE
//
//
// The BSD-STYLE LICENSE is based on licensing guidelines as published at
// http://www.opensource.org/licenses


#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifndef WIN32

// additional headers and definitions for UNIX

#include <fcntl.h>
#include <fnmatch.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/stat.h>

typedef int HANDLE;
typedef const char *LPCSTR;
typedef unsigned long DWORD;
typedef short BOOL;

#ifndef NAME_MAX
#define NAME_MAX MAXNAMLEN
#endif // NAME_MAX

#define MAX_PATH MAXNAMLEN
#define TRUE (BOOL)(!0)
#define FALSE (BOOL)0

#endif // WIN32

#define _VERSION_      103
#define _VER_STRING_ "v1.03"

void usage()
{
  fprintf(stderr, "crlf " _VER_STRING_ " Copyright (c) 2003,2004,2013 by Bob Frazier and S.F.T. Inc.\n"
                  "                             all rights reserved\n\n"
                  "USAGE:  crlf [-[u|m][-[n|b]][-h][-r][-q][-t][-0]] [--] <filename>\n"
                  "        translates '\\n' to '\\r\\n' and vice versa\n\n"
#ifdef WIN32
                  "  -u    converts '\\r\\n' to '\\n' (unix style)\n"
                  "  -m    (default) converts '\\n' to '\\r\\n' (MS DOS/Windows style)\n"
#else // WIN32
                  "  -u    (default) converts '\\r\\n' to '\\n' (unix style)\n"
                  "  -m    converts '\\n' to '\\r\\n' (MS DOS/Windows style)\n"
#endif // WIN32
                  "        (-u and -m are mutually exclusive options)\n"
                  "  -n    do NOT create a '.bak' backup version of the original\n"
                  "  -b    (default)create a '.bak' backup version of the original\n"
                  "  -r    recurse sub-directories (only valid if a directory name or\n"
                  "        wildcard filename specification is used\n"
                  "  -t    'trim' mode:  remove excess white space at the end of a line\n"
                  "  -0    'allow zero byte' mode - do NOT trim '0' bytes\n"
                  "  -q    'quiet' mode:  do not print file/directory names when\n"
                  "        converting, recursing directories, or creating backup files.\n"
                  "  -h    prints this 'usage' information\n"
                  "\n"
                  );
}

#ifdef HAVE_GETOPT
extern char *optarg;  // for getopt
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;
#else // HAVE_GETOPT
int optind = 1;
#endif // HAVE_GETOPT

int MainLoop(LPCSTR szFileName, BOOL bUnixFlag, BOOL bBakFile, BOOL bQuietMode, BOOL bTrim, BOOL bAllowZero);
int RecurseLoop(LPCSTR szFileName, BOOL bUnixFlag, BOOL bBakFile, BOOL bRecurse, BOOL bQuietMode, BOOL bTrim, BOOL bAllowZero);
BOOL IsDirectory(LPCSTR szFileName);


int main(int argc, char *argv[])
{
int i1, iRval = -1;
#ifdef WIN32
BOOL bUnixFlag = FALSE;
#else // WIN32
BOOL bUnixFlag = TRUE;
#endif // WIN32
BOOL bRecurse = FALSE;
BOOL bBakFile = TRUE;
BOOL bQuietMode = FALSE;
BOOL bTrim = FALSE;
BOOL bAllowZero = FALSE;

#ifdef WIN32
  if(argc < 2)
  {
    usage();
    return(0);
  }
#endif // WIN32

#ifndef HAVE_GETOPT
  for(optind=1; optind < argc; optind++)
  {
    if(argv[optind][0] != '-')
      break;

    if(argv[optind][1] == '-')
    {
      optind++;
      break;
    }

    if(argv[optind][1] == 'u' ||  // typically MS-DOS so be case insensitive
        argv[optind][1] == 'U')
    {
      bUnixFlag = TRUE;
    }
    else if(argv[optind][1] == '0')
    {
      bAllowZero = TRUE;
    }
    else if(argv[optind][1] == 'm' ||
            argv[optind][1] == 'M')
    {
      bUnixFlag = FALSE;
    }
    else if(argv[optind][1] == 'r' ||
            argv[optind][1] == 'R')
    {
      bRecurse = TRUE;
    }
    else if(argv[optind][1] == 'b' ||
            argv[optind][1] == 'B')
    {
      bBakFile = TRUE;
    }
    else if(argv[optind][1] == 'n' ||
            argv[optind][1] == 'N')
    {
      bBakFile = FALSE;
    }
    else if(argv[optind][1] == 'q' ||
            argv[optind][1] == 'Q')
    {
      bQuietMode = TRUE;
    }
    else if(argv[optind][1] == 'h' ||
            argv[optind][1] == 'H' ||
            argv[optind][1] == '?')
    {
      usage();
      return(1);
    }
    else if(argv[optind][1] == 't' ||
            argv[optind][1] == 'T')
    {
      bTrim = TRUE;
    }
    else
    {
      fprintf(stderr, "Illegal or unrecognized option\n");
      usage();
      return(1);
    }
  }
#else  // HAVE_GETOPT
  while((i1 = getopt(argc, argv, "humnbrqt0")) != -1)
  {
    switch(i1)
    {
      case '0': // allow 0-bytes
        bAllowZero = TRUE;
        break;
      case 'u': // unix-style (strip the CR's)
        bUnixFlag = TRUE;
        break;
      case 'm': // MS style (add the CR's)
        bUnixFlag = FALSE;
        break;
      case 'r': // recurse
        bRecurse = TRUE;
        break;
      case 'n': // no backup
        bBakFile = FALSE;
        break;
      case 'b': // create backup
        bBakFile = TRUE;
        break;
      case 'q': // quiet mode
        bQuietMode = TRUE;
        break;
      case 't': // trim white space
        bTrim = TRUE;
        break;
      default:
        fprintf(stderr, "Illegal or unrecognized option\n");
      case 'h':
        usage();
        return(1);
    }
  }
#endif // HAVE_GETOPT

  argc -= optind;
  argv += optind;

  if(argc <= 0)
  {
    usage();
    return(1);
  }

  for(i1=0, iRval = 0; i1 < argc; i1++)
  {
    // check file name for wildcards

    if(strchr(argv[i1], '?') ||
       strchr(argv[i1], '*') ||
       IsDirectory(argv[i1]))
    {
      iRval = RecurseLoop(argv[i1], bUnixFlag, bBakFile, bRecurse, bQuietMode, bTrim, bAllowZero);
        // use a directory listing

      if(!bQuietMode && (i1 + 1) < argc)
        fprintf(stderr, "\n");
    }
    else
    {
      if(!bQuietMode)
        fprintf(stderr, "%s  ", argv[i1]);

      iRval = MainLoop(argv[i1], bUnixFlag, bBakFile, bQuietMode, bTrim, bAllowZero);

      if(!bQuietMode)
        fprintf(stderr, "\n");
    }

    if(iRval)
      break;
  }

  return(iRval);
}

int AddStringToBuffer(char **ppBuf, int *pcbBuf, int *pcbBufCur, LPCSTR szString)
{
  int iLen = strlen(szString);

  if(!*ppBuf || (*pcbBufCur + iLen + 2) < *pcbBufCur)
  {
    if(!*ppBuf)
    {
      *ppBuf = malloc(*pcbBuf = 16384);  // initial length
      *pcbBufCur = 0;
    }
    else
    {
      int cbNewSize = (1 + ((*pcbBufCur + iLen + 8192) / 16384)) * 16384;
      char *pTemp = realloc(*ppBuf, cbNewSize);

      if(!pTemp)
      {
        fprintf(stderr, "Insufficient memory to continue\n");
        return(-1);  // error
      }

      *ppBuf = pTemp;
      *pcbBuf = cbNewSize;
    }
  }

  memcpy(*ppBuf + *pcbBufCur, szString, iLen);

  *pcbBufCur += iLen;
  *(*ppBuf + ((*pcbBufCur)++)) = 0;  // to mark end of string
  *(*ppBuf + *pcbBufCur) = 0; // to mark end of array

  return(0);  // success
}

void FreeBuffer(char **ppBuf)
{
  if(*ppBuf)
  {
    free(*ppBuf);
    *ppBuf = NULL;
  }
}

int RecurseLoop(LPCSTR szFileName, BOOL bUnixFlag, BOOL bBakFile, BOOL bRecurse, BOOL bQuietMode, BOOL bTrim, BOOL bAllowZero)
{
char tbuf[MAX_PATH * 2], tbuf2[MAX_PATH * 2];
char *p1, *pBuf = NULL, *pBuf2 = NULL;
int iRval = 0, cbBuf = 0, cbBufCur = 0, cbBuf2 = 0, cbBufCur2 = 0;
#ifdef WIN32
WIN32_FIND_DATA fd;
HANDLE hFF;
#else  // WIN32
DIR *hD;
char de[sizeof(struct dirent) + NAME_MAX + 2];
struct dirent *pD;
struct stat sF;
#endif // WIN32

  if((strlen(szFileName) + 4) > sizeof(tbuf))
  {
    fprintf(stderr, "ERROR:  file name too long - %s\n", szFileName);
    return(-1);
  }

  if(!strchr(szFileName, '?') &&
     !strchr(szFileName, '*') &&
     IsDirectory(szFileName))
  {
    // a directory name, but no wildcard stuff

    strcpy(tbuf, szFileName);
    p1 = tbuf + strlen(tbuf);  // point at end of string

    // at this point, p1 points to the file name or file name spec
    // and everything prior to that is the directory name

#ifdef WIN32
    if(p1 > tbuf && *(p1 - 1) != '\\')
      *(p1++) = '\\';  // point just past the '\' so I can build file names
    strcpy(p1, "*.*");
#else  // WIN32
    if(p1 > tbuf && *(p1 - 1) != '/')
      *(p1++) = '/';  // point just past the '/' so I can build file names
    *p1 = '*';
    *(p1 + 1) = 0;
#endif // WIN32

    // this adds the wildcard file spec on the end, dirname/*
  }
  else
  {
    // either a wildcard or a file name

    strcpy(tbuf, szFileName);

#ifdef WIN32
    p1 = strrchr(tbuf, '\\');
#else  // WIN32
    p1 = strrchr(tbuf, '/');
#endif // WIN32
    if(!p1)
    {
#ifdef WIN32
      p1 = tbuf;
      *(p1++) = '.';
      *(p1++) = '\\';
#else  // WIN32
      p1 = tbuf;
      *(p1++) = '.';
      *(p1++) = '/';
#endif // WIN32
      strcpy(p1, szFileName);  // build a path of './filename'
    }
    else
      p1++;  // points at the file name now
  }

#ifdef WIN32
  strcpy(tbuf2, p1);  // capture original file pattern spec

  if((hFF = FindFirstFile(tbuf, &fd))
      != INVALID_HANDLE_VALUE)
  {
    // note:  *nix behavior and WIN32 behavior are identical

    do
    {
      strcpy(p1, fd.cFileName);

      if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
        iRval = AddStringToBuffer(&pBuf, &cbBuf, &cbBufCur, tbuf);
      }
    } while(!iRval && FindNextFile(hFF, &fd));

    FindClose(hFF);
  }

  if(!iRval && bRecurse)
  {
    // now we look for sub-directories

    strcpy(p1, "*.*");  // and now use a wildcard file spec

    if((hFF = FindFirstFile(tbuf, &fd))
        != INVALID_HANDLE_VALUE)
    {
      do
      {
        strcpy(p1, fd.cFileName);

        if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          if(fd.cFileName[0] != '.' ||
            (fd.cFileName[1] &&
              (fd.cFileName[1] != '.' || fd.cFileName[2])))
          {
            if(bRecurse)
            {
              strcat(tbuf, "\\");
              strcat(tbuf, tbuf2);  // add file spec to directory name

              iRval = AddStringToBuffer(&pBuf2, &cbBuf2, &cbBufCur2, tbuf);
            }
          }
        }
      } while(!iRval && FindNextFile(hFF, &fd));

      FindClose(hFF);
    }
  }
#else  // WIN32
  if(p1 > (tbuf + 1) && *(p1 - 1) == '/')
  {
    *(p1 - 1) = 0;  // temporarily
    hD = opendir(tbuf);
    *(p1 - 1) = '/';
  }
  else
    hD = opendir(".");  // open current directory

  if(hD)
  {
    strcpy(tbuf2, p1);  // I need a copy of the pattern

    while(!readdir_r(hD, (struct dirent *)&de, &pD) && pD)
    {
      // skip '.' and '..'
      if(pD->d_name[0] == '.' &&
         (!pD->d_name[1] ||
          (pD->d_name[1] == '.' && !pD->d_name[2])))
      {
        continue;  // no '.' or '..'
      }

      strcpy(p1, pD->d_name);

      // This app differs from the *nix way of doing things in that
      // when I recurse a directory, I do not apply the filter to it.
      // In this way, using a command 'crlf -r "*.c" ' will recurse
      // all directories, and only modify the '.c' files that it finds.
      // Other possible uses of '-r' NOT fed by shell 'globbing' are so
      // impractical that I won't even bother trying to do it.  If you
      // want the 'glob' behavior, set globbing and don't quote the name.

      if(!stat(tbuf, &sF))
      {
        if(sF.st_mode & S_IFDIR)
        {
          if(bRecurse)
          {
            strcat(tbuf, "/");
            strcat(tbuf, tbuf2);  // add file spec to directory name

            iRval = AddStringToBuffer(&pBuf2, &cbBuf2, &cbBufCur2, tbuf);
          }
        }
        else if(!fnmatch(tbuf2, p1, FNM_PERIOD))  // 'tbuf2' is my pattern
        {
          iRval = AddStringToBuffer(&pBuf, &cbBuf, &cbBufCur, tbuf);
        }
      }
      else
        fprintf(stderr, "Warning:  can't 'stat' %s\n", tbuf);

      if(iRval)
        break;
    }

    closedir(hD);
  }
#endif // WIN32

  // at this point 'pBuf' points to a list of strings separated by '\0'
  // that match my file spec, and 'pBuf2' points to a similar list of
  // strings for the list of directories I need to recurse into.  I have
  // saved doing the actual work for last so that any changes I make to
  // the directory contents as a result of running the 'crlf' change will
  // not cause looping problems or skipped files.

  if(!iRval)
  {
    // first do files in THIS directory

    if(pBuf)
    {
      p1 = pBuf;
      while(!iRval && *p1)
      {
        if(!bQuietMode)
          fprintf(stderr, "%s  ", p1);

        iRval = MainLoop(p1, bUnixFlag, bBakFile, bQuietMode, bTrim, bAllowZero);
        p1 += strlen(p1) + 1;

        if(!bQuietMode)
          fprintf(stderr, "\n");
      }
    }

    // next, recurse sub-directories

    if(pBuf2 && bRecurse)
    {
      p1 = pBuf2;
      while(!iRval && *p1)
      {
        if(!bQuietMode)
          fprintf(stderr, "\nDirectory: %s\n", p1);

        iRval = RecurseLoop(p1, bUnixFlag, bBakFile, TRUE, bQuietMode, bTrim, bAllowZero);
        p1 += strlen(p1) + 1;
      }
    }
  }

  // free up resources

  FreeBuffer(&pBuf);
  FreeBuffer(&pBuf2);

  return(iRval);
}

const char * DoTrimZeros(const char *lpText, DWORD *pcbLine, char **ppBuf, DWORD *pcbBuf)
{
const char *p1;
char *pBuf = *ppBuf, *p2;
DWORD cb1, cb2, cbLine = *pcbLine;

  if(!pBuf || cbLine > *pcbBuf)
  {
    char *pTemp;
    DWORD cbNew = (*pcbLine + 1024 + 512) & ~1023L;

    if(!pBuf)
    {
      pTemp = malloc(cbNew);
    }
    else
    {
      pTemp = realloc(pBuf, cbNew);
    }

    if(pTemp)
    {
      *ppBuf = pTemp;
      pBuf = pTemp;
      *pcbBuf = cbNew;
    }
    else
    {
      return lpText; // just return this and don't process it
    }
  }

  for(p1=lpText, p2=pBuf, cb2=0, cb1=cbLine; cb1 > 0; cb1--, p1++)
  {
    if(*p1)
    {
      *(p2++) = *p1;
      cb2++;
    }
  }

  *pcbLine = cb2;
  return pBuf;
}


int MainLoop(LPCSTR szFileName, BOOL bUnixFlag, BOOL bBakFile, BOOL bQuietMode, BOOL bTrim, BOOL bAllowZero)
{
HANDLE hFile, hMap, hOut;
LPCSTR lpMap, lpPos, lp1;
DWORD cbFile, cb1, cb2, cbLine;
char tbuf[MAX_PATH * 2];
int iRval = -1;
DWORD cbBuf = 0, cbNew;
char *pBuf = NULL;
const char *pTemp;

#ifdef WIN32
  hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                     OPEN_EXISTING, 0, NULL);
#else // WIN32
#ifdef O_SHLOCK
  hFile = open(szFileName, O_RDONLY | O_SHLOCK);
#else // O_SHLOCK
  hFile = open(szFileName, O_RDONLY);
#endif // O_SHLOCK
#endif // WIN32

#ifdef WIN32
  if(hFile == INVALID_HANDLE_VALUE)
#else // WIN32
  if(hFile < 0)
#endif // WIN32
  {
    fprintf(stderr, "Can't open %s\n", szFileName);
    return(1);
  }

  strcpy(tbuf, szFileName);
  strcat(tbuf, ".tmp");

#ifdef WIN32
  hOut = CreateFile(tbuf, GENERIC_WRITE, 0, NULL,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#else // WIN32
#ifdef O_EXLOCK
  hOut = open(tbuf, O_CREAT | O_RDWR | O_EXLOCK,
#else // O_EXLOCK
  hOut = open(tbuf, O_CREAT | O_RDWR,
#endif // O_EXLOCK
              S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#endif // WIN32

#ifdef WIN32
  if(hOut == INVALID_HANDLE_VALUE)
#else // WIN32
  if(hOut < 0)
#endif // WIN32
  {
#ifdef WIN32
    CloseHandle(hFile);
#else // WIN32
    close(hFile);
#endif // WIN32

    fprintf(stderr, "Can't create %s\n", tbuf);
    return(2);
  }

  iRval = 999;  // will be assigned to zero on success

#ifdef WIN32
  cbFile = GetFileSize(hFile, NULL);
#else // WIN32
  cbFile = lseek(hFile, 0, SEEK_END);
  lseek(hFile, 0, SEEK_SET);
#endif // WIN32

#ifdef WIN32
  hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

  if(!hMap)
  {
    fprintf(stderr, "Unable to create file mapping\n");
    iRval = 3;
  }
  else
  {
    lpMap = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, cbFile);
#else // WIN32
#ifdef MAP_NOCORE
    lpMap = mmap(NULL, cbFile, PROT_READ, MAP_NOCORE | MAP_PRIVATE, hFile, 0);
#else // MAP_NOCORE
    lpMap = mmap(NULL, cbFile, PROT_READ, MAP_PRIVATE, hFile, 0);
#endif // MAP_NOCORE
#endif // WIN32

    if(!lpMap)
    {
      fprintf(stderr, "Unable to map view of file\n");
      iRval = 4;
    }
    else
    {
      lpPos = lpMap;
      iRval = 0;  // for later (when I finish without error, the return code will be zero)

      while(lpPos < (lpMap + cbFile))
      {
        lp1 = lpPos;
        while(lpPos < (lpMap + cbFile) && *lpPos != '\n')
        {
          lpPos++;
        }

        if(lpPos < (lpMap + cbFile) && *lpPos == '\n')
        {
          lpPos++;  // so that I point "just past" the newline
        }

        cb1 = lpPos - lp1;
        if(bTrim)
        {
          cbLine = cb1;
          while(cbLine > 0 && lp1[cbLine - 1] <= ' ')
          {
            cbLine--;
          }
        }
        else
        {
          cbLine = cb1;
        }

        if(lpPos > (lpMap + 1) && *(lpPos - 2) == '\r' && *(lpPos - 1) == '\n')
        {
          // the line ends in CRLF

          if(bTrim || bUnixFlag)
          {
            if(!bTrim)  // trimmed lines already exclude the line feed etc.
            {
              if(cbLine > 2)  // write all but the final 2 characters in the line
                cbLine -= 2;
              else
                cbLine = 0;
            }

            cbNew = cbLine;

            if(!bAllowZero)
            {
              pTemp = DoTrimZeros(lp1, &cbNew, &pBuf, &cbBuf);
            }
            else
            {
              pTemp = lp1;
            }

            if(cbNew/*cbLine*/ > 0 &&
#ifdef WIN32
               (!WriteFile(hOut, pTemp/*lp1*/, cbNew/*cbLine*/, &cb2, NULL) ||
#else  // WIN32
               (!(cb2 = write(hOut, pTemp/*lp1*/, cbNew/*cbLine*/)) ||
#endif // WIN32
                (cbNew/*cbLine*/) != cb2))
            {
              fprintf(stderr, "Write error on output file (b)\n");
              iRval = 6;

              break;
            }

            if(lpPos > lpMap && *(lpPos - 1) == '\n')  // so any final "no CRLF" section is left as-is
            {
              const char *pEnd;

              if(!bUnixFlag)
              {
                cb1 = 2;
                pEnd = "\r\n";
              }
              else
              {
                cb1 = 1;
                pEnd = "\n";
              }
#ifdef WIN32
              if(!WriteFile(hOut, pEnd, cb1, &cb2, NULL) ||
#else  // WIN32
              if(!(cb2 = write(hOut, pEnd, cb1)) ||
#endif // WIN32
                 cb2 != cb1)
              {
                fprintf(stderr, "Write error on output file (c)\n");
                iRval = 7;

                break;
              }
            }
          }
          else
          {
            cbNew = cb1;

            if(!bAllowZero)
            {
              pTemp = DoTrimZeros(lp1, &cbNew, &pBuf, &cbBuf);
            }
            else
            {
              pTemp = lp1;
            }

            // 'LF' found - write it 'as-is'
            if(cbNew > 0)
            {
#ifdef WIN32
              if(!WriteFile(hOut, pTemp/*lp1*/, cbNew/*cb1*/, &cb2, NULL) ||
#else  // WIN32
              if(!(cb2 = write(hOut, pTemp/*lp1*/, cbNew/*cb1*/)) ||
#endif // WIN32
                 cbNew/*cb1*/ != cb2)
              {
                fprintf(stderr, "Write error on output file (a)\n");
                iRval = 5;

                break;
              }
            }
          }
        }
        else
        {
          // the line ends in LF

          if(!bTrim && bUnixFlag)
          {
            cbNew = cb1;

            if(!bAllowZero)
            {
              pTemp = DoTrimZeros(lp1, &cbNew, &pBuf, &cbBuf);
            }
            else
            {
              pTemp = lp1;
            }

            if(cbNew > 0)
            {
              // 'just LF' found - write it 'as-is'
#ifdef WIN32
              if(!WriteFile(hOut, pTemp/*lp1*/, cbNew/*cb1*/, &cb2, NULL) ||
#else  // WIN32
              if(!(cb2 = write(hOut, pTemp/*lp1*/, cbNew/*cb1*/)) ||
#endif // WIN32
                 cbNew/*cb1*/ != cb2)
              {
                fprintf(stderr, "Write error on output file (a2)\n");
                iRval = 5;

                break;
              }
            }
          }
          else
          {
            if(!bTrim)  // trimmed lines already exclude the line feed etc.
            {
              if(cbLine > 1)  // write all but the final character in the line
                cbLine --;
              else
                cbLine = 0;
            }

            cbNew = cbLine;

            if(!bAllowZero)
            {
              pTemp = DoTrimZeros(lp1, &cbNew, &pBuf, &cbBuf);
            }
            else
            {
              pTemp = lp1;
            }

            if(cbNew/*cbLine*/ > 0 &&
#ifdef WIN32
               (!WriteFile(hOut, pTemp/*lp1*/, cbNew/*cbLine*/, &cb2, NULL) ||
#else  // WIN32
               (!(cb2 = write(hOut, pTemp/*lp1*/, cbNew/*cbLine*/)) ||
#endif // WIN32
                cbNew/*cbLine*/ != cb2))
            {
              fprintf(stderr, "Write error on output file (b2)\n");
              iRval = 6;

              break;
            }

            // write the CRLF now...
            if(lpPos > lpMap && *(lpPos - 1) == '\n')  // so any final "no LF" section is left as-is
            {
              const char *pEnd;

              if(!bUnixFlag)
              {
                cb1 = 2;
                pEnd = "\r\n";
              }
              else
              {
                cb1 = 1;
                pEnd = "\n";
              }
#ifdef WIN32
              if(!WriteFile(hOut, pEnd, cb1, &cb2, NULL) ||
#else  // WIN32
              if(!(cb2 = write(hOut, pEnd, cb1)) ||
#endif // WIN32
                 cb2 != cb1)
              {
                fprintf(stderr, "Write error on output file (c2)\n");
                iRval = 7;

                break;
              }
            }
          }
        }
      }
#ifdef WIN32
      UnmapViewOfFile(lpMap);
#else // WIN32
      munmap((void *)lpMap, cbFile);
#endif // WIN32
    }

#ifdef WIN32
    CloseHandle(hMap);
  }

  CloseHandle(hOut);
  CloseHandle(hFile);
#else // WIN32
  close(hOut);
  close(hFile);
#endif // WIN32

  if(pBuf)
  {
    free(pBuf);
    pBuf = NULL; // by convention
  }

  // CLEANUP SECTION

  if(iRval)
  {
#ifdef WIN32
    DeleteFile(tbuf);
#else // WIN32
    unlink(tbuf);
#endif // WIN32
  }
  else if(!bBakFile)
  {
#ifdef WIN32
    DeleteFile(szFileName);  // in preparation for rename
#else // WIN32
    unlink(szFileName);
#endif // WIN32

    strcpy(tbuf, szFileName);
    strcat(tbuf, ".tmp");

//    if(!bQuietMode)
//      fprintf(stderr, "Moving %s to %s", tbuf, szFileName);

#ifdef WIN32
    MoveFile(tbuf, szFileName);
#else // WIN32
    rename(tbuf, szFileName);
#endif // WIN32
  }
  else
  {
    strcpy(tbuf, szFileName);
    strcat(tbuf, ".bak");

#ifdef WIN32
    DeleteFile(tbuf);  // in preparation for rename
#else // WIN32
    unlink(tbuf);
#endif // WIN32

    if(!bQuietMode)
      fprintf(stderr, "(backing up %s as %s)", szFileName, tbuf);

#ifdef WIN32
    MoveFile(szFileName, tbuf);
#else // WIN32
    rename(szFileName, tbuf);
#endif // WIN32

    strcpy(tbuf, szFileName);
    strcat(tbuf, ".tmp");

#ifdef WIN32
    MoveFile(tbuf, szFileName);
#else // WIN32
    rename(tbuf, szFileName);
#endif // WIN32
  }

  return(iRval);
}

BOOL IsDirectory(LPCSTR szFileName)
{
BOOL bRval = FALSE;

#ifdef WIN32
WIN32_FIND_DATA fd;
HANDLE hFF;

  hFF = FindFirstFile(szFileName, &fd);

  if(hFF != INVALID_HANDLE_VALUE)
  {
    bRval = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    FindClose(hFF);
  }

#else // WIN32
struct stat sF;

  if(!stat(szFileName, &sF))
    bRval = (sF.st_mode & S_IFDIR) != 0;

#endif // WIN32

  return(bRval);
}
