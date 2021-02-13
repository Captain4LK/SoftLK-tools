/*
SLK_make - a build-system

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//-------------------------------------

//Internal includes
#define CUTE_PATH_IMPLEMENTATION
#include "../../external/cute_path.h"
#include "table.h"
#include "deps.h"
#include "SLK_make.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
enum
{ MAX_SRC = 10000 };
Name_table *src_files;
Name_table *src_deps[MAX_SRC];
//-------------------------------------

//Function prototypes

static int find_src(char *filename);
//-------------------------------------

//Function implementations

static int find_src(char *filename)
{
   for(int i = 0; i < table_size(src_files); i++)
      if(strcmp(table_element(src_files, i), filename) == 0)
         return i;
   return -1;
}

Name_table *get_deps(char *filename, Name_table * includes)
{
   int f = find_src(filename);
   if(f != -1)
      return src_deps[f];

   FILE *fp = fopen(filename, "rb");
   if(!fp)
      return 0;
   fseek(fp, 0, SEEK_END);
   int size = ftell(fp);
   fseek(fp, 0, SEEK_SET);

   char *mem = malloc(size + 1);
   fread(mem, 1, size, fp);
   fclose(fp);
   mem[size] = 0;

   int x = table_add(src_files, strdup(filename), -1);
   table_add(src_deps[x], strdup(filename), -1);

   char *p = mem, name2[100], *s, name3[255];

   while(*p)
   {
      if(p[0] == '#' &&
         p[1] == 'i' &&
         p[2] == 'n' &&
         p[3] == 'c' &&
         p[4] == 'l' &&
         p[5] == 'u' && p[6] == 'd' && p[7] == 'e' && p[8] == ' ')
      {
         p += 9;

         while(*p == ' ')
            p++;
         if(*p == '"')
         {
            p++;
            s = p;
            while(*p && *p != '"')
               p++;

            if(*p)
            {
               *p = 0;
               p++;
            }
            
            path_pop(filename,name3,NULL);
            strcat(name3,"/");
            strcat(name3,s);
            Name_table *ret = get_deps(name3, includes);

            for(int j = 0; !ret && j < table_size(includes); j++)
            {
               sprintf(name2, "%s/%s", table_element(includes, j), s);
               ret = get_deps(name2, includes);
            }

            if(ret)
            {
               for(int i = 0; i < table_size(ret); i++)
               {
                  int found = 0;
                  for(int j = 0; j < table_size(src_deps[x]); j++)
                     if(strcmp
                        (table_element(src_deps[x], j),
                         table_element(ret, i)) == 0)
                        found = 1;
                  if(!found)
                     table_add(src_deps[x], table_element(ret, i), -1);
               }
            }
            else
            {
               // ignore files with a '~' because they are for name mangled 8.3 systems
               int t = 0;
               for(char *q = s; *q; q++)
                  if(*q == '~')
                     t = 1;
               if(!t)
                  fprintf(stderr, "%s : couldn't find include %s\n", filename,
                          s);
            }
         }
      }
      else
         p++;
   }

   free(mem);

   return src_deps[x];
}

//-------------------------------------
