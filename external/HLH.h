/*
Misc helper macros/functions/data structures etc.

Written in 2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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

//General purpose macros
//--------------------------------
#define HLH_max(a,b) ((a)>(b)?(a):(b))
#define HLH_min(a,b) ((a)<(b)?(a):(b))
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
#define HLH_array_length_set(a,n) do { if((a)==NULL) HLH_array_grow(a,n,0); else HLH_array_maygrow(a,n-HLH_array_header(a)->length); HLH_array_header(a)->length = n; }while(0)
#define HLH_array_free(a) ((a)!=NULL?(_HLH_free(a-HLH_array_header_offset(a)),0):0)

//Internal
typedef struct
{
   size_t length;
   size_t size;
}HLH_aheader;
#define HLH_array_header(a) ((HLH_aheader *)(a-HLH_array_header_offset(a)))
#define HLH_array_header_offset(a) ((sizeof(HLH_aheader)*2-1)/sizeof(*a))
#define HLH_array_maygrow(a,n) (((a)==NULL||HLH_array_header(a)->length+n>HLH_array_header(a)->size)?(HLH_array_grow(a,n,0),0):0)
#define HLH_array_grow(a,n,min) ((a) = _HLH_array_grow(a,sizeof(*a),n,min))
static void *_HLH_array_grow(void *old, size_t size, size_t grow, size_t min);
//--------------------------------

//Internal
//--------------------------------

//malloc family functions are hidden behind functions
//so that they only need to be overriden with HLH_MALLOC/-FREE/-REALLOC in
//the file containing the implementation
static void *_HLH_malloc(size_t size);
static void _HLH_free(void *ptr);
static void *_HLH_realloc(void *ptr, size_t new_size);
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

static void *_HLH_malloc(size_t size)
{
   return HLH_MALLOC(size);
}

static void _HLH_free(void *ptr)
{
   HLH_FREE(ptr);
}

static void *_HLH_realloc(void *ptr, size_t new_size)
{
   return HLH_REALLOC(ptr,new_size);
}

static void *_HLH_array_grow(void *old, size_t size, size_t grow, size_t min)
{
   size_t header_off = (sizeof(HLH_aheader)*2-1)/size;

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

#endif
#endif
