/*
SLK_make - a build-system

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
//-------------------------------------

//Internal includes
#include "error.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
int line_on = 0;
char *file_on = 0;
int mk_debug_on = 0;
char *file_contents = 0;
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

void mk_error(char *format,...)
{
   if(file_on)
      fprintf(stderr, "%s:%d: ", file_on, line_on);

   va_list args;

   va_start(args,format);
   vfprintf(stderr, format, args);
   va_end(args);

   exit(1);
}

void mk_debug(char *format,...)
{
   if(mk_debug_on)
   {
      va_list args;

      va_start(args,format);
      vfprintf(stderr, format, args);
      va_end(args);
   }
}

//-------------------------------------
