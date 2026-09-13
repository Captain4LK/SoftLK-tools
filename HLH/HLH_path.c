/*
 * HLH - base layer - path manipulation
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
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations
HLH_string HLH_path_directory(HLH_string path)
{
   if(path.size == 0)
   {
      return HLH_string_lit("");
   }

   HLH_string dir = {};
   HLH_string filename = {};
   HLH_path_split(path, &dir, &filename);
   return dir;
}

void HLH_path_split(HLH_string path, HLH_string dir[static 1], HLH_string filename[static 1])
{
   HLH_ssize i = 0;
   for(i = path.size - 1; i >= 0 && path.str[i] != '/' && path.str[i] != '\\'; i -= 1);
   if(i == 0)
   {
      dir->str = path.str;
      dir->size = i + 1;
      filename->str = path.str + dir->size;
      filename->size = HLH_max(0, path.size - dir->size);

      return;
   }
   if(i > 0)
   {
      dir->str = path.str;
      dir->size = i;
      filename->str = path.str + i + 1;
      filename->size = HLH_max(0, path.size - i - 1);

      return;
   }

   *dir = HLH_string_lit("");
   *filename = path;
}

HLH_string HLH_path_ext(HLH_string path)
{
   HLH_ssize i = 0;
   for(i = path.size - 1; i >= 0 && path.str[i] != '/' && path.str[i] != '\\'; i -= 1)
   {
      if(path.str[i] == '.')
      {
         return (HLH_string){.str = path.str + i, .size = path.size - i};
      }
   }

   return HLH_string_lit("");
}

HLH_string HLH_path_stem(HLH_string path)
{
   if(path.size == 0)
   {
      return HLH_string_lit("");
   }

   char last = path.str[path.size - 1];
   if(last == '/' || last == '\\')
   {
      return HLH_string_lit("");
   }

   HLH_string stem = HLH_path_base(path);
   HLH_ssize idx;
   for(idx = stem.size - 1; idx >= 0 && stem.str[idx] != '/' && stem.str[idx] != '\\'; idx -= 1);
   if(idx != -1)
   {
      stem = (HLH_string){.str = stem.str + idx + 1, .size = stem.size - (idx + 1)};
   }

   for(idx = stem.size - 1; idx >= 0 && stem.str[idx] != '.'; idx -= 1);
   if(idx != -1)
   {
      stem = (HLH_string){.str = stem.str, .size = idx};
   }

   return stem;
}

HLH_string HLH_path_base(HLH_string path)
{
   if(path.size == 0)
   {
      return HLH_string_lit("");
   }

   HLH_string dir = {};
   HLH_string filename = {};
   HLH_path_split(path, &dir, &filename);
   return filename;
}
//-------------------------------------

