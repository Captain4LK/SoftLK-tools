/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//Code in this file has been taken from http://alpng.sourceforge.net/octree.html
//Original license:
/*
Octree quantization

Copyright (c) 2006 Michal Molhanec

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented;
     you must not claim that you wrote the original software.
     If you use this software in a product, an acknowledgment
     in the product documentation would be appreciated but
     is not required.

  2. Altered source versions must be plainly marked as such,
     and must not be misrepresented as being the original software.

  3. This notice may not be removed or altered from any
     source distribution.
*/

//External includes
#include <stdlib.h>
#include <string.h>
#include <SLK/SLK.h>
#include "../../external/SLK_gui.h"
//-------------------------------------

//Internal includes
//-------------------------------------

//#defines
#define MIN(a,b) \
   ((a)>(b)?(b):(a))

#define BITS_USED 8
//-------------------------------------

//Typedefs
typedef struct octree_node 
{
   uint32_t r, g, b;
   uint32_t counter;
   int leaf;
   int leaf_parent;
   struct octree_node *subnodes[8];
   int palette_entry;
   struct octree_node *prev;
   struct octree_node *next;
   struct octree_node *parent;
}octree_node;

typedef struct octree_tree 
{
   struct octree_node *root;
   uint32_t number_of_leaves;
   struct octree_node *leaves_parents;
}octree_tree;
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static void octree_quant(SLK_Palette *pal, int colors, SLK_RGB_sprite *in);
static octree_node* octree_create_node(octree_node *parent);
static int octree_insert_pixel(octree_tree *tree, int r, int g, int b);
static uint32_t octree_calc_counters(octree_node *node);
static struct octree_node* octree_find_smallest(octree_tree *tree, uint32_t *last_min);
static void octree_reduce(octree_tree *tree, int colors);
static void octree_fill_palette(SLK_Palette *pal, int *index, octree_tree *tree);
static void octree_free_node(octree_node *node);
//-------------------------------------

//Function implementations

void quantize(SLK_Palette *pal, int colors, SLK_RGB_sprite *in)
{
   if(in==NULL||pal==NULL)
      return;
   memset(pal->colors,0,sizeof(*pal->colors)*256);
   pal->colors[0].r = 255;
   pal->colors[0].g = 0;
   pal->colors[0].b = 255;
   colors = MIN(256,colors); 
   if(colors<=1)
      return;

   SLK_RGB_sprite *down_in = SLK_rgb_sprite_create(512,512);
   float fw = (float)(in->width-1)/(float)512;
   float fh = (float)(in->height-1)/(float)512;
   for(int y = 0;y<512;y++)
   {
      for(int x = 0;x<512;x++)
      {
         SLK_Color c = SLK_rgb_sprite_get_pixel(in,((float)x*fw),((float)y*fh));
         down_in->data[y*512+x].r = c.r;
         down_in->data[y*512+x].b = c.b;
         down_in->data[y*512+x].g = c.g;
         down_in->data[y*512+x].a = c.a;
      }
   }
   octree_quant(pal,colors,down_in);
   SLK_rgb_sprite_destroy(down_in);
}

static void octree_quant(SLK_Palette *pal, int colors, SLK_RGB_sprite *in)
{
   struct octree_tree tree;
   int x,y;
   int i = 0;
   int r,g,b;

   tree.number_of_leaves = 0;
   tree.root = octree_create_node(0);
   tree.leaves_parents = 0;

   for(y = 0;y<in->height;y++)
   {
      for(x = 0;x<in->width;x++)
      {
         r = in->data[y*in->width+x].r;
         g = in->data[y*in->width+x].g;
         b = in->data[y*in->width+x].b;
         if(!octree_insert_pixel(&tree,r,g,b))
         {
            octree_free_node(tree.root);
            return;
         }
      }
   }

   octree_reduce(&tree,colors);
   octree_fill_palette(pal,&i,&tree);
}

static octree_node* octree_create_node(octree_node *parent)
{
   octree_node *n = calloc(1,sizeof(*n));
   if(n)
      n->parent = parent;

   return n;
}

static int octree_insert_pixel(octree_tree *tree, int r, int g, int b)
{
   int mask;
   int r_bit, g_bit, b_bit;
   int index;
   int i;
   octree_node *node = tree->root;
   for(i = BITS_USED;i>=0;i--)
   {
      mask =  1<<i;
      r_bit = (r&mask)>>i;
      g_bit = (g&mask)>>i;
      b_bit = (b&mask)>>i;
      index = (r_bit<<2)+(g_bit<<1)+b_bit;

      if(!node->subnodes[index])
      {
         node->subnodes[index] = octree_create_node(node);
         if (!node->subnodes[index])
            return 0;
      }
      node = node->subnodes[index];
   }
   if(node->counter==0) 
   {
      tree->number_of_leaves++;
      node->leaf = 1;
      if (!node->parent->leaf_parent)
      {
         node->parent->leaf_parent = 1;
         if (tree->leaves_parents)
            tree->leaves_parents->prev = node->parent;
         node->parent->next = tree->leaves_parents;
         tree->leaves_parents = node->parent;
      }
   }

   node->counter++;
   node->r+=r;
   node->g+=g;
   node->b+=b;
   return 1;
}

static uint32_t octree_calc_counters(octree_node *node)
{
   int i;
   if (node->leaf)
      return node->counter;

   for(i = 0;i<8;i++)
      if(node->subnodes[i])
         node->counter += octree_calc_counters(node->subnodes[i]);

   return node->counter;
}

static struct octree_node *octree_find_smallest(octree_tree *tree, uint32_t *last_min)
{
   octree_node *min = tree->leaves_parents;
   octree_node *n = tree->leaves_parents->next;

   while(n!=0)
   {
      if(min->counter==*last_min)
         return min;
      if(n->counter<min->counter)
         min = n;
      n = n->next;
   }
   *last_min = min->counter;
   return min;
}

static void octree_reduce(octree_tree *tree, int colors)
{
   octree_node *n;
   uint32_t min = 1;
   int i;
   if(tree->number_of_leaves<=colors)
      return;
   octree_calc_counters(tree->root);
   while(tree->number_of_leaves>colors)
   {
      n = octree_find_smallest(tree,&min);
      for(i = 0;i<8;i++)
      {
         if(n->subnodes[i])
         {
            n->r+=n->subnodes[i]->r;
            n->g+=n->subnodes[i]->g;
            n->b+=n->subnodes[i]->b;
            free(n->subnodes[i]);
            n->subnodes[i] = 0;
            tree->number_of_leaves--;
         }
      }
      tree->number_of_leaves++;
      n->leaf = 1;
      if(!n->parent->leaf_parent)
      {
         n->parent->leaf_parent = 1;
         n->parent->next = n->next;
         n->parent->prev = n->prev;
         if(n->prev)
            n->prev->next = n->parent;
         else
            tree->leaves_parents = n->parent;
         if(n->next)
            n->next->prev = n->parent;
      }
      else
      {
         if(n->prev)
            n->prev->next = n->next;
         else
            tree->leaves_parents = n->next;
         if(n->next)
            n->next->prev = n->prev;
      }
   }
}

static void octree_fill_palette(SLK_Palette *pal, int *index, octree_tree *tree)
{
   int i;
   octree_node *n = tree->leaves_parents;
   while(n)
   {
      for(i = 0;i<8;i++)
      {
         if(n->subnodes[i]&&n->subnodes[i]->leaf)
         {
            pal->colors[*index].r = n->subnodes[i]->r/n->subnodes[i]->counter;
            pal->colors[*index].g = n->subnodes[i]->g/n->subnodes[i]->counter;
            pal->colors[*index].b = n->subnodes[i]->b/n->subnodes[i]->counter;
            pal->colors[*index].a = 255;
            n->subnodes[i]->palette_entry = (*index)++;
         }
      }
      n = n->next;
   }
}

static void octree_free_node(octree_node *node)
{
   int i;
   for(i = 0;i<8;i++)
      if(node->subnodes[i])
         octree_free_node(node->subnodes[i]);
   free(node);
}
//-------------------------------------
