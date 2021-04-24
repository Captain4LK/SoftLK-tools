/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
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

#define MAX_DEPTH 8
//-------------------------------------

//Typedefs
typedef struct
{
   int r,g,b;
}Color;

typedef struct Node
{
   int level;
   uint8_t leaf;
   struct Node *next[8];
   int color_count;
   int color_index;
   Color rgb;
   struct Node **next_node;
}Node;
//-------------------------------------

//Variables
static int octree_depth = 0;
static int octree_size = 0;
static Node *octree;
static int allocs = 0;
static Node **reduce_list[MAX_DEPTH];
//-------------------------------------

//Function prototypes
static int get_bit(uint8_t bit, int value);
static int branch(Color rgb, int depth);
static void octree_init(Node **tree, int depth);
static void octree_insert(Node **tree, Color rgb, int depth);
static void octree_get_reducible(Node **tree);
static void octree_make_reducible(int level, Node **node);
static void octree_reduce();
static void octree_fill_palette(Node **node, SLK_Palette *pal);
static void octree_free(Node *node);
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
   octree_depth = 8;
   octree_size = 0;
   octree = NULL;
   allocs = 0;
   memset(reduce_list,0,sizeof(reduce_list));
   for(int i = 0;i<512*512;i++)
   {
      octree_insert(&octree,(Color){down_in->data[i].r,down_in->data[i].g,down_in->data[i].b},1);
      while(octree_size>colors) octree_reduce();
   }
   pal->used = 0;
   octree_fill_palette(&octree,pal);
   octree_free(octree);
   SLK_rgb_sprite_destroy(down_in);
}

static int get_bit(uint8_t bit, int value)
{
   return (value&(1<<bit))!=0;
}

static int branch(Color rgb, int depth)
{
   return get_bit(MAX_DEPTH-depth,rgb.r)*4+get_bit(MAX_DEPTH-depth,rgb.g)*2+get_bit(MAX_DEPTH-depth,rgb.b);
}

static void octree_init(Node **tree, int depth)
{
   *tree = malloc(sizeof(**tree));
   (*tree)->level = depth;
   (*tree)->leaf = depth>=octree_depth;
   (*tree)->next_node = NULL;
   (*tree)->color_count = 0;
   (*tree)->rgb.r = 0;
   (*tree)->rgb.g = 0;
   (*tree)->rgb.b = 0;
   for(int i = 0;i<8;i++)
      (*tree)->next[i] = NULL;
   if((*tree)->leaf)
   {
      octree_size++;

      allocs++;
      if(allocs>=(1<<24))
      {
         puts("Too many allocs, something went wrong");
         exit(-1);
      }
   }
   else
   {
      octree_make_reducible(depth,tree);
   }
}

static void octree_insert(Node **tree, Color rgb, int depth)
{
   if(*tree==NULL)
   {
      octree_init(tree,depth);
   }

   if((*tree)->leaf)
   {
      (*tree)->color_count++;
      (*tree)->rgb.r+=rgb.r;
      (*tree)->rgb.g+=rgb.g;
      (*tree)->rgb.b+=rgb.b;
   }
   else
   {
      octree_insert(&((*tree)->next[branch(rgb,depth)]),rgb,depth+1);
   }
}

static void octree_get_reducible(Node **tree)
{
   while(reduce_list[octree_depth-1]==NULL)
      octree_depth--;
   *tree = *reduce_list[octree_depth-1];
   reduce_list[octree_depth-1] = (*reduce_list[octree_depth-1])->next_node;
}

static void octree_make_reducible(int level, Node **node)
{
   Node **n = reduce_list[level];
   (*node)->next_node = n;
   reduce_list[level] = node;
}

static void octree_reduce()
{
   Node *tree = NULL;
   int children = 0;
   Color sum = {0};

   octree_get_reducible(&tree);
   for(int i = 0;i<8;i++)
   {
      if(tree->next[i]!=NULL)
      {
         children++; 
         sum.r+=(*tree->next[i]).rgb.r;
         sum.g+=(*tree->next[i]).rgb.g;
         sum.b+=(*tree->next[i]).rgb.b;
         tree->color_count++;
      }
   }
   tree->leaf = 1;
   tree->rgb = sum;
   octree_size = octree_size-children+1;
}

static void octree_fill_palette(Node **node, SLK_Palette *pal)
{
   if((*node)!=NULL)
   {
      if((*node)->leaf)
      {
         if((*node)->color_count!=0)
         {
            pal->colors[pal->used].r = (*node)->rgb.r/(*node)->color_count;
            pal->colors[pal->used].g = (*node)->rgb.g/(*node)->color_count;
            pal->colors[pal->used].b = (*node)->rgb.b/(*node)->color_count;
            pal->colors[pal->used].a = 255;
            pal->used++;
         }
      }
      else
      {
         for(int i = 0;i<8;i++)
            octree_fill_palette(&((*node)->next[i]),pal);
      }
   }
}

static void octree_free(Node *node)
{
   if((node)!=NULL)
   {
      for(int i = 0;i<8;i++)
         octree_free(((node)->next[i]));
      free(node);
   }
}
//-------------------------------------
