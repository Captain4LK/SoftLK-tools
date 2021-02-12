/*
SLK_make - a build-system

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _TABLE_H_

#define _TABLE_H_

typedef struct
{
   char **entry;
   int used;
   int entries;
   int grow;
}Name_table;

typedef Name_table list;

int         table_size(const Name_table *t);
char       *table_element(Name_table *t, int index);
Name_table *table_create(int entries, int grow);
int         table_add(Name_table *t, char *item, int ref);
int         table_add_table(Name_table *t, const Name_table *tab, int ref);
void        table_destroy(Name_table *t);
void        table_remove(Name_table *t, int ref);
void        table_clear(Name_table *t);
int         table_find(Name_table *t, char *item);

#endif
