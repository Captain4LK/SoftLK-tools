/*
 HLH - base layer

 Written in 2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

 To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

//External includes
#include <stdbool.h>
//-------------------------------------

//Internal includes
#include "HLH_base.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations
HLH_string HLH_string_from_cstring(const char *cstr)
{
   if(cstr == NULL)
   {
      HLH_string str;
      str.size = 0;
      str.str = NULL;
      return str;
   }

   HLH_string str;
   str.str = cstr;
   str.size = strlen(cstr);

   return str;
}

HLH_string HLH_string_from_cslice(HLH_cslice_char slice)
{
   HLH_string str;
   str.str = slice.ptr;
   str.size = slice.size;

   return str;
}

// Allocates a new string, to be freed with HLH_string_delete
HLH_string HLH_string_clone(HLH_string str_src)
{
   HLH_string str;
   str.size = str_src.size;
   char *mem = calloc(str_src.size, 1);
   memcpy(mem, str_src.str, str.size);
   str.str = mem;

   return str;
}

HLH_string HLH_string_clone_cstring(const char *cstr)
{
   return HLH_string_clone(HLH_string_from_cstring(cstr));
}

void HLH_string_delete(HLH_string *str)
{
   free((void *)str->str);
   str->str = NULL;
   str->size = 0;
}

char *HLH_string_clone_to_cstring(HLH_string str_src)
{
   char *str = calloc(str_src.size + 1, 1);
   memcpy(str, str_src.str, str_src.size);

   return str;
}

bool HLH_string_equal(HLH_string a, HLH_string b)
{
   if(a.size != b.size)
   {
      return false;
   }

   return memcmp(a.str, b.str, a.size) == 0;
}
//-------------------------------------
