/*
 * HLH - base layer
 *
 * Written in 2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com
 *
 * To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

//External includes
//-------------------------------------

//Internal includes
//-------------------------------------

//#defines
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

FILE *HLH_fopen(const char *path, const char *mode)
{
#ifdef _WIN32
   int size_wpath = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
   wchar_t *wpath = calloc(size_wpath, sizeof(*wpath));
   MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, size_wpath);

   int size_wmode = MultiByteToWideChar(CP_UTF8, 0, mode, -1, NULL, 0);
   wchar_t *wmode = calloc(size_wmode, sizeof(*wmode));
   MultiByteToWideChar(CP_UTF8, 0, mode, -1, wmode, size_wmode);

   FILE *f = _wfopen(wpath, wmode);

   free(wmode);
   free(wpath);

   return f;
#else
   return fopen(path, mode);
#endif
}
//-------------------------------------

