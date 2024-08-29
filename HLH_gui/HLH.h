/*
Misc helper macros/functions/data structures etc.

Written in 2022,2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

/* 
   To create implementation (the function definitions) add
      #define HLH_IMPLEMENTATION
   before including this file in *one* C file (translation unit)
*/

/*
   malloc(), realloc() and free() can be overwritten by 
   defining the following macros:

   HLH_MALLOC
   HLH_FREE
   HLH_REALLOC
*/

#ifndef _HLH_H_

#define _HLH_H_

#include <stdint.h>
#include <stddef.h>

//General purpose macros
//--------------------------------
#define HLH_max(a,b) ((a)>(b)?(a):(b))
#define HLH_min(a,b) ((a)<(b)?(a):(b))
#define HLH_non_zero(a) ((a)+((a)==0))
//--------------------------------

//Error checking
//--------------------------------
#define HLH_log_line(w, ...) _HLH_logl(w, __FILE__, __LINE__, __VA_ARGS__)
#define HLH_error_fail(w, ...) do { HLH_log_line(w, __VA_ARGS__); goto HLH_err; } while(0)
#define HLH_error_check(X, w, ...) do { if(!(X)) HLH_error_fail(w, __VA_ARGS__); } while(0)
//--------------------------------

//Dynamic array/"stretchy buffer"
//--------------------------------

//Usage example:
//
//int *a = NULL;
//HLH_array_push(a,4);
//HLH_array_push(a,3);
//
//printf("%d\n",a[0]); --> 4
//printf("%d\n",HLH_array_length(a)); --> 2
//
//HLH_array_free(a);

//Important: 'a' may be modified by HLH_array_push() and HLH_array_length_set()!
//If an array is passed to a function as an argument, it must be done as a double pointer.
//e.g: void foo(int **a) { HLH_array_push(*a,1); }
#define HLH_array_push(a,o) (HLH_array_maygrow(a,1),(a)[HLH_array_header(a)->length++] = (o))
#define HLH_array_length(a) ((a)!=NULL?HLH_array_header(a)->length:0)
#define HLH_array_length_set(a,n) do { if((a)==NULL) HLH_array_grow((a),1,(n)); else HLH_array_maygrow((a),(n)-HLH_array_header(a)->length); HLH_array_header(a)->length = (n); }while(0)
#define HLH_array_free(a) ((a)!=NULL?(_HLH_free(a-HLH_array_header_offset(a)),0):0)

//Internal
typedef struct
{
   size_t length;
   size_t size;
}HLH_aheader;
#define HLH_array_header(a) ((HLH_aheader *)(a-HLH_array_header_offset(a)))
#define HLH_array_header_offset(a) ((sizeof(HLH_aheader)+sizeof(*a)-1)/sizeof(*a))
#define HLH_array_maygrow(a,n) (((a)==NULL||HLH_array_header(a)->length+n>HLH_array_header(a)->size)?(HLH_array_grow(a,n,0),0):0)
#define HLH_array_grow(a,n,min) ((a) = _HLH_array_grow(a,sizeof(*a),n,min))
void *_HLH_array_grow(void *old, size_t size, size_t grow, size_t min);
//--------------------------------

//Bitmaps
//--------------------------------
uint32_t *HLH_bitmap_create(intptr_t bits);
void HLH_bitmap_free(uint32_t *bitmap);
void HLH_bitmap_set(uint32_t *bitmap, intptr_t bit);
void HLH_bitmap_unset(uint32_t *bitmap, intptr_t bit);
int HLH_bitmap_check(uint32_t *bitmap, intptr_t bit);
intptr_t HLH_bitmap_first_set(uint32_t *bitmap);
void HLH_bitmap_clear(uint32_t *bitmap);
//--------------------------------

//Internal
//--------------------------------

//malloc family functions are hidden behind functions
//so that they only need to be overriden with HLH_MALLOC/-FREE/-REALLOC in
//the file containing the implementation
void *_HLH_malloc(size_t size);
void _HLH_free(void *ptr);
void *_HLH_realloc(void *ptr, size_t new_size);

void _HLH_logl(const char *fun, const char *file, unsigned line, const char *format, ...);
//--------------------------------

#endif

#ifdef HLH_IMPLEMENTATION
#ifndef HLH_IMPLEMENTATION_ONCE
#define HLH_IMPLEMENTATION_ONCE

#ifndef HLH_MALLOC
#define HLH_MALLOC malloc
#endif

#ifndef HLH_FREE
#define HLH_FREE free
#endif

#ifndef HLH_REALLOC
#define HLH_REALLOC realloc
#endif

#include <stdarg.h>

void *_HLH_malloc(size_t size)
{
   return HLH_MALLOC(size);
}

void _HLH_free(void *ptr)
{
   HLH_FREE(ptr);
}

void *_HLH_realloc(void *ptr, size_t new_size)
{
   return HLH_REALLOC(ptr,new_size);
}

void *_HLH_array_grow(void *old, size_t size, size_t grow, size_t min)
{
   size_t header_off = (sizeof(HLH_aheader)+size-1)/size;

   if(min<4)
      min = 4;
   if(old==NULL)
   {
      //goodbye c++
      char *new = _HLH_malloc(header_off*size+min*size);
      HLH_aheader *h = (HLH_aheader *)new;
      h->length = 0;
      h->size = min;

      return new+header_off*size;
   }

   HLH_aheader *h = (HLH_aheader *)(((char *)old)-header_off*size);
   if(h->size<min)
      h->size = min;
   while(h->size<h->length+grow)
      h->size*=2;
   h = _HLH_realloc(h,header_off*size+h->size*size);

   return ((char *)h)+header_off*size;
}

uint32_t *HLH_bitmap_create(intptr_t bits)
{
   uint32_t *bitmap = NULL;
   HLH_array_length_set(bitmap,(bits+31)/32);
   //printf("pre %d\n",HLH_array_length(bitmap));
   memset(bitmap,0,sizeof(*bitmap)*HLH_array_length(bitmap));
   //printf("%d\n",HLH_array_length(bitmap));

   return bitmap;
}

void HLH_bitmap_free(uint32_t *bitmap)
{
   HLH_array_free(bitmap);
}

void HLH_bitmap_set(uint32_t *bitmap, intptr_t bit)
{
   if(bitmap==NULL)
      return;

   intptr_t pos = bit/32;
   uint32_t value = (uint32_t)1<<(bit-pos*32);
   bitmap[pos]|=value;
}

void HLH_bitmap_unset(uint32_t *bitmap, intptr_t bit)
{
   if(bitmap==NULL)
      return;

   intptr_t pos = bit/32;
   uint32_t old = bitmap[pos];
   uint32_t value = (uint32_t)1<<(bit-pos*32);
   bitmap[pos]&=~value;
}

int HLH_bitmap_check(uint32_t *bitmap, intptr_t bit)
{
   if(bitmap==NULL)
      return 0;

   intptr_t pos = bit/32;
   uint32_t value = 1<<(bit-pos*32);
   return !!(bitmap[pos]&value);
}

intptr_t HLH_bitmap_first_set(uint32_t *bitmap)
{
   if(bitmap==NULL)
      return -1;

   for(int i = 0;i<HLH_array_length(bitmap);i++)
   {
      if(bitmap[i]==0)
         continue;

      uint32_t val = bitmap[i];
      //uint32_t pos = (intptr_t)i*32;
      intptr_t pos = 1;

      if((val&0x0000ffff)==0) { pos+=16; val>>=16; }
      if((val&0x000000ff)==0) { pos+=8; val>>=8; }
      if((val&0x0000000f)==0) { pos+=4; val>>=4; }
      if((val&0x00000003)==0) { pos+=2; val>>=2; }
      pos = pos-(val&1);
      pos+=(intptr_t)i*32;
      //if(val<=0x0000ffff) { pos+=16; val<<=16; }
      //if(val<=0x00ffffff) { pos+=8; val<<=8; }
      //if(val<=0x0fffffff) { pos+=4; val<<=4; }
      //if(val<=0x3fffffff) { pos+=2; val<<=2; }
      //if(val<=0x7fffffff) { pos+=1; val<<=1; }

      return pos;
   }

   return -1;
}

void HLH_bitmap_clear(uint32_t *bitmap)
{
   memset(bitmap,0,sizeof(*bitmap)*HLH_array_length(bitmap));
}

void _HLH_logl(const char *fun, const char *file, unsigned line, const char *format, ...)
{
   fprintf(stderr, "%s (%s:%u): ", fun, file, line);

   va_list args;
   va_start(args, format);
   vfprintf(stderr, format, args);
   va_end(args);
}

#undef HLH_MALLOC
#undef HLH_FREE
#undef HLH_REALLOC

#endif
#endif
