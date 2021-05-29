/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//Based on Golgotha public domain release

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
enum
{
   HIST_SIZE = 0x10000,
   MAX_COLORS = 256,
};

typedef struct
{
   uint16_t reference[HIST_SIZE];
   uint32_t counts[HIST_SIZE];
   uint32_t tcolors;              // total original colors in the histogram [length of reference]
   uint32_t total_pixels;     
}Histogramm;

typedef struct
{
   uint32_t index;
   uint32_t colors;
   uint32_t sum;
}Box;
//-------------------------------------

//Variables
static uint32_t *counts;
//-------------------------------------

//Function prototypes
static void increment_color(Histogramm *h, uint16_t color, uint32_t count);
static inline void _16_to_rgb(uint32_t rgb, uint8_t *r, uint8_t *g, uint8_t *b);
static int red_compare(const void *a, const void *b);
static int green_compare(const void *a, const void *b);
static int blue_compare(const void *a, const void *b);
static int box_compare(const void *a, const void *b);
//-------------------------------------

//Function implementations

void quantize(SLK_Palette *pal, int colors, SLK_RGB_sprite *in)
{
   if(in==NULL||pal==NULL)
      return;

   Histogramm *hist = malloc(sizeof(*hist));
   memset(hist,0,sizeof(*hist));
   pal->used = colors;

   //Add colors to histogram
   uint32_t loop_count = in->width*in->height;
   SLK_Color *pixel = in->data;
   for(;loop_count;loop_count--)
   {
      SLK_Color c = *pixel;
      //convert the color to 16 bit
      uint16_t col = (((uint16_t)c.b>>3)<<11)|
                     (((uint16_t)c.g>>2)<<5)|
                     ((uint16_t)c.r>>3);

      increment_color(hist,col,1);

      //move to the next pixel in the image
      ++pixel;
   }

   //Median cut
   counts = hist->counts;

   Box *box_list = malloc(sizeof(Box)*MAX_COLORS);

   //setup the initial box
   uint32_t total_boxes    = 1;
   box_list[0].index  = 0;
   box_list[0].colors = hist->tcolors;
   box_list[0].sum    = hist->total_pixels;

   uint32_t box_index;

   // split boxes until we have all the colors
   while(total_boxes<colors-0/*skip_colors*/)
   {
      // Find the first splittable box.
      for(box_index = 0; box_index < total_boxes; ++box_index )
         if (box_list[box_index].colors >= 2)
            break;

      if( box_index == total_boxes)
         break;	/* ran out of colors! */


      uint32_t start = box_list[box_index].index;
      uint32_t end   = start + box_list[box_index].colors;

      uint8_t  min_r=0xff, max_r=0x00,
      min_g=0xff, max_g=0x00,
      min_b=0xff, max_b=0x00,
      r,g,b;


      // now find the minimum and maximum r g b values for this box
      for (loop_count = start; loop_count<end; loop_count++)
      {
         _16_to_rgb(hist->reference[loop_count],&r,&g,&b);

         if (r>max_r) max_r=r;
         if (r<min_r) min_r=r;

         if (g>max_g) max_g=g;
         if (g<min_g) min_g=g;

         if (b>max_b) max_b=b;
         if (b<min_b) min_b=b;
      }


      // Find the largest dimension, and sort by that component. 
      if (((max_r - min_r) >= (max_g - min_g)) && ((max_r - min_r) >= (max_b - min_b)))
         qsort(hist->reference + start,
               end-start,             // total elements to sort
               sizeof(uint16_t),
               red_compare);
      else if ( (max_g - min_g) >= (max_b - min_b) )
         qsort(hist->reference + start,
               end-start,             // total elements to sort
               sizeof(uint16_t),
               green_compare);
      else
         qsort(hist->reference + start,
               end-start,             // total elements to sort
               sizeof(uint16_t),
               blue_compare);



      // now find the division which closest divides into an equal number of pixels
      uint32_t low_count= counts[hist->reference[start]];
      uint32_t mid_number=box_list[box_index].sum/2;

      for(loop_count = start+1; loop_count<end-1&&low_count<mid_number;loop_count++)
         low_count+=counts[hist->reference[loop_count]];


      // now split the box
      box_list[total_boxes].index  = loop_count;
      box_list[total_boxes].colors = end-loop_count;
      box_list[total_boxes].sum    = box_list[box_index].sum-low_count;
      total_boxes++;

      box_list[box_index].colors= loop_count-start;
      box_list[box_index].sum   = low_count;

      // sort to bring the biggest boxes to the top
      //qsort(box_list, total_boxes, sizeof(Box), box_compare );    

      /*    for (int z=0;z<total_boxes;z++)
      {
      printf("box #%d : index = %d, colors= %d, sum=%d\n",z, 
      box_list[z].index, box_list[z].colors, box_list[z].sum);
      } */

   }


   // we should have 256 boxes, we now need to choose a color for each box
   // we do this by averaging all the colors within the box
   uint32_t r_tot, g_tot, b_tot;
   for (box_index = 0; box_index<total_boxes-0/*skip_colors*/; box_index++)
   {
      r_tot = g_tot = b_tot = 0;

      uint32_t start = box_list[box_index].index;
      uint32_t end   = start + box_list[box_index].colors;

      uint8_t r,g,b;
      for (loop_count = start; loop_count < end; loop_count++)
      {
         _16_to_rgb(hist->reference[loop_count],&r,&g,&b);

         r_tot+=r;
         g_tot+=g;
         b_tot+=b;
      }

      r_tot/=(end-start);
      g_tot/=(end-start);
      b_tot/=(end-start);

      pal->colors[box_index+0/*skip_colors*/].n = (r_tot<<16)|
      (g_tot<<8) |
      b_tot;
      pal->colors[box_index+0/*skip_colors*/].a = 255;
   }

   for(;box_index+0/*skip_colors*/<colors; box_index++)
      pal->colors[box_index+0/*skip_colors*/].n = 0;

   free(box_list);
}

static void increment_color(Histogramm *h, uint16_t color, uint32_t count)
{
   if(!h->counts[color])            // is this an original color?
   {
      h->reference[h->tcolors] = color;    // add this color to the reference list
      h->tcolors++;
   }
   h->counts[color]+=count;          // increment the counter for this color
   h->total_pixels+=count;           // count total pixels we've looked at
}

static inline void _16_to_rgb(uint32_t rgb, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *b=(rgb & (1 | 2 | 4 | 8 | 16))<<3;
  rgb>>=5;

  *g=(rgb & (1 | 2 | 4 | 8 | 16 | 32))<<2;
  rgb>>=6;

  *r=(rgb & (1 | 2 | 4 | 8 | 16))<<3;
}

static int red_compare(const void *a, const void *b)
{
  uint16_t color1=counts[*((uint16_t *)a)],
      color2=counts[*((uint16_t *)b)];

  return ((int32_t) ((color1 & (  (1 | 2 | 4 | 8 | 16) << 11))>>11))-
    ((int32_t) ((color2 & (  (1 | 2 | 4 | 8 | 16) << 11))>>11));
}


static int green_compare(const void *a, const void *b)
{
  uint16_t color1=counts[*((uint16_t *)a)],
      color2=counts[*((uint16_t *)b)];

  return ((int32_t) (color1 & (  (1 | 2 | 4 | 8 | 16 | 32) << 5)))-
         ((int32_t) (color2 & (  (1 | 2 | 4 | 8 | 16 | 32) << 5)));
}


static int blue_compare(const void *a, const void *b)
{
  uint16_t color1=counts[*((uint16_t *)a)],
      color2=counts[*((uint16_t *)b)];

  return ((int32_t) (color1 & (  (1 | 2 | 4 | 8 | 16) << 0)))-
         ((int32_t) (color2 & (  (1 | 2 | 4 | 8 | 16) << 0)));
}

static int box_compare(const void *a, const void *b)
{
  return ((Box *)b)->sum-((Box *)a)->sum;
}
//-------------------------------------
