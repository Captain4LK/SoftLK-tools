#ifndef _HLH_QOI_H_

/*
   QOI encoder/decoder

   Written in 2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

   To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

/* 
   To create implementation (the function definitions) add
      #define HLH_QOI_IMPLEMENTATION
   before including this file in *one* C file (translation unit)
*/

/*
   malloc() can be overwritten by 
   defining the following macros:

   HLH_QOI_MALLOC
*/

#define _HLH_QOI_H_

#ifndef _HLH_STREAM_H_
#error "HLH_QOI.h needs HLH_stream.h!"
#endif

typedef struct
{
   uint8_t r,g,b,a;
}HLH_qoi_color;

typedef struct
{
   uint32_t width;
   uint32_t height;
   uint8_t channels;
   uint8_t color_space;

   HLH_qoi_color *data;
}HLH_qoi_image;

HLH_qoi_image *HLH_qoi_decode(HLH_rw *rw);
void           HLH_qoi_encode(HLH_rw *rw, HLH_qoi_image *img);

#endif

#ifdef HLH_QOI_IMPLEMENTATION
#ifndef HLH_QOI_IMPLEMENTATION_ONCE
#define HLH_QOI_IMPLEMENTATION_ONCE

#ifndef HLH_QOI_MALLOC
#define HLH_QOI_MALLOC malloc
#endif

#define HLH_QOI_COLOR_EQ(c0,c1) ((c0).r==(c1).r&&(c0).g==(c1).g&&(c0).b==(c1).b&&(c0).a==(c1).a)

HLH_qoi_image *HLH_qoi_decode(HLH_rw *rw)
{
   uint8_t magic[4] = {0};
   magic[0] = HLH_rw_read_u8(rw);
   magic[1] = HLH_rw_read_u8(rw);
   magic[2] = HLH_rw_read_u8(rw);
   magic[3] = HLH_rw_read_u8(rw);

   if(magic[0]!='q'||magic[1]!='o'||magic[2]!='i'||magic[3]!='f')
      return NULL;

   uint32_t width = HLH_rw_read_u32(rw);
   uint32_t height = HLH_rw_read_u32(rw);
   uint8_t channels = HLH_rw_read_u8(rw);
   uint8_t color_space = HLH_rw_read_u8(rw);

   if((channels!=3&&channels!=4)||(color_space!=0&&color_space!=1))
      return NULL;

   HLH_qoi_image *img = HLH_QOI_MALLOC(sizeof(*img)+sizeof(*img->data)*width*height);
   img->data = (HLH_qoi_color *)(img+1);
   img->width = width;
   img->height = height;
   img->channels = channels;
   img->color_space = color_space;

   HLH_qoi_color prev = {.r = 0, .g = 0, .b = 0, .a = 255};
   HLH_qoi_color prev_hash[64] = {0};
   int pos = 0;
   int limit = img->width*img->height;
   while(pos<limit)
   {
      HLH_qoi_color cur = prev;
      uint8_t b0 = HLH_rw_read_u8(rw);
      uint8_t flag = (b0&(3<<6))>>6;

      //QOI_OP_RUN
      if(flag==3)
      {
         uint8_t run = b0&63;

         //QOI_OP_RGBA
         if(run==63)
         {
            cur.r = HLH_rw_read_u8(rw);
            cur.g = HLH_rw_read_u8(rw);
            cur.b = HLH_rw_read_u8(rw);
            cur.a = HLH_rw_read_u8(rw);

            img->data[pos++] = cur;
         }
         //-------------------------------------

         //QOI_OP_RGB
         else if(run==62)
         {
            cur.r = HLH_rw_read_u8(rw);
            cur.g = HLH_rw_read_u8(rw);
            cur.b = HLH_rw_read_u8(rw);

            img->data[pos++] = cur;
         }
         //-------------------------------------

         //QOI_OP_RUN
         else
         {
            for(int i = 0;i<run+1&&pos<limit;i++)
               img->data[pos++] = cur;
         }
         //-------------------------------------
      }
      //-------------------------------------

      //QOI_OP_INDEX
      else if(flag==0)
      {
         cur = prev_hash[b0&63];
         img->data[pos++] = cur;
      }
      //-------------------------------------

      //QOI_OP_DIFF
      else if(flag==1)
      {
         int dr = ((b0&(3<<4))>>4)-2;
         int dg = ((b0&(3<<2))>>2)-2;
         int db = (b0&3)-2;
         cur.r+=dr;
         cur.g+=dg;
         cur.b+=db;
         img->data[pos++] = cur;
      }
      //-------------------------------------

      //QOI_OP_LUMA
      else if(flag==2)
      {
         uint8_t b1 = HLH_rw_read_u8(rw);
         int dg = (b0&63)-32;
         int dr_dg = ((b1&(15<<4))>>4)-8;
         int db_dg = (b1&15)-8;
         cur.r+=dg+dr_dg;
         cur.g+=dg;
         cur.b+=dg+db_dg;
         img->data[pos++] = cur;
      }
      //-------------------------------------

      uint8_t hash = (cur.r*3+cur.g*5+cur.b*7+cur.a*11)&63;
      prev_hash[hash] = cur;
      prev = cur;
   }

   return img;
}

void HLH_qoi_encode(HLH_rw *rw, HLH_qoi_image *img)
{
   if(rw==NULL||img==NULL||img->data==NULL)
      return;

   //Write header
   HLH_rw_write_u8(rw,'q');
   HLH_rw_write_u8(rw,'o');
   HLH_rw_write_u8(rw,'i');
   HLH_rw_write_u8(rw,'f');
   HLH_rw_write_u32(rw,img->width);
   HLH_rw_write_u32(rw,img->height);
   HLH_rw_write_u8(rw,img->channels);
   HLH_rw_write_u8(rw,img->color_space);

   HLH_qoi_color prev = {.r = 0, .g = 0, .b = 0, .a = 255};
   HLH_qoi_color prev_hash[64] = {0};
   int run = 0;

   for(int i = 0;i<img->width*img->height;i++)
   {
      HLH_qoi_color cur = img->data[i];
      uint8_t hash = (cur.r*3+cur.g*5+cur.b*7+cur.a*11)&63;

      //QOI_OP_RUN
      if((!HLH_QOI_COLOR_EQ(cur,prev)&&run>0)||run==62)
      {
         uint8_t val = run-1;
         val|=3<<6;
         HLH_rw_write_u8(rw,val);
         run = 0;
      }

      if(HLH_QOI_COLOR_EQ(cur,prev))
      {
         run++;

         //Cry about it
         goto next;
      }
      //-------------------------------------

      //QOI_OP_INDEX
      if(HLH_QOI_COLOR_EQ(cur,prev_hash[hash]))
      {
         HLH_rw_write_u8(rw,hash);

         goto next;
      }
      //-------------------------------------

      if(cur.a==prev.a)
      {
         //QOI_OP_DIFF
         int8_t dr = cur.r-prev.r;
         int8_t dg = cur.g-prev.g;
         int8_t db = cur.b-prev.b;
         if(dr>=-2&&dr<=1&&
            dg>=-2&&dg<=1&&
            db>=-2&&db<=1)
         {
            uint8_t val = db+2;
            val|=(dg+2)<<2;
            val|=(dr+2)<<4;
            val|=1<<6;
            HLH_rw_write_u8(rw,val);

            goto next;
         }

         //-------------------------------------

         //QOI_OP_LUMA
         int8_t dr_dg = dr-dg;
         int8_t db_dg = db-dg;
         if(dr_dg>=-8&&dr_dg<=7&&
            db_dg>=-8&&db_dg<=7&&
            dg>=-32&&dg<=31)
         {
            uint8_t val = dg+32;
            val|=2<<6;
            HLH_rw_write_u8(rw,val);
            val = db_dg+8;
            val|=(dr_dg+8)<<4;
            HLH_rw_write_u8(rw,val);

            goto next;
         }
         //-------------------------------------

         //QOI_OP_RGB
         HLH_rw_write_u8(rw,254);
         HLH_rw_write_u8(rw,cur.r);
         HLH_rw_write_u8(rw,cur.g);
         HLH_rw_write_u8(rw,cur.b);

         goto next;
         //-------------------------------------
      }

      //QOI_OP_RGBA
      HLH_rw_write_u8(rw,255);
      HLH_rw_write_u8(rw,cur.r);
      HLH_rw_write_u8(rw,cur.g);
      HLH_rw_write_u8(rw,cur.b);
      HLH_rw_write_u8(rw,cur.a);
      //-------------------------------------

next:
      prev_hash[hash] = cur;
      prev = cur;
   }

   //Terminate run
   if(run>0)
   {
      uint8_t val = run-1;
      val|=3<<6;
      HLH_rw_write_u8(rw,val);
   }

   //Footer
   HLH_rw_write_u8(rw,0);
   HLH_rw_write_u8(rw,0);
   HLH_rw_write_u8(rw,0);
   HLH_rw_write_u8(rw,0);
   HLH_rw_write_u8(rw,0);
   HLH_rw_write_u8(rw,0);
   HLH_rw_write_u8(rw,0);
   HLH_rw_write_u8(rw,1);
}

#undef HLH_QOI_COLOR_EQ

#endif
#endif
