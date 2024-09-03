/*
File stream/mem buffer/etc. wrapper

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

/* 
   To create implementation (the function definitions) add
      #define HLH_RW_IMPLEMENTATION
   before including this file in *one* C file (translation unit)
*/

/*
   malloc(), realloc() and free() can be overwritten by 
   defining the following macros:

   HLH_MALLOC
   HLH_FREE
   HLH_REALLOC
*/

#ifndef _HLH_RW_H_

#define _HLH_RW_H_

#include <stdio.h>

typedef enum
{
   HLH_RW_INVALID = 0,
   HLH_RW_STD_FILE = 1,
   HLH_RW_STD_FILE_PATH = 2,
   HLH_RW_MEM = 3,
   HLH_RW_DYN_MEM = 4,
   HLH_RW_CONST_MEM = 5,
   HLH_RW_USR = 6,
}HLH_rw_type;

typedef enum
{
   HLH_RW_LITTLE_ENDIAN = 0,
   HLH_RW_BIG_ENDIAN = 1,
}HLH_rw_endian;

typedef struct HLH_rw HLH_rw;

typedef void (*HLH_rw_usr_init)  (HLH_rw *rw, void *data);
typedef void (*HLH_rw_usr_close) (HLH_rw *rw);
typedef void (*HLH_rw_usr_flush) (HLH_rw *rw);
typedef int (*HLH_rw_usr_seek)  (HLH_rw *rw, long offset, int origin);
typedef long (*HLH_rw_usr_tell)  (HLH_rw *rw);
typedef int (*HLH_rw_usr_eof)   (HLH_rw *rw);
typedef size_t (*HLH_rw_usr_read)  (HLH_rw *rw, void *buffer, size_t size, size_t count);
typedef size_t (*HLH_rw_usr_write) (HLH_rw *rw, const void *buffer, size_t size, size_t count);

struct HLH_rw
{
   HLH_rw_type type;
   HLH_rw_endian endian;

   union
   {
      FILE *fp;

      struct
      {
         void *mem;
         long size;
         long csize;
         long pos;
      } mem;

      struct
      {
         void *mem;
         long size;
         long csize;
         long pos;
         long min_grow;
      } dmem;

      struct
      {
         const void *mem;
         long size;
         long pos;
      } cmem;

      struct
      {
         HLH_rw_usr_close close;
         HLH_rw_usr_flush flush;
         HLH_rw_usr_seek seek;
         HLH_rw_usr_tell tell;
         HLH_rw_usr_eof eof;
         HLH_rw_usr_read read;
         HLH_rw_usr_write write;

         void *data;
      } usr;
   } as;
};

void HLH_rw_init_file(HLH_rw *rw, FILE *f);
void HLH_rw_init_path(HLH_rw *rw, const char *path, const char *mode);
void HLH_rw_init_mem(HLH_rw *rw, void *mem, size_t len, size_t clen);
void HLH_rw_init_dyn_mem(HLH_rw *rw, size_t base_len, size_t min_grow);
void HLH_rw_init_const_mem(HLH_rw *rw, const void *mem, size_t len);
void HLH_rw_init_usr(HLH_rw *rw, HLH_rw_usr_init init, void *data);

int    HLH_rw_valid(const HLH_rw *rw);
void   HLH_rw_endian_set(HLH_rw *rw, HLH_rw_endian endian);
void   HLH_rw_close(HLH_rw *rw);
void   HLH_rw_flush(HLH_rw *rw);
int    HLH_rw_seek(HLH_rw *rw, long offset, int origin);
long   HLH_rw_tell(HLH_rw *rw);
int    HLH_rw_eof(HLH_rw *rw);
size_t HLH_rw_read(HLH_rw *rw, void *buffer, size_t size, size_t count);
size_t HLH_rw_write(HLH_rw *rw, const void *buffer, size_t size, size_t count);
int    HLH_rw_printf(HLH_rw *rw, const char *format, ...);

size_t HLH_rw_write_u8(HLH_rw *rw, uint8_t val);
size_t HLH_rw_write_u16(HLH_rw *rw, uint16_t val);
size_t HLH_rw_write_u32(HLH_rw *rw, uint32_t val);
size_t HLH_rw_write_u64(HLH_rw *rw, uint64_t val);

uint8_t  HLH_rw_read_u8(HLH_rw *rw);
uint16_t HLH_rw_read_u16(HLH_rw *rw);
uint32_t HLH_rw_read_u32(HLH_rw *rw);
uint64_t HLH_rw_read_u64(HLH_rw *rw);

#endif

#ifdef HLH_RW_IMPLEMENTATION
#ifndef HLH_RW_IMPLEMENTATION_ONCE
#define HLH_RW_IMPLEMENTATION_ONCE

#ifndef HLH_MALLOC
#define HLH_MALLOC malloc
#endif

#ifndef HLH_FREE
#define HLH_FREE free
#endif

#ifndef HLH_REALLOC
#define HLH_REALLOC realloc
#endif

#define HLH_rw_max(a,b) ((a)>(b)?(a):(b))

void HLH_rw_init_file(HLH_rw *rw, FILE *f)
{
   if(rw==NULL)
      return;

   rw->type = HLH_RW_INVALID;
   if(f==NULL)
      return;

   rw->type = HLH_RW_STD_FILE;
   rw->endian = HLH_RW_LITTLE_ENDIAN;
   rw->as.fp = f;
}

void HLH_rw_init_path(HLH_rw *rw, const char *path, const char *mode)
{
   if(rw==NULL)
      return;

   rw->type = HLH_RW_INVALID;
   if(path==NULL||mode==NULL)
      return;

   rw->type = HLH_RW_STD_FILE_PATH;
   rw->endian = HLH_RW_LITTLE_ENDIAN;
   rw->as.fp = fopen(path, mode);
   if(rw->as.fp==NULL)
      rw->type = HLH_RW_INVALID;
}

void HLH_rw_init_mem(HLH_rw *rw, void *mem, size_t len, size_t clen)
{
   if(rw==NULL)
      return;

   rw->type = HLH_RW_INVALID;
   if(mem==NULL)
      return;

   rw->type = HLH_RW_MEM;
   rw->endian = HLH_RW_LITTLE_ENDIAN;
   rw->as.mem.mem = mem;
   rw->as.mem.size = len;
   rw->as.mem.pos = 0;
   rw->as.mem.csize = clen;
}

void HLH_rw_init_dyn_mem(HLH_rw *rw, size_t base_len, size_t min_grow)
{
   if(rw==NULL)
      return;

   rw->type = HLH_RW_DYN_MEM;
   rw->as.dmem.mem = HLH_MALLOC(base_len);
   rw->as.dmem.size = base_len;
   rw->as.dmem.csize = 0;
   rw->as.dmem.pos = 0;
   rw->as.dmem.min_grow = min_grow;
}

void HLH_rw_init_const_mem(HLH_rw *rw, const void *mem, size_t len)
{
   if(rw==NULL)
      return;

   rw->type = HLH_RW_INVALID;
   if(mem==NULL)
      return;

   rw->type = HLH_RW_CONST_MEM;
   rw->endian = HLH_RW_LITTLE_ENDIAN;
   rw->as.cmem.mem = mem;
   rw->as.cmem.size = len;
   rw->as.cmem.pos = 0;
}

void HLH_rw_init_usr(HLH_rw *rw, HLH_rw_usr_init init, void *data)
{
   rw->type = HLH_RW_USR;
   rw->endian = HLH_RW_LITTLE_ENDIAN;
   init(rw, data);
}

int HLH_rw_valid(const HLH_rw *rw)
{
   if(rw==NULL)
      return 0;

   //Invalid file pointer
   if((rw->type==HLH_RW_STD_FILE||rw->type==HLH_RW_STD_FILE_PATH)&&rw->as.fp==0)
      return 0;

   return rw->type!=HLH_RW_INVALID;
}

void HLH_rw_endian_set(HLH_rw *rw, HLH_rw_endian endian)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return;
   rw->endian = endian;
}

void HLH_rw_close(HLH_rw *rw)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return;

   if(rw->type==HLH_RW_STD_FILE_PATH)
      fclose(rw->as.fp);
   else if(rw->type==HLH_RW_DYN_MEM)
      HLH_FREE(rw->as.dmem.mem);
   else if(rw->type==HLH_RW_USR)
      rw->as.usr.flush(rw);

   rw->type = HLH_RW_INVALID;
}

void HLH_rw_flush(HLH_rw *rw)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return;

   if(rw->type==HLH_RW_STD_FILE||rw->type==HLH_RW_STD_FILE_PATH)
      fflush(rw->as.fp);
   else if(rw->type==HLH_RW_USR)
      rw->as.usr.flush(rw);
}

int HLH_rw_seek(HLH_rw *rw, long offset, int origin)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return 1;

   if(rw->type==HLH_RW_STD_FILE||rw->type==HLH_RW_STD_FILE_PATH)
   {
      return fseek(rw->as.fp, offset, origin);
   }
   else if(rw->type==HLH_RW_MEM)
   {
      if(origin==SEEK_SET)
         rw->as.mem.pos = offset;
      else if(origin==SEEK_CUR)
         rw->as.mem.pos += offset;
      else if(origin==SEEK_END)
         rw->as.mem.pos = rw->as.mem.csize + offset;

      if(rw->as.mem.pos<0)
      {
         rw->as.mem.pos = 0;
         return 1;
      }

      return 0;
   }
   else if(rw->type==HLH_RW_DYN_MEM)
   {
      if(origin==SEEK_SET)
         rw->as.dmem.pos = offset;
      else if(origin==SEEK_CUR)
         rw->as.dmem.pos += offset;
      else if(origin==SEEK_END)
         rw->as.dmem.pos = rw->as.dmem.csize + offset;

      if(rw->as.dmem.pos<0)
      {
         rw->as.dmem.pos = 0;
         return 1;
      }

      return 0;
   }
   else if(rw->type==HLH_RW_CONST_MEM)
   {
      if(origin==SEEK_SET)
         rw->as.cmem.pos = offset;
      else if(origin==SEEK_CUR)
         rw->as.cmem.pos += offset;
      else if(origin==SEEK_END)
         rw->as.cmem.pos = rw->as.cmem.size + offset;

      if(rw->as.cmem.pos<0)
      {
         rw->as.cmem.pos = 0;
         return 1;
      }

      return 0;
   }
   else if(rw->type==HLH_RW_USR)
   {
      return rw->as.usr.seek(rw, offset, origin);
   }

   return 1;
}

long HLH_rw_tell(HLH_rw *rw)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return EOF;

   if(rw->type==HLH_RW_STD_FILE||rw->type==HLH_RW_STD_FILE_PATH)
      return ftell(rw->as.fp);
   else if(rw->type==HLH_RW_MEM)
      return rw->as.mem.pos;
   else if(rw->type==HLH_RW_DYN_MEM)
      return rw->as.dmem.pos;
   else if(rw->type==HLH_RW_CONST_MEM)
      return rw->as.cmem.pos;
   else if(rw->type==HLH_RW_USR)
      return rw->as.usr.tell(rw);

   return EOF;
}

int HLH_rw_eof(HLH_rw *rw)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return 1;

   if(rw->type==HLH_RW_STD_FILE||rw->type==HLH_RW_STD_FILE_PATH)
      return feof(rw->as.fp);
   else if(rw->type==HLH_RW_MEM)
      return rw->as.mem.pos>=rw->as.mem.csize;
   else if(rw->type==HLH_RW_DYN_MEM)
      return rw->as.dmem.pos>=rw->as.dmem.csize;
   else if(rw->type==HLH_RW_CONST_MEM)
      return rw->as.cmem.pos>=rw->as.cmem.size;
   else if(rw->type==HLH_RW_USR)
      return rw->as.usr.eof(rw);

   return 1;
}

size_t HLH_rw_read(HLH_rw *rw, void *buffer, size_t size, size_t count)
{
   if(rw==NULL||buffer==NULL||!HLH_rw_valid(rw))
      return 0;

   if(rw->type==HLH_RW_STD_FILE||rw->type==HLH_RW_STD_FILE_PATH)
   {
      return fread(buffer, size, count, rw->as.fp);
   }
   else if(rw->type==HLH_RW_MEM)
   {
      uint8_t *buff_out = buffer;
      uint8_t *buff_in = rw->as.mem.mem;

      for(size_t i = 0; i<count; i++)
      {
         if(rw->as.mem.pos + (long)size>rw->as.mem.csize)
            return i;

         memcpy(buff_out + (i * size), buff_in + rw->as.mem.pos, size);
         rw->as.mem.pos += size;
      }

      return count;
   }
   else if(rw->type==HLH_RW_DYN_MEM)
   {
      uint8_t *buff_out = buffer;
      uint8_t *buff_in = rw->as.dmem.mem;

      for(size_t i = 0; i<count; i++)
      {
         if(rw->as.dmem.pos + (long)size>rw->as.dmem.csize)
            return i;

         memcpy(buff_out + (i * size), buff_in + rw->as.dmem.pos, size);
         rw->as.dmem.pos += size;
      }

      return count;
   }
   else if(rw->type==HLH_RW_CONST_MEM)
   {
      uint8_t *buff_out = buffer;
      const uint8_t *buff_in = rw->as.cmem.mem;

      for(size_t i = 0; i<count; i++)
      {
         if(rw->as.cmem.pos + (long)size>rw->as.cmem.size)
            return i;

         memcpy(buff_out + (i * size), buff_in + rw->as.cmem.pos, size);
         rw->as.cmem.pos += size;
      }

      return count;
   }
   else if(rw->type==HLH_RW_USR)
   {
      return rw->as.usr.read(rw, buffer, size, count);
   }


   return 0;
}

size_t HLH_rw_write(HLH_rw *rw, const void *buffer, size_t size, size_t count)
{
   if(rw==NULL||buffer==NULL||!HLH_rw_valid(rw))
      return 0;

   if(rw->type==HLH_RW_STD_FILE||rw->type==HLH_RW_STD_FILE_PATH)
   {
      return fwrite(buffer, size, count, rw->as.fp);
   }
   else if(rw->type==HLH_RW_MEM)
   {
      uint8_t *buff_out = rw->as.mem.mem;
      const uint8_t *buff_in = buffer;

      for(size_t i = 0; i<count; i++)
      {
         if(rw->as.mem.pos + (long)size>rw->as.mem.size)
            return 1;

         memcpy(buff_out + rw->as.mem.pos, buff_in + (i * size), size);
         rw->as.mem.pos += size;
         rw->as.mem.csize = HLH_rw_max(rw->as.mem.csize, rw->as.mem.pos);
      }

      return count;
   }
   else if(rw->type==HLH_RW_DYN_MEM)
   {
      uint8_t *buff_out = rw->as.dmem.mem;
      const uint8_t *buff_in = buffer;

      for(size_t i = 0; i<count; i++)
      {
         if(rw->as.dmem.pos + (long)size>rw->as.dmem.size)
         {
            rw->as.dmem.size += HLH_rw_max(rw->as.dmem.min_grow, rw->as.dmem.pos + (long)size - rw->as.dmem.size);
            rw->as.dmem.mem = HLH_REALLOC(rw->as.dmem.mem, rw->as.dmem.size);
         }

         rw->as.dmem.csize = HLH_rw_max(rw->as.dmem.csize, rw->as.dmem.pos);
         memcpy(buff_out + rw->as.dmem.pos, buff_in + (i * size), size);
         rw->as.dmem.pos += size;
      }

      return count;
   }
   else if(rw->type==HLH_RW_CONST_MEM)
   {
      return 0;
   }
   else if(rw->type==HLH_RW_USR)
   {
      return rw->as.usr.write(rw, buffer, size, count);
   }

   return 0;
}

int HLH_rw_printf(HLH_rw *rw, const char *format, ...)
{
   if(rw==NULL||!HLH_rw_valid(rw)||format==NULL)
      return -1;

   //Limit of 1024 characters
   char tmp[1024];

   va_list args;
   va_start(args, format);
   int ret = vsnprintf(tmp, 1024, format, args);
   va_end(args);

   HLH_rw_write(rw, tmp, strlen(tmp), 1);

   return ret;
}

size_t HLH_rw_write_u8(HLH_rw *rw, uint8_t val)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return 0;

   return HLH_rw_write(rw, &val, 1, 1);
}

size_t HLH_rw_write_u16(HLH_rw *rw, uint16_t val)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return 0;

   size_t res = 0;
   if(rw->endian==HLH_RW_LITTLE_ENDIAN)
   {
      res += HLH_rw_write_u8(rw, (uint8_t)(val & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 8) & 255));
   }
   else if(rw->endian==HLH_RW_BIG_ENDIAN)
   {
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 8) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)(val & 255));
   }

   return res;
}

size_t HLH_rw_write_u32(HLH_rw *rw, uint32_t val)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return 0;

   size_t res = 0;
   if(rw->endian==HLH_RW_LITTLE_ENDIAN)
   {
      res += HLH_rw_write_u8(rw, (uint8_t)((val) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 8) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 16) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 24) & 255));
   }
   else if(rw->endian==HLH_RW_BIG_ENDIAN)
   {
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 24) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 16) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 8) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val) & 255));
   }

   return res;
}

size_t HLH_rw_write_u64(HLH_rw *rw, uint64_t val)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return 0;

   size_t res = 0;
   if(rw->endian==HLH_RW_LITTLE_ENDIAN)
   {
      res += HLH_rw_write_u8(rw, (uint8_t)((val) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 8) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 16) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 24) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 32) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 40) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 48) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 56) & 255));
   }
   else if(rw->endian==HLH_RW_BIG_ENDIAN)
   {
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 56) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 48) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 40) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 32) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 24) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 16) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val >> 8) & 255));
      res += HLH_rw_write_u8(rw, (uint8_t)((val) & 255));
   }

   return res;
}

uint8_t HLH_rw_read_u8(HLH_rw *rw)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return 0;

   uint8_t b0 = 0;
   HLH_rw_read(rw, &b0, 1, 1);

   return b0;
}

uint16_t HLH_rw_read_u16(HLH_rw *rw)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return 0;

   uint16_t b0 = HLH_rw_read_u8(rw);
   uint16_t b1 = HLH_rw_read_u8(rw);

   if(rw->endian==HLH_RW_LITTLE_ENDIAN)
      return (b1 << 8) | (b0);
   else if(rw->endian==HLH_RW_BIG_ENDIAN)
      return (b0 << 8) | (b1);

   return 0;
}

uint32_t HLH_rw_read_u32(HLH_rw *rw)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return 0;

   uint32_t b0 = HLH_rw_read_u8(rw);
   uint32_t b1 = HLH_rw_read_u8(rw);
   uint32_t b2 = HLH_rw_read_u8(rw);
   uint32_t b3 = HLH_rw_read_u8(rw);

   if(rw->endian==HLH_RW_LITTLE_ENDIAN)
      return (b3 << 24) | (b2 << 16) | (b1 << 8) | (b0);
   else if(rw->endian==HLH_RW_BIG_ENDIAN)
      return (b0 << 24) | (b1 << 16) | (b2 << 8) | (b3);

   return 0;
}

uint64_t HLH_rw_read_u64(HLH_rw *rw)
{
   if(rw==NULL||!HLH_rw_valid(rw))
      return 0;

   uint64_t b0 = HLH_rw_read_u8(rw);
   uint64_t b1 = HLH_rw_read_u8(rw);
   uint64_t b2 = HLH_rw_read_u8(rw);
   uint64_t b3 = HLH_rw_read_u8(rw);
   uint64_t b4 = HLH_rw_read_u8(rw);
   uint64_t b5 = HLH_rw_read_u8(rw);
   uint64_t b6 = HLH_rw_read_u8(rw);
   uint64_t b7 = HLH_rw_read_u8(rw);

   if(rw->endian==HLH_RW_LITTLE_ENDIAN)
      return (b7 << 56) | (b6 << 48) | (b5 << 40) | (b4 << 32) | (b3 << 24) | (b2 << 16) | (b1 << 8) | (b0);
   else if(rw->endian==HLH_RW_BIG_ENDIAN)
      return (b0 << 56) | (b1 << 48) | (b2 << 40) | (b3 << 32) | (b4 << 24) | (b5 << 16) | (b6 << 8) | (b7);

   return 0;
}

#undef HLH_MALLOC
#undef HLH_FREE
#undef HLH_REALLOC
#undef HLH_rw_max

#endif
#endif
