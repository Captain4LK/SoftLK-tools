/*
 HLH - base layer

 Written in 2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

 To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef _HLH_BASE_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define _HLH_BASE_H_

#define HLH_slice_define(type, name) \
typedef struct \
{\
   type *ptr; \
   size_t size; \
}HLH_slice_ ## name ; \
\
typedef struct \
{\
   const type *ptr; \
   size_t size; \
}HLH_cslice_ ## name;
//HLH_slice_ ## name HLH_slice_range_ ## name (type *ptr, size_t first, size_t last);

#define HLH_slice_types \
HLH_X(char, char)

#define HLH_X(type, name) HLH_slice_define(type, name)
HLH_slice_types
#undef HLH_X

typedef intptr_t HLH_ssize;

// Utility functions
// ----------------
#define HLH_max(a, b) ((a) > (b) ? (a) : (b))
#define HLH_min(a, b) ((a) < (b) ? (a) : (b))
// ----------------

// Strings
// ----------------
typedef struct
{
   const char *str;
   size_t size;
}HLH_string;

#define HLH_string_lit(s) (HLH_string){.str = s, .size = sizeof(s) - 1}

// Non owning, uses passed memory
HLH_string HLH_string_from_cstring(const char *cstr);
HLH_string HLH_string_from_cslice(HLH_cslice_char slice);

// Allocates a new string, to be freed with HLH_string_delete
HLH_string HLH_string_clone(HLH_string str_src);
HLH_string HLH_string_clone_cstring(const char *cstr);

// Allocates a new c string, to be free with free()
char *HLH_string_clone_to_cstring(HLH_string str_src);

void HLH_string_delete(HLH_string *str);

bool HLH_string_equal(HLH_string a, HLH_string b);
// ----------------

// OS specific replacements (aka windows hall of shame)
// ----------------
FILE *HLH_fopen(const char *path, const char *mode);
// ----------------

#endif
