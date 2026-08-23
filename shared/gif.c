/*
SLK_img2pixel - a tool for converting images to pixelart

Minimal animated GIF writer - see gif.h for the public API.

Written in 2026 by June / SerbianKnifeFight, released to the public domain (CC0), same terms as the
rest of this project - see COPYING.
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//-------------------------------------

//Internal includes
#include "gif.h"
//-------------------------------------

//#defines
#define SLK_GIF_LZW_HASH_SIZE 8192 //power of two, must stay bigger than max dict entries (4096)
#define SLK_GIF_MAX_CODE 4096
//-------------------------------------

//Typedefs
typedef struct
{
   FILE *f;
   uint8_t sub_block[255];
   int sub_block_len;

   uint32_t bit_buffer;
   int bit_count;

   //LZW dictionary: open addressing hash map from (prefix_code<<8|pixel) to code
   int32_t hash_key[SLK_GIF_LZW_HASH_SIZE];
   int32_t hash_code[SLK_GIF_LZW_HASH_SIZE];

   int min_code_size;
   int clear_code;
   int end_code;
   int next_code;
   int code_size;
}slk_gif_lzw;
//-------------------------------------

//Function prototypes
static void slk_gif_lzw_init(slk_gif_lzw *lzw, FILE *f, int min_code_size);
static void slk_gif_lzw_reset_dict(slk_gif_lzw *lzw);
static void slk_gif_lzw_emit_code(slk_gif_lzw *lzw, int code);
static void slk_gif_lzw_flush_sub_block(slk_gif_lzw *lzw);
static void slk_gif_lzw_encode(slk_gif_lzw *lzw, const uint8_t *pixels, int count);
static void slk_gif_lzw_finish(slk_gif_lzw *lzw);

static int slk_gif_hash_find(const slk_gif_lzw *lzw, int32_t key);
static void slk_gif_hash_insert(slk_gif_lzw *lzw, int32_t key, int32_t code);

static int slk_gif_bits_for(int value);
static void slk_gif_write_u16(FILE *f, uint16_t v);
//-------------------------------------

//Function implementations

int SLK_gif_write(const char *path, int width, int height,
                   const uint32_t *palette, int palette_size,
                   uint8_t *const *frames, int frame_count,
                   const int *delays_cs, int loop, int transparent_index)
{
   if(path==NULL||palette==NULL||frames==NULL)
      return 0;
   if(width<=0||height<=0||frame_count<=0)
      return 0;
   if(palette_size<=0)
      return 0;
   if(palette_size>256)
      palette_size = 256;

   //GIF color tables must be a power of two in size, minimum 2
   int table_size = 2;
   while(table_size<palette_size&&table_size<256)
      table_size*=2;

   int min_code_size = slk_gif_bits_for(table_size-1);
   if(min_code_size<2)
      min_code_size = 2;

   FILE *f = fopen(path,"wb");
   if(f==NULL)
      return 0;

   //Header
   fwrite("GIF89a",1,6,f);

   //Logical screen descriptor
   slk_gif_write_u16(f,(uint16_t)width);
   slk_gif_write_u16(f,(uint16_t)height);
   uint8_t packed = (uint8_t)(0x80|((min_code_size-1)<<4)|(min_code_size-1)); //global color table follows, color resolution, table size
   fputc(packed,f);
   fputc(0,f); //background color index
   fputc(0,f); //pixel aspect ratio

   //Global color table
   for(int i = 0;i<table_size;i++)
   {
      if(i<palette_size)
      {
         uint32_t c = palette[i];
         fputc((int)(c&0xff),f);
         fputc((int)((c>>8)&0xff),f);
         fputc((int)((c>>16)&0xff),f);
      }
      else
      {
         fputc(0,f);
         fputc(0,f);
         fputc(0,f);
      }
   }

   //NETSCAPE2.0 application extension, only needed/valid for animations
   if(frame_count>1)
   {
      fputc(0x21,f); //extension introducer
      fputc(0xff,f); //application extension label
      fputc(11,f); //block size
      fwrite("NETSCAPE2.0",1,11,f);
      fputc(3,f); //sub-block size
      fputc(1,f); //sub-block id
      slk_gif_write_u16(f,(uint16_t)(loop?0:1)); //0 = loop forever
      fputc(0,f); //block terminator
   }

   for(int frame = 0;frame<frame_count;frame++)
   {
      int delay = delays_cs!=NULL?delays_cs[frame]:0;
      if(delay<0)
         delay = 0;

      //Graphic control extension
      fputc(0x21,f);
      fputc(0xf9,f);
      fputc(4,f);
      uint8_t gce_packed = 0x04; //disposal method 1: do not dispose
      if(transparent_index>=0)
         gce_packed = (uint8_t)(gce_packed|0x01);
      fputc(gce_packed,f);
      slk_gif_write_u16(f,(uint16_t)delay);
      fputc(transparent_index>=0?(transparent_index&0xff):0,f);
      fputc(0,f);

      //Image descriptor (no local color table, no interlace)
      fputc(0x2c,f);
      slk_gif_write_u16(f,0); //left
      slk_gif_write_u16(f,0); //top
      slk_gif_write_u16(f,(uint16_t)width);
      slk_gif_write_u16(f,(uint16_t)height);
      fputc(0,f);

      //LZW-compressed image data
      fputc(min_code_size,f);
      slk_gif_lzw lzw;
      slk_gif_lzw_init(&lzw,f,min_code_size);
      slk_gif_lzw_encode(&lzw,frames[frame],width*height);
      slk_gif_lzw_finish(&lzw);
      fputc(0,f); //block terminator
   }

   fputc(0x3b,f); //trailer
   fclose(f);

   return 1;
}

static int slk_gif_bits_for(int value)
{
   int bits = 1;
   while((1<<bits)<=value)
      bits++;
   return bits;
}

static void slk_gif_write_u16(FILE *f, uint16_t v)
{
   fputc(v&0xff,f);
   fputc((v>>8)&0xff,f);
}

static void slk_gif_lzw_init(slk_gif_lzw *lzw, FILE *f, int min_code_size)
{
   lzw->f = f;
   lzw->sub_block_len = 0;
   lzw->bit_buffer = 0;
   lzw->bit_count = 0;
   lzw->min_code_size = min_code_size;
   lzw->clear_code = 1<<min_code_size;
   lzw->end_code = lzw->clear_code+1;

   slk_gif_lzw_reset_dict(lzw);
   slk_gif_lzw_emit_code(lzw,lzw->clear_code);
}

static void slk_gif_lzw_reset_dict(slk_gif_lzw *lzw)
{
   for(int i = 0;i<SLK_GIF_LZW_HASH_SIZE;i++)
      lzw->hash_key[i] = -1;

   lzw->next_code = lzw->end_code+1;
   lzw->code_size = lzw->min_code_size+1;
}

static int slk_gif_hash_find(const slk_gif_lzw *lzw, int32_t key)
{
   uint32_t index = ((uint32_t)key*2654435761u)&(SLK_GIF_LZW_HASH_SIZE-1);
   for(;;)
   {
      if(lzw->hash_key[index]==-1)
         return -1;
      if(lzw->hash_key[index]==key)
         return lzw->hash_code[index];
      index = (index+1)&(SLK_GIF_LZW_HASH_SIZE-1);
   }
}

static void slk_gif_hash_insert(slk_gif_lzw *lzw, int32_t key, int32_t code)
{
   uint32_t index = ((uint32_t)key*2654435761u)&(SLK_GIF_LZW_HASH_SIZE-1);
   while(lzw->hash_key[index]!=-1)
      index = (index+1)&(SLK_GIF_LZW_HASH_SIZE-1);
   lzw->hash_key[index] = key;
   lzw->hash_code[index] = code;
}

static void slk_gif_lzw_flush_sub_block(slk_gif_lzw *lzw)
{
   if(lzw->sub_block_len<=0)
      return;

   fputc(lzw->sub_block_len,lzw->f);
   fwrite(lzw->sub_block,1,(size_t)lzw->sub_block_len,lzw->f);
   lzw->sub_block_len = 0;
}

static void slk_gif_lzw_emit_code(slk_gif_lzw *lzw, int code)
{
   lzw->bit_buffer|=((uint32_t)code)<<lzw->bit_count;
   lzw->bit_count+=lzw->code_size;

   while(lzw->bit_count>=8)
   {
      lzw->sub_block[lzw->sub_block_len++] = (uint8_t)(lzw->bit_buffer&0xff);
      lzw->bit_buffer>>=8;
      lzw->bit_count-=8;

      if(lzw->sub_block_len>=255)
         slk_gif_lzw_flush_sub_block(lzw);
   }
}

static void slk_gif_lzw_encode(slk_gif_lzw *lzw, const uint8_t *pixels, int count)
{
   if(count<=0)
      return;

   int32_t prefix = pixels[0];

   for(int i = 1;i<count;i++)
   {
      int32_t pixel = pixels[i];
      int32_t key = (prefix<<8)|pixel;
      int found = slk_gif_hash_find(lzw,key);

      if(found>=0)
      {
         prefix = found;
         continue;
      }

      slk_gif_lzw_emit_code(lzw,prefix);

      if(lzw->next_code<SLK_GIF_MAX_CODE)
      {
         slk_gif_hash_insert(lzw,key,lzw->next_code);
         lzw->next_code++;

         //Widen code size as soon as the dictionary needs more bits.
         //Codes are emitted using the OLD code_size until it grows, per spec.
         if(lzw->next_code>(1<<lzw->code_size)&&lzw->code_size<12)
            lzw->code_size++;
      }
      else
      {
         //Dictionary full: reset it and start over
         slk_gif_lzw_emit_code(lzw,lzw->clear_code);
         slk_gif_lzw_reset_dict(lzw);
      }

      prefix = pixel;
   }

   slk_gif_lzw_emit_code(lzw,prefix);
}

static void slk_gif_lzw_finish(slk_gif_lzw *lzw)
{
   slk_gif_lzw_emit_code(lzw,lzw->end_code);

   //Flush any remaining bits (pad final byte with zero bits)
   if(lzw->bit_count>0)
   {
      lzw->sub_block[lzw->sub_block_len++] = (uint8_t)(lzw->bit_buffer&0xff);
      lzw->bit_buffer = 0;
      lzw->bit_count = 0;
      if(lzw->sub_block_len>=255)
         slk_gif_lzw_flush_sub_block(lzw);
   }

   slk_gif_lzw_flush_sub_block(lzw);
}
//-------------------------------------
