/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include "HLH.h"
//-------------------------------------

//Internal includes
#include "project.h"
#include "layer.h"
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

Project *project_new(int32_t width, int32_t height)
{
   if(width<=0||height<=0)
      return NULL;

   Project *p = calloc(1,sizeof(*p));
   p->width = width;
   p->height = height;
   p->num_layers = 1;
   p->combined = image32_new(width,height);
   p->layers = calloc(1,sizeof(*p->layers));
   p->layers[0] = layer_new(width*height);
   p->undo_map = HLH_bitmap_create(((width+15)/16)*((height+15)/16));
   p->old = layer_new(width*height);
   p->palette[0] = 0xff000000;
   p->palette[1] = 0xff342022;
   p->palette[2] = 0xff3c2845;
   p->palette[3] = 0xff313966;
   p->palette[4] = 0xff3b568f;
   p->palette[5] = 0xff2671e0;
   p->palette[6] = 0xff66a0d9;
   p->palette[7] = 0xff9ac3ef;
   p->palette[8] = 0xff36f2fb;
   p->palette[9] = 0xff50e599;
   p->palette[10] = 0xff30be6a;
   p->palette[11] = 0xff6e9437;
   p->palette[12] = 0xff2f694b;
   p->palette[13] = 0xff244b52;
   p->palette[14] = 0xff393c32;
   p->palette[15] = 0xff743f3f;
   p->palette[16] = 0xff826030;
   p->palette[17] = 0xffe16e5b;
   p->palette[18] = 0xffff9b63;
   p->palette[19] = 0xffe4cd5f;
   p->palette[20] = 0xfffcdbcb;
   p->palette[21] = 0xffffffff;
   p->palette[22] = 0xffb7ad9b;
   p->palette[23] = 0xff877e84;
   p->palette[24] = 0xff6a6a69;
   p->palette[25] = 0xff525659;
   p->palette[26] = 0xff8a4276;
   p->palette[27] = 0xff3232ac;
   p->palette[28] = 0xff6357d9;
   p->palette[29] = 0xffba7bd7;
   p->palette[30] = 0xff4a978f;
   p->palette[31] = 0xff306f8a;

   return p;
}

Image32 *project_to_image32(const Project *project)
{
   if(project==NULL)
      return NULL;

   Image32 *img = image32_new(project->width,project->height);
   if(img==NULL)
      return NULL;

   for(int i = 0;i<project->num_layers;i++)
   {
      for(int j = 0;j<project->width*project->height;j++)
      {
         img->data[j] = project->palette[project->layers[i]->data[j]];
      }
   }

   return img;
}

void project_free(Project *project)
{
   if(project==NULL)
      return;

   for(int i = 0;i<project->num_layers;i++)
      layer_free(project->layers[i]);
   
   free(project->layers);
   free(project);
}
//-------------------------------------
