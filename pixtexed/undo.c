/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//-------------------------------------

//Internal includes
#include "undo.h"
//-------------------------------------

//#defines
#define UNDO_BUFFER_SIZE (1 << 25)
#define WRAP(p) ((p) & (UNDO_BUFFER_SIZE - 1))
#define UNDO_RECORD (UINT8_MAX)
#define REDO_RECORD (UINT8_MAX - 2)
#define JUNK_RECORD (UINT8_MAX - 1)

#define undo_read8(p,var,pos) do { var = p->undo_buffer[pos]; pos =  WRAP(pos-1); } while(0)
#define undo_read16(p,var,pos) do { uint16_t lo16,hi16; undo_read8(p,hi16,pos); undo_read8(p,lo16,pos); var = (uint16_t)(hi16*0x100+lo16); } while(0)
#define undo_read32(p,var,pos) do { uint32_t lo32,hi32; undo_read16(p,hi32,pos); undo_read16(p,lo32,pos); var = (uint32_t)(hi32*0x10000+lo32); } while(0)
#define undo_read64(p,var,pos) do { uint64_t lo64,hi64; undo_read32(p,hi64,pos); undo_read32(p,lo64,pos); var = (uint64_t)(hi64*0x100000000+lo64); } while(0)

#define redo_read8(p,var,pos) do { var = p->redo_buffer[pos]; pos = WRAP(pos-1); } while(0)
#define redo_read16(p,var,pos) do { uint16_t lo16,hi16; redo_read8(p,hi16,pos); redo_read8(p,lo16,pos); var = (uint16_t)(hi16*0x100+lo16); } while(0)
#define redo_read32(p,var,pos) do { uint32_t lo32,hi32; redo_read16(p,hi32,pos); redo_read16(p,lo32,pos); var = (uint32_t)(hi32*0x10000+lo32); } while(0)
#define redo_read64(p,var,pos) do { uint64_t lo64,hi64; redo_read32(p,hi64,pos); redo_read32(p,lo64,pos); var = (uint64_t)(hi64*0x100000000+lo64); } while(0)
//-------------------------------------

//Typedefs
typedef enum
{
   ED_LAYER_CHUNKS,
}Ed_action;
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static void undo_write8(Project *p, uint8_t val);
static void undo_write16(Project *p, uint16_t val);
static void undo_write32(Project *p, uint32_t val);
static void undo_write64(Project *p, uint64_t val);

static void undo_begin(Project *p, Ed_action action);
static void undo_end(Project *p);

static void redo_write8(Project *p, uint8_t val);
static void redo_write16(Project *p, uint16_t val);
static void redo_write32(Project *p, uint32_t val);
static void redo_write64(Project *p, uint64_t val);

static void undo_layer_chunk(Project *p, int pos, int endpos);
static void redo_layer_chunk(Project *p, int pos, int endpos);
//-------------------------------------

//Function implementations

void undo_init(Project *p)
{
   if(p==NULL)
      return;

   p->undo_buffer = malloc(sizeof(*p->undo_buffer) * UNDO_BUFFER_SIZE);
   memset(p->undo_buffer, 0, sizeof(*p->undo_buffer) * UNDO_BUFFER_SIZE);
   p->redo_buffer = malloc(sizeof(*p->redo_buffer) * UNDO_BUFFER_SIZE);
   memset(p->redo_buffer, 0, sizeof(*p->redo_buffer) * UNDO_BUFFER_SIZE);
}

void undo_free(Project *p)
{
   if(p==NULL)
      return;

   free(p->undo_buffer);
   p->undo_buffer = NULL;
   free(p->redo_buffer);
   p->redo_buffer = NULL;
}

void undo_reset(Project *p)
{
   p->undo_len = 0;
   p->undo_pos = 0;
   p->redo_len = 0;
   p->redo_pos = 0;
}

void undo(Project *p)
{
   int pos = p->undo_pos;
   uint32_t len = 0;
   Ed_action action;

   if(p->undo_buffer[p->undo_pos]!=JUNK_RECORD)
      return;

   pos = WRAP(pos-1);
   undo_read32(p,len,pos);
   int endpos = WRAP(pos-len);
   undo_read16(p,action,endpos);
   endpos = WRAP(endpos+2);

   if(pos==endpos)
      return;


   //New redo entry
   redo_write8(p,REDO_RECORD);
   redo_write16(p,action);
   p->redo_entry_len = 0;


   //Apply undoes
   switch(action)
   {
   case ED_LAYER_CHUNKS: undo_layer_chunk(p,pos,endpos); break;
   }

   redo_write32(p,p->redo_entry_len);
   p->redo_buffer[p->redo_pos] = JUNK_RECORD;
   p->undo_pos = WRAP(p->undo_pos-len-7);
   p->undo_buffer[p->undo_pos] = JUNK_RECORD;

   project_update_full(p);
}

void redo(Project *p)
{
   int pos = p->redo_pos;
   uint32_t len = 0;
   Ed_action action;

   if(p->redo_buffer[p->redo_pos]!=JUNK_RECORD)
      return;
   if(p->redo_len<=0)
      return;
   pos = WRAP(pos-1);
   redo_read32(p,len,pos);
   int endpos = WRAP(pos-len);
   redo_read16(p,action,endpos);
   endpos = WRAP(endpos+2);
   //int endpos = redo_find_end(&len);
   //if(endpos<0)
      //return;

   //Ed_action action = undo_buffer[endpos];
   //pos = WRAP(undo_pos + 3);

   if(pos==endpos)
      return;

   //New undo entry
   undo_write8(p,UNDO_RECORD);
   undo_write16(p,action);
   p->undo_entry_len = 0;
   //printf("%d\n",action);

   //Apply redoes
   switch(action)
   {
   case ED_LAYER_CHUNKS: redo_layer_chunk(p,pos,endpos); break;
   }

   undo_write32(p,p->undo_entry_len);
   //undo_write16((len >> 16) & UINT16_MAX);
   //undo_write16(len & UINT16_MAX);
   p->undo_buffer[p->undo_pos] = JUNK_RECORD;
   p->redo_pos = WRAP(p->redo_pos-len-7);
   p->redo_buffer[p->redo_pos] = JUNK_RECORD;

   project_update_full(p);
}

static void undo_write8(Project *p, uint8_t val)
{
   int pos = p->undo_pos;
   p->undo_buffer[pos] = val;
   p->undo_pos = WRAP(pos + 1);
   p->undo_len += (p->undo_len<UNDO_BUFFER_SIZE - 2);
   //redo_len -= (redo_len>0);
   p->undo_entry_len++;
}

static void undo_write16(Project *p, uint16_t val)
{
   undo_write8(p,(uint8_t)(val&255));
   undo_write8(p,(uint8_t)((val>>8)&255));
}

static void undo_write32(Project *p, uint32_t val)
{
   undo_write16(p,(uint16_t)(val&0xffff));
   undo_write16(p,(uint16_t)((val>>16)&0xffff));
}

static void undo_write64(Project *p, uint64_t val)
{
   undo_write32(p,(uint32_t)(val&0xffffffff));
   undo_write32(p,(uint32_t)((val>>32)&0xffffffff));
}

static void redo_write8(Project *p, uint8_t val)
{
   int pos = p->redo_pos;
   p->redo_buffer[pos] = val;
   p->redo_pos = WRAP(pos + 1);
   p->redo_len += (p->redo_len<UNDO_BUFFER_SIZE - 2);
   //undo_len -= (undo_len>0);
   p->redo_entry_len++;

   //int pos = undo_pos;
   //undo_buffer[pos] = val;
   //undo_pos = WRAP(pos - 1);
   //redo_len += (redo_len<UNDO_BUFFER_SIZE - 2);
   //undo_len -= (undo_len>0);
   //redo_entry_len++;
}

static void redo_write16(Project *p, uint16_t val)
{
   redo_write8(p,(uint8_t)(val&255));
   redo_write8(p,(uint8_t)((val>>8)&255));
}

static void redo_write32(Project *p, uint32_t val)
{
   redo_write16(p,(uint16_t)(val&0xffff));
   redo_write16(p,(uint16_t)((val>>16)&0xffff));
}

static void redo_write64(Project *p, uint64_t val)
{
   redo_write32(p,(uint32_t)(val&0xffffffff));
   redo_write32(p,(uint32_t)((val>>32)&0xffffffff));
}

static void undo_begin(Project *p, Ed_action action)
{
   //map_modified = 1;

   p->redo_len = 0;
   p->redo_pos = 0;
   undo_write8(p,UNDO_RECORD);
   undo_write16(p,action);
   p->undo_entry_len = 0;
}

static void undo_end(Project *p)
{
   int pos = WRAP(p->undo_pos - 3);
   if(p->undo_entry_len==0)
   {
      //empty
      p->undo_pos = pos;
      p->undo_len -= 3;
      p->undo_buffer[p->undo_pos] = JUNK_RECORD;
   }
   else
   {
      undo_write32(p,p->undo_entry_len);
      p->undo_buffer[p->undo_pos] = JUNK_RECORD;
   }
}

void undo_begin_layer_chunks(Project *p)
{
   undo_begin(p,ED_LAYER_CHUNKS);
}

void undo_track_layer_chunk(Project *p, int x, int y, int32_t layer)
{
   undo_write32(p,x);
   undo_write32(p,y);
   undo_write32(p,layer);

   uint8_t chunk[256] = {0};

   int width = p->width-x*16;
   if(width>16) width = 16;
   int height = p->height-y*16;
   if(height>16) height = 16;
   for(int py = 0;py<height;py++)
   {
      for(int px = 0;px<width;px++)
      {
         chunk[py*16+px] = p->layers[layer]->data[(y*16+py)*p->width+x*16+px];
      }
   }

   for(int i = 0;i<256;i++)
      undo_write8(p,chunk[i]);
}

void undo_end_layer_chunks(Project *p)
{
   undo_end(p);
}

static void undo_layer_chunk(Project *p, int pos, int endpos)
{
   while(pos!=endpos)
   {
      uint8_t chunk[256];
      for(int i = 255;i>=0;i--)
         undo_read8(p,chunk[i],pos);
      int32_t x;
      int32_t y;
      int32_t layer;
      undo_read32(p,layer,pos);
      undo_read32(p,y,pos);
      undo_read32(p,x,pos);

      redo_write32(p,x);
      redo_write32(p,y);
      redo_write32(p,layer);

      int width = p->width-x*16;
      if(width>16) width = 16;
      int height = p->height-y*16;
      if(height>16) height = 16;
      uint8_t chunk_write[256] = {0};
      //printf("%d %d %d %d\n",x,y,width,height);
      for(int py = 0;py<height;py++)
      {
         for(int px = 0;px<width;px++)
         {
            chunk_write[py*16+px] = p->layers[layer]->data[(y*16+py)*p->width+x*16+px];
         }
      }
      for(int i = 0;i<256;i++)
         redo_write8(p,chunk_write[i]);

      for(int py = 0;py<height;py++)
      {
         for(int px = 0;px<width;px++)
         {
            p->layers[layer]->data[(y*16+py)*p->width+x*16+px] = chunk[py*16+px];
         }
      }
   }
}

static void redo_layer_chunk(Project *p, int pos, int endpos)
{
   while(pos!=endpos)
   {
      uint8_t chunk[256];
      for(int i = 255;i>=0;i--)
         redo_read8(p,chunk[i],pos);
      int32_t x;
      int32_t y;
      int32_t layer;
      redo_read32(p,layer,pos);
      redo_read32(p,y,pos);
      redo_read32(p,x,pos);

      undo_write32(p,x);
      undo_write32(p,y);
      undo_write32(p,layer);

      int width = p->width-x*16;
      if(width>16) width = 16;
      int height = p->height-y*16;
      if(height>16) height = 16;
      uint8_t chunk_write[256] = {0};
      for(int py = 0;py<height;py++)
      {
         for(int px = 0;px<width;px++)
         {
            chunk_write[py*16+px] = p->layers[layer]->data[(y*16+py)*p->width+x*16+px];
         }
      }
      for(int i = 0;i<256;i++)
         undo_write8(p,chunk_write[i]);

      for(int py = 0;py<height;py++)
      {
         for(int px = 0;px<width;px++)
         {
            p->layers[layer]->data[(y*16+py)*p->width+x*16+px] = chunk[py*16+px];
         }
      }
   }
}
//-------------------------------------
