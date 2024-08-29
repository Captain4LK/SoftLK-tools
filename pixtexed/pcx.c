/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdint.h>
#include "HLH.h"
#include "HLH_rw.h"
//-------------------------------------

//Internal includes
#include "color.h"
#include "pcx.h"
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

int pcx_save(const Image8 *img, const char *path)
{
   if(img==NULL||path==NULL)
      return 1;

   FILE *f = fopen(path,"wb");
   if(f==NULL)
      return 1;

   HLH_rw rw = {0};
   HLH_rw_init_file(&rw,f);

   //Header
   HLH_rw_write_u8(&rw,0x0a);
   HLH_rw_write_u8(&rw,0x05);
   HLH_rw_write_u8(&rw,0x01);
   HLH_rw_write_u8(&rw,0x08);
   HLH_rw_write_u16(&rw,0x0000);
   HLH_rw_write_u16(&rw,0x0000);
   HLH_rw_write_u16(&rw,(uint16_t)(img->width-1));
   HLH_rw_write_u16(&rw,(uint16_t)(img->height-1));
   HLH_rw_write_u16(&rw,0x0048);
   HLH_rw_write_u16(&rw,0x0048);
   for(int i = 0;i<48;i++)
      HLH_rw_write_u8(&rw,0x00);
   HLH_rw_write_u8(&rw,0x00);
   HLH_rw_write_u8(&rw,0x01);
   HLH_rw_write_u16(&rw,(uint16_t)img->width);
   HLH_rw_write_u16(&rw,0x0001);
   HLH_rw_write_u16(&rw,0x0000);
   HLH_rw_write_u16(&rw,0x0000);
   for(int i = 0;i<54;i++)
      HLH_rw_write_u8(&rw,0x00);

   //RLE encode
   for(int y = 0;y<img->height;y++)
   {
      uint8_t current = img->data[y*img->width];
      int length = 1;
      for(int x = 1;x<img->width;x++)
      {
         if(img->data[y*img->width+x]==current)
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
                  HLH_rw_write_u8(&rw,(uint8_t)(part_len|0xc0));
                  HLH_rw_write_u8(&rw,current);
                  length-=part_len;
               }
            }
            else
            {
               HLH_rw_write_u8(&rw,current);
            }
            length = 1;
            current = img->data[y*img->width+x];
         }
      }
      if(length>0)
      {
         if(length>1||current>191)
         {
            while(length>0)
            {
               int part_len = HLH_min(length,63);
               HLH_rw_write_u8(&rw,(uint8_t)(part_len|0xc0));
               HLH_rw_write_u8(&rw,current);
               length-=part_len;
            }
         }
         else
         {
            HLH_rw_write_u8(&rw,current);
         }
      }
   }

   //Palette
   HLH_rw_write_u8(&rw,0x0c);
   for(int i = 0;i<256;i++)
   {
      HLH_rw_write_u8(&rw,(uint8_t)color32_r(img->palette[i]));
      HLH_rw_write_u8(&rw,(uint8_t)color32_g(img->palette[i]));
      HLH_rw_write_u8(&rw,(uint8_t)color32_b(img->palette[i]));
   }

   HLH_rw_close(&rw);
   fclose(f);

   return 0;
}
//-------------------------------------
