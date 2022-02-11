/*
SoftLK-lib palettized image format

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _HLH_SLK_H_

#define _HLH_SLK_H_

typedef struct
{
   int32_t width;
   int32_t height;
   uint8_t *data;
}HLH_slk;

HLH_slk    *HLH_slk_image_load(const char *path);
HLH_slk    *HLH_slk_image_load_file(FILE *f);
HLH_slk    *HLH_slk_image_load_mem_buffer(const void *data, int length);
void        HLH_slk_image_free(HLH_slk *i);
void        HLH_slk_image_write(HLH_slk *i, FILE *f, int32_t comp);
const char *HLH_slk_get_error();

#endif

#ifdef HLH_SLK_IMPLEMENTATION
#ifndef HLH_SLK_IMPLEMENTATION_ONCE
#define HLH_SLK_IMPLEMENTATION_ONCE

#define CHECK(in,text_fail,action) \
   if(!in) { HLH_slk_error = text_fail; action; }

#define MIN(a,b) \
   ((a)<(b)?(a):(b))

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef HLH_SLK_MALLOC
#define HLH_SLK_MALLOC malloc
#endif

#ifndef HLH_SLK_FREE
#define HLH_SLK_FREE free
#endif

typedef struct
{
   char ident[8];
   int32_t width;
   int32_t height;
   int32_t comp;
}HLH_slk_header;
static const char *HLH_slk_error = NULL;

HLH_slk *HLH_slk_image_load(const char *path)
{
   FILE *f = fopen(path,"rb");
   CHECK(f,"HLH_slk_image_load: failed to open file",return NULL)

   HLH_slk *img = HLH_slk_image_load_file(f); 
   fclose(f);

   return img;
}

HLH_slk *HLH_slk_image_load_file(FILE *f)
{
   CHECK(f,"HLH_slk_image_load_file: file pointer is NULL",return NULL)

   char *buffer = NULL;
   int size = 0;
   fseek(f,0,SEEK_END);
   size = ftell(f);
   fseek(f,0,SEEK_SET);

   buffer = HLH_SLK_MALLOC(size+1);
   CHECK(buffer,"HLH_slk_image_load_file: malloc failed (out of memory)",return NULL)

   fread(buffer,size,1,f);
   buffer[size] = 0;
   HLH_slk *img = HLH_slk_image_load_mem_buffer(buffer,size);
   HLH_SLK_FREE(buffer);

   return img;
}

HLH_slk *HLH_slk_image_load_mem_buffer(const void *data, int length)
{
   const HLH_slk_header *h = (const HLH_slk_header *)data;
   CHECK((strncmp("SLKIMAGE",h->ident,8)==0),"HLH_slk_image_load_mem_buffer: memory does not seem to be slk file",return NULL)

   HLH_slk *s = HLH_SLK_MALLOC(sizeof(*s));
   CHECK(s,"HLH_slk_image_load_mem_buffer: malloc failed (out of memory)",return NULL)
   s->width = h->width;
   s->height = h->height;
   s->data = HLH_SLK_MALLOC(sizeof(*s->data)*s->width*s->height);
   CHECK(s->data,"HLH_slk_image_load_mem_buffer: malloc failed (out of memory)",HLH_SLK_FREE(s);return NULL)
   memset(s->data,1,s->width*s->height);
   int data_ptr = sizeof(*h)+7;
   if(data_ptr>=length)
      return s;

   //Index rle encoding 
   if(h->comp&1)
   {
      int end = s->width*s->height;
      int i = 0;
      while(i<end)
      {
         uint8_t lengthl = *((uint8_t *)data+data_ptr);
         data_ptr++;
         if(data_ptr>=length)
            break;
         uint8_t byte = *((uint8_t *)data+data_ptr);
         data_ptr++;
         if(data_ptr>=length)
            break;

         lengthl = MIN(lengthl,(end-i));
         for(int o = 0;o<lengthl;o++)
         {
            s->data[i] = byte;
            i++;
         }
      }
   }
   //No rle encoding
   else
   {
      int end = s->width*s->height;
      for(int i = 0;i<end;i++)
      {
         s->data[i] = *((char *)data+data_ptr);
         data_ptr++;
         if(data_ptr>=length)
            break;
      }
   }

   CHECK((strncmp("INDICES_END",(char *)data+data_ptr,11)==0),"HLH_slk_image_load_mem_buffer: error in image file",HLH_SLK_FREE(s->data);HLH_SLK_FREE(s);return NULL)

   return s;
}

void HLH_slk_image_free(HLH_slk *i)
{
   HLH_SLK_FREE(i->data);
   HLH_SLK_FREE(i);
}

void HLH_slk_image_write(HLH_slk *i, FILE *f, int32_t comp)
{
   HLH_slk_header h;
   strcpy(h.ident,"SLKIMAG");
   h.ident[7] = 'E';
   h.width = i->width;
   h.height = i->height;
   h.comp = comp;
   fwrite(&h,sizeof(h),1,f);
   fprintf(f,"INDICES");

   if(comp&1) //RLE encode indices
   {
      uint8_t length = 1;
      int end = i->width*i->height;
      uint8_t current = i->data[0];
      for(int o = 1;o<end;o++)
      {
         if(i->data[o]==current)
         {
            length++;
            if(length==255)
            {
               fputc(length,f);
               fputc(current,f);
               length = 0;
            }
         }
         else
         {
            fputc(length,f);
            fputc(current,f);
            length = 1;
            current = i->data[o];
         }
      }
      fputc(length,f);
      fputc(current,f);
   }
   else
   {
      for(int o = 0;o<i->width*i->height;o++)
         fputc(i->data[o],f);
   }
   fprintf(f,"INDICES_END");
}

const char *HLH_slk_get_error()
{
   return HLH_slk_error;
}

#undef CHECK
#undef MIN

#endif
#endif
