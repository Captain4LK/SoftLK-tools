/*
SLK_make - a build-system

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _ARRAY_TREE_H_

#define _ARRAY_TREE_H_

typedef struct
{
   int left, right;
   unsigned long checksum;
   unsigned long mod_time;
} mk_file_mod_node;

typedef struct
{
   int used;
   mk_file_mod_node nodes[5000];
} Array_tree;

void array_tree_add(Array_tree * t, mk_file_mod_node n);
int array_tree_find(Array_tree * t, mk_file_mod_node n);
mk_file_mod_node *array_tree_get(Array_tree * t, int n);

#endif
