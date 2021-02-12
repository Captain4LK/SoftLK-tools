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
#include "strings.h"
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

char *form(char *form, ... )
{
   va_list args;
   char buf[1024];

   va_start(args,form);
   vsprintf(buf,form,args);
   va_end(args);

   return strdup(buf);
}
//-------------------------------------
