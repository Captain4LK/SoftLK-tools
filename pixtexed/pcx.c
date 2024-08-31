/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
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
            current = img->data[y*img->width+x];
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
      HLH_rw_write_u8(&rw,(uint8_t)color32_r(img->palette[i]));
      HLH_rw_write_u8(&rw,(uint8_t)color32_g(img->palette[i]));
      HLH_rw_write_u8(&rw,(uint8_t)color32_b(img->palette[i]));
   }

   HLH_rw_close(&rw);
   fclose(f);

   return 0;
}

Image8 *pcx_load(const char *path)
{
   HLH_rw rw = {0};
   FILE *f = NULL;
   Image8 *img = NULL;
   uint8_t *decoded = NULL;

   HLH_error_check(path!=NULL,"pcx_load","path must be non-NULL\n");

   f = fopen(path,"rb");
   HLH_error_check(f!=NULL,"pcx_load","failed to open file '%s': '%s'\n",path,strerror(errno));
   HLH_rw_init_file(&rw,f);

   //Header
   //--------------------------------
   uint8_t header = HLH_rw_read_u8(&rw);
   HLH_error_check(header==0x0a,"pcx_load","invalid header, expected 0x0a, but got 0x%02x\n",header);
   uint8_t version = HLH_rw_read_u8(&rw);
   HLH_error_check(version<=5,"pcx_load","invalid version, expected one of {0,1,2,3,5}, but got %d\n",version);
   uint8_t encoding = HLH_rw_read_u8(&rw);
   HLH_error_check(encoding==0||encoding==1,"pcx_load","invalid encoding, expected one of {0,1}, but got %d\n",encoding);
   uint8_t bit_depth = HLH_rw_read_u8(&rw);
   HLH_error_check(bit_depth==1||bit_depth==2||bit_depth==4||bit_depth==8,"pcx_load","unsupported bit depth, expected one of {1,2,4,8}, but got %d\n",bit_depth);

   uint16_t min_x = HLH_rw_read_u16(&rw);
   uint16_t min_y = HLH_rw_read_u16(&rw);
   uint16_t max_x = HLH_rw_read_u16(&rw);
   uint16_t max_y = HLH_rw_read_u16(&rw);
   int width = max_x-min_x+1;
   int height = max_y-min_y+1;
   HLH_error_check(width>0,"pcx_load","got width %d, but width must be larger than 0\n",width);
   HLH_error_check(height>0,"pcx_load","got height %d, but height must be larger than 0\n",height);
   img = image8_new(width,height);
   HLH_error_check(img!=NULL,"pcx_load","image creation failed, out of memory?\n");
   HLH_rw_read_u16(&rw);
   HLH_rw_read_u16(&rw);
   uint32_t ega_palette[16] = {0};
   for(int i = 0;i<16;i++)
   {
      uint8_t r = HLH_rw_read_u8(&rw);
      uint8_t g = HLH_rw_read_u8(&rw);
      uint8_t b = HLH_rw_read_u8(&rw);
      ega_palette[i] = color32(r,g,b,0xff);
   }
   HLH_rw_read_u8(&rw);
   uint8_t num_planes = HLH_rw_read_u8(&rw);
   HLH_error_check(num_planes==1||num_planes==3||num_planes==4,"pcx_load","unsupported number of planes, expected one of {1,3,4}, but got %d\n",num_planes);
   uint16_t stride = HLH_rw_read_u16(&rw);
   uint16_t palette_mode = HLH_rw_read_u16(&rw);
   //NOTE(Captain4LK): palette mode is often zero instead of 1 or 2, so we just ignore it
   //HLH_error_check(palette_mode==1||palette_mode==2,"pcx_load","invalid palette mode, expected one of {1,2}, but got %d\n",palette_mode);
   HLH_rw_read_u16(&rw);
   HLH_rw_read_u16(&rw);
   for(int i = 0;i<54;i++)
      HLH_rw_read_u8(&rw);
   //--------------------------------

   //Decode
   //--------------------------------
   printf("width: %d, stride: %d\n",width,stride);
   size_t decoded_len = stride*num_planes*height;
   decoded = calloc(decoded_len,sizeof(*decoded));
   if(encoding==0)
   {
      for(int i = 0;i<decoded_len;i++)
         decoded[i] = HLH_rw_read_u8(&rw);
   }
   else
   {
      size_t cur = 0;
      uint8_t run_len = 0;
      while(cur<decoded_len)
      {
         uint8_t byte = HLH_rw_read_u8(&rw);
         if(run_len==0&&(byte&0xc0)==0xc0)
         {
            run_len = byte&0x3f;
         }
         else
         {
            if(run_len==0)
               run_len = 1;

            for(int i = 0;i<run_len;i++)
               decoded[cur++] = byte;

            run_len = 0;
         }
      }
   }
   //--------------------------------

   //Convert to image8
   //--------------------------------
   printf("version: %d, bit depth: %d, plane count: %d, palette mode: %d\n",version,bit_depth,num_planes,palette_mode);
   //256 color palette
   if(bit_depth==8&&num_planes==1)
   {
      HLH_rw_seek(&rw,-768,SEEK_END);
      for(int i = 0;i<256;i++)
      {
         uint8_t r = HLH_rw_read_u8(&rw);
         uint8_t g = HLH_rw_read_u8(&rw);
         uint8_t b = HLH_rw_read_u8(&rw);
         img->palette[i] = color32(r,g,b,0xff);
      }
      for(int y = 0;y<height;y++)
      {
         for(int x = 0;x<width;x++)
         {
            img->data[y*img->width+x] = decoded[y*stride+x];
         }
      }
   }
   //1 bit
   else if(bit_depth==1&&num_planes==1)
   {
      img->palette[0] = color32(0,0,0,0xff);
      img->palette[1] = color32(0xff,0xff,0xff,0xff);
      img->color_count = 2;
      for(int y = 0;y<height;y++)
      {
         for(int x = 0;x<width;x++)
         {
            img->data[y*img->width+x] = !!(decoded[y*stride+(x/8)]&(1<<((7-x)&7)));
         }
      }
   }
   //CGA
   else if(bit_depth==2&&num_planes==1)
   {
      uint32_t cga_palette[16] = {
         color32(0x00,0x00,0x00,0xff),
         color32(0x00,0x00,0xaa,0xff),
         color32(0x00,0xaa,0x00,0xff),
         color32(0x00,0xaa,0xaa,0xff),
         color32(0xaa,0x00,0x00,0xff),
         color32(0xaa,0x00,0xaa,0xff),
         color32(0xaa,0x55,0x00,0xff),
         color32(0xaa,0xaa,0xaa,0xff),
         color32(0x55,0x55,0x55,0xff),
         color32(0x55,0x55,0xff,0xff),
         color32(0x55,0xff,0x55,0xff),
         color32(0x55,0xff,0xff,0xff),
         color32(0xff,0x55,0x55,0xff),
         color32(0xff,0x55,0xff,0xff),
         color32(0xff,0xff,0x55,0xff),
         color32(0xff,0xff,0xff,0xff),
      };

      uint8_t status_color_burst = !!(color32_r(ega_palette[1])&128);
      uint8_t status_palette = !!(color32_r(ega_palette[1])&64);
      uint8_t status_intensity = !!(color32_r(ega_palette[1])&32);
      if(palette_mode!=0)
      {
         status_palette = color32_g(ega_palette[1])<=color32_b(ega_palette[1]);
         status_intensity = HLH_max(color32_g(ega_palette[1]),color32_b(ega_palette[1]))>200;
      }
      uint32_t cga_map[4][3] = {{2,4,6},{10,12,14},{3,5,7},{11,13,15}};
      uint8_t index = status_palette*2+status_intensity;
      uint8_t foreground = (color32_r(ega_palette[0]))/16;
      img->palette[0] = cga_palette[foreground];
      img->palette[1] = cga_palette[cga_map[index][0]];
      img->palette[2] = cga_palette[cga_map[index][1]];
      img->palette[3] = cga_palette[cga_map[index][2]];
      img->color_count = 4;

      for(int y = 0;y<height;y++)
      {
         for(int x = 0;x<width;x++)
         {
            img->data[y*img->width+x] = 2*(!!(decoded[y*stride+(x/4)]&(1<<((7-2*x)&7))));
            img->data[y*img->width+x]+=!!(decoded[y*stride+(x/4)]&(1<<((7-(2*x+1))&7)));
         }
      }
   }
   free(decoded);
   //--------------------------------

   HLH_rw_close(&rw);
   fclose(f);

   return img;

HLH_err:
   if(img!=NULL)
      free(img);
   if(decoded!=NULL)
      free(decoded);
   if(HLH_rw_valid(&rw))
      HLH_rw_close(&rw);
   if(f!=NULL)
      fclose(f);

   return NULL;
}
//-------------------------------------
