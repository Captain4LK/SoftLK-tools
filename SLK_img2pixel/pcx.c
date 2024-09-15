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
#include "HLH_rw.h"
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

   HLH_rw rw = {0};
   HLH_rw_init_file(&rw,f);
   //Header
   HLH_rw_write_u8(&rw,0x0a);
   HLH_rw_write_u8(&rw,0x05);
   HLH_rw_write_u8(&rw,0x01);
   HLH_rw_write_u8(&rw,0x08);
   HLH_rw_write_u16(&rw,0x0000);
   HLH_rw_write_u16(&rw,0x0000);
   HLH_rw_write_u16(&rw,(uint16_t)(img->w-1));
   HLH_rw_write_u16(&rw,(uint16_t)(img->h-1));
   HLH_rw_write_u16(&rw,0x0048);
   HLH_rw_write_u16(&rw,0x0048);
   for(int i = 0;i<48;i++)
      HLH_rw_write_u8(&rw,0x00);
   HLH_rw_write_u8(&rw,0x00);
   HLH_rw_write_u8(&rw,0x01);
   HLH_rw_write_u16(&rw,(uint16_t)img->w);
   HLH_rw_write_u16(&rw,0x0001);
   HLH_rw_write_u16(&rw,0x0000);
   HLH_rw_write_u16(&rw,0x0000);
   for(int i = 0;i<54;i++)
      HLH_rw_write_u8(&rw,0x00);

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
            while(length>0)
            {
               if(length>1||current>191)
               {
                  int part_len = HLH_min(length,63);
                  HLH_rw_write_u8(&rw,(uint8_t)(part_len|0xc0));
                  HLH_rw_write_u8(&rw,current);
                  length-=part_len;
               }
               else
               {
                  HLH_rw_write_u8(&rw,current);
                  length--;
               }
            }
            length = 1;
            current = indices[y*img->w+x];
         }
      }
      while(length>0)
      {
         if(length>1||current>191)
         {
            int part_len = HLH_min(length,63);
            HLH_rw_write_u8(&rw,(uint8_t)(part_len|0xc0));
            HLH_rw_write_u8(&rw,current);
            length-=part_len;
         }
         else
         {
            HLH_rw_write_u8(&rw,current);
            length--;
         }
      }
   }

   //Palette
   HLH_rw_write_u8(&rw,0x0c);
   for(int i = 0;i<256;i++)
   {
      if(i<color_count)
      {
         HLH_rw_write_u8(&rw,(uint8_t)SLK_color32_r(colors[i]));
         HLH_rw_write_u8(&rw,(uint8_t)SLK_color32_g(colors[i]));
         HLH_rw_write_u8(&rw,(uint8_t)SLK_color32_b(colors[i]));
      }
      else
      {
         HLH_rw_write_u8(&rw,0);
         HLH_rw_write_u8(&rw,0);
         HLH_rw_write_u8(&rw,0);
      }
   }

   HLH_rw_close(&rw);
   free(indices);
}
//-------------------------------------
