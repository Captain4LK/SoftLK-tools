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
#include "array_tree.h"
#include "error.h"
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

void array_tree_add(Array_tree * t, mk_file_mod_node n)
{
   int i = 0;

   if(t->used + 1 == 5000)
      mk_error("array tree full");
   else if(t->used)
   {
      int parent = 0, found = 0;

      while(i != -1)
      {
         parent = i;
         if(n.checksum < t->nodes[i].checksum)
            i = t->nodes[i].left;
         else if(n.checksum > t->nodes[i].checksum)
            i = t->nodes[i].right;
         else
            return;             // already in tree               
      }

      i = t->used;
      if(n.checksum < t->nodes[parent].checksum)
         t->nodes[parent].left = i;
      else
         t->nodes[parent].right = i;
   }

   t->nodes[i] = n;
   t->nodes[i].left = -1;
   t->nodes[i].right = -1;
   t->used++;
}

int array_tree_find(Array_tree * t, mk_file_mod_node n)
{
   if(!t->used)
      return -1;

   for(int i = 0;;)
   {
      if(n.checksum < t->nodes[i].checksum)
         i = t->nodes[i].left;
      else if(n.checksum > t->nodes[i].checksum)
         i = t->nodes[i].right;
      else
         return i;

      if(i == -1)
         return -1;
   }
}

mk_file_mod_node *array_tree_get(Array_tree * t, int n)
{
   return &t->nodes[n];
}

//-------------------------------------
