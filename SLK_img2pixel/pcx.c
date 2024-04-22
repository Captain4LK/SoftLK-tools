/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include "HLH.h"
//-------------------------------------

//Internal includes
#include "../external/HLH.h"
#include "img2pixel.h"
//-------------------------------------

//#defines
//-------------------------------------
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

void SLK_image32_write_pcx(FILE *f, const SLK_image32 *img, uint32_t *colors, int color_count)
{
   //Make indexed
   uint8_t *indices = calloc(img->w*img->h,1);
   for(int i = 0;i<img->w*img->h;i++)
   {
      uint32_t c = img->data[i];
      uint64_t dist_min = UINT64_MAX;
      int index = 0;
      for(int j = 0;j<color_count;j++)
      {
         int32_t cr = SLK_color32_r(c);
         int32_t cg = SLK_color32_g(c);
         int32_t cb = SLK_color32_b(c);
         int32_t pr = SLK_color32_r(colors[j]);
         int32_t pg = SLK_color32_g(colors[j]);
         int32_t pb = SLK_color32_b(colors[j]);
         uint64_t dist = (cr-pr)*(cr-pr)+(cg-pg)*(cg-pg)+(cb-pb)*(cb-pb);
         if(dist<dist_min)
         {
            dist_min = dist;
            index = j;
         }
      }

      indices[i] = index;
   }

   uint16_t val = 0;
   fputc(0x0a,f);
   fputc(0x05,f);
   fputc(0x01,f);
   fputc(0x08,f);
   val = 0; fwrite(&val,2,1,f);
   val = 0; fwrite(&val,2,1,f);
   val = img->w-1; fwrite(&val,2,1,f);
   val = img->h-1; fwrite(&val,2,1,f);
   val = img->w; fwrite(&val,2,1,f);
   val = img->h; fwrite(&val,2,1,f);
   for(int i = 0;i<48;i++)
      fputc(0x00,f);
   fputc(0x00,f);
   fputc(0x01,f);
   val = img->w; fwrite(&val,2,1,f);
   val = 0; fwrite(&val,2,1,f);
   val = 0; fwrite(&val,2,1,f);
   val = 0; fwrite(&val,2,1,f);
   for(int i = 0;i<54;i++)
      fputc(0x00,f);

   //RLE encode
   for(int y = 0;y<img->h;y++)
   {
      uint8_t current = indices[y*img->w];
      int length = 1;
      for(int x = 1;x<img->w;x++)
      {
         if(indices[y*img->w+x]==current)
         {
            length++;
         }
         else
         {
            if(length>1||current>191)
            {
               while(length>0)
               {
                  int part_len = HLH_min(length,63);
                  fputc(part_len|0xc0,f);
                  fputc(current,f);
                  length-=part_len;
               }
            }
            else
            {
               fputc(current,f);
            }
            length = 1;
            current = indices[y*img->w+x];
         }
      }
      if(length>0)
      {
         if(length>1||current>191)
         {
            while(length>0)
            {
               int part_len = HLH_min(length,63);
               fputc(part_len|0xc0,f);
               fputc(current,f);
               length-=part_len;
            }
         }
         else
         {
            fputc(current,f);
         }
      }
   }

   //Write palette
   fputc(0x0c,f);
   for(int i = 0;i<256;i++)
   {
      if(i>=color_count)
      {
         fputc(0x00,f);
         fputc(0x00,f);
         fputc(0x00,f);
      }
      else
      {
         fputc(SLK_color32_r(colors[i]),f);
         fputc(SLK_color32_g(colors[i]),f);
         fputc(SLK_color32_b(colors[i]),f);
      }
   }

   free(indices);
}
//-------------------------------------
