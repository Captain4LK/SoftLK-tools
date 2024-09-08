/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
//-------------------------------------

//Internal includes
#include "settings.h"
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

Settings *settings_init()
{
   Settings *s = calloc(1,sizeof(*s));

   //Palette
   //-------------------------------------
   s->palette_colors = 32;
   s->palette[0] = 0xff000000;
   s->palette[1] = 0xff342022;
   s->palette[2] = 0xff3c2845;
   s->palette[3] = 0xff313966;
   s->palette[4] = 0xff3b568f;
   s->palette[5] = 0xff2671e0;
   s->palette[6] = 0xff66a0d9;
   s->palette[7] = 0xff9ac3ef;
   s->palette[8] = 0xff36f2fb;
   s->palette[9] = 0xff50e599;
   s->palette[10] = 0xff30be6a;
   s->palette[11] = 0xff6e9437;
   s->palette[12] = 0xff2f694b;
   s->palette[13] = 0xff244b52;
   s->palette[14] = 0xff393c32;
   s->palette[15] = 0xff743f3f;
   s->palette[16] = 0xff826030;
   s->palette[17] = 0xffe16e5b;
   s->palette[18] = 0xffff9b63;
   s->palette[19] = 0xffe4cd5f;
   s->palette[20] = 0xfffcdbcb;
   s->palette[21] = 0xffffffff;
   s->palette[22] = 0xffb7ad9b;
   s->palette[23] = 0xff877e84;
   s->palette[24] = 0xff6a6a69;
   s->palette[25] = 0xff525659;
   s->palette[26] = 0xff8a4276;
   s->palette[27] = 0xff3232ac;
   s->palette[28] = 0xff6357d9;
   s->palette[29] = 0xffba7bd7;
   s->palette[30] = 0xff4a978f;
   s->palette[31] = 0xff306f8a;
   //-------------------------------------

   //Brushes
   //-------------------------------------
   
   //Square 1x1
   s->brushes[0] = calloc(1,sizeof(*s->brushes[0])+1);
   s->brushes[0]->width = 1;
   s->brushes[0]->height = 1;
   s->brushes[0]->data[0] = 1;

   //Square 2x2
   s->brushes[1] = calloc(1,sizeof(*s->brushes[1])+4);
   s->brushes[1]->width = 2;
   s->brushes[1]->height = 2;
   for(int i = 0;i<4;i++) s->brushes[1]->data[i] = 1;

   //Square 3x3
   s->brushes[2] = calloc(1,sizeof(*s->brushes[2])+9);
   s->brushes[2]->width = 3;
   s->brushes[2]->height = 3;
   for(int i = 0;i<9;i++) s->brushes[2]->data[i] = 1;

   //Square 4x4
   s->brushes[3] = calloc(1,sizeof(*s->brushes[3])+16);
   s->brushes[3]->width = 4;
   s->brushes[3]->height = 4;
   for(int i = 0;i<16;i++) s->brushes[3]->data[i] = 1;

   //Square 5x5
   s->brushes[4] = calloc(1,sizeof(*s->brushes[4])+25);
   s->brushes[4]->width = 5;
   s->brushes[4]->height = 5;
   for(int i = 0;i<25;i++) s->brushes[4]->data[i] = 1;

   //Square 7x7
   s->brushes[5] = calloc(1,sizeof(*s->brushes[5])+49);
   s->brushes[5]->width = 7;
   s->brushes[5]->height = 7;
   for(int i = 0;i<49;i++) s->brushes[5]->data[i] = 1;

   //Square 8x8
   s->brushes[6] = calloc(1,sizeof(*s->brushes[6])+64);
   s->brushes[6]->width = 8;
   s->brushes[6]->height = 8;
   for(int i = 0;i<64;i++) s->brushes[6]->data[i] = 1;

   //Square 12x12
   s->brushes[7] = calloc(1,sizeof(*s->brushes[7])+144);
   s->brushes[7]->width = 12;
   s->brushes[7]->height = 12;
   for(int i = 0;i<144;i++) s->brushes[7]->data[i] = 1;

   //Square 16x16
   s->brushes[8] = calloc(1,sizeof(*s->brushes[8])+256);
   s->brushes[8]->width = 16;
   s->brushes[8]->height = 16;
   for(int i = 0;i<256;i++) s->brushes[8]->data[i] = 1;

   //Square sieve 16x16
   s->brushes[9] = calloc(1,sizeof(*s->brushes[9])+256);
   s->brushes[9]->width = 16;
   s->brushes[9]->height = 16;
   for(int i = 0;i<256;i++) if(((i&1)+i/16)&1) s->brushes[9]->data[i] = 1;
   //-------------------------------------

   s->palette_selected = 1;

   return s;
}

void settings_free(Settings *s)
{
   if(s==NULL)
      return;

   for(int i = 0;i<64;i++)
   {
      if(s->brushes[i]!=NULL)
         free(s->brushes[i]);
      s->brushes[i] = NULL;
   }
}
//-------------------------------------
