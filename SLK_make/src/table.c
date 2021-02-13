/*
SLK_make - a build-system

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
//-------------------------------------

//Internal includes
#include "error.h"
#include "table.h"
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

int table_size(const Name_table * t)
{
   return t->used;
}

char *table_element(Name_table * t, int index)
{
   assert(index >= 0 && index < t->used, "Name_table out of bounds");
   return t->entry[index];
}

Name_table *table_create(int entries, int grow)
{
   Name_table *t = malloc(sizeof(*t));
   t->entries = entries;
   t->grow = grow;
   t->entry = NULL;
   t->used = 0;
   if(entries > 0)
      t->entry = malloc(sizeof(*t->entry) * entries);
   else
      t->entry = malloc(sizeof(*t->entry) * grow);
   assert(t->entry, "Malloc failed");

   return t;
}

int table_add(Name_table * t, char *item, int ref)
{
   assert(item, "Bad item add");
   if(ref < 0)
      ref += t->used + 1;
   assert(ref >= 0 && ref <= t->used, "Bad item referenced");

   if(t->used >= t->entries)
   {
      if(t->grow)
      {
         t->entries += t->grow;
         char **new_entry =
            realloc(t->entry, sizeof(*new_entry) * t->entries);
         assert(new_entry, "Out of memory");
         t->entry = new_entry;
      }
   }

   for(int i = t->used; i > ref; i--)
      t->entry[i] = t->entry[i - 1];
   t->entry[ref] = item;
   t->used++;

   return ref;
}

int table_add_table(Name_table * t, const Name_table * tab, int ref)
{
   if(ref < 0)
      ref += t->used + 1;
   assert(ref >= 0 && ref <= t->used, "Bad item referenced");

   if(t->used + table_size(tab) >= t->entries + t->grow)
   {
      if(t->grow)
      {
         if(t->used + table_size(tab) >= t->entries + t->grow)
            t->entries = t->used + table_size(tab);
         else
            t->entries += t->grow;

         char **new_entry =
            realloc(t->entry, sizeof(*new_entry) * t->entries);

         assert(new_entry, "table::out of memory");

         t->entry = new_entry;
      }
      else
         assert(0, "table out of entries");
   }

   for(int i = t->used - 1; i > ref; i--)
      t->entry[i + table_size(tab)] = t->entry[i];
   for(int i = 0; i < table_size(tab); i++)
      t->entry[ref + i] = tab->entry[i];

   t->used += table_size(tab);

   return ref;
}

void table_destroy(Name_table * t)
{
   free(t->entry);
   free(t);
}

void table_remove(Name_table * t, int ref)
{
   assert(ref >= 0 && ref < t->used, "table bad item deletion");

   t->used--;
   for(int i = ref; i < t->used; i++)
      t->entry[i] = t->entry[i + 1];
}

void table_clear(Name_table * t)
{
   t->used = 0;
}

int table_find(Name_table * t, char *item)
{
   for(int i = 0; i < table_size(t); i++)
      if(strcmp(item, t->entry[i]) == 0)
         return i;
   return -1;
}

//-------------------------------------
