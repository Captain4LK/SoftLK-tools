/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
#include <string.h>
#include "HLH.h"
//-------------------------------------

//Internal includes
#include "project.h"
#include "layer.h"
#include "undo.h"
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

Project *project_new(int32_t width, int32_t height, const Settings *settings)
{
   if(width<=0||height<=0)
      return NULL;

   Project *p = calloc(1,sizeof(*p));
   p->width = width;
   p->height = height;
   p->num_layers = 2;
   p->layers = calloc(1,sizeof(*p->layers));
   p->layers[0] = layer_new(width*height);
   p->layers[1] = layer_new(width*height);
   p->undo_map = HLH_bitmap_create(((width+15)/16)*((height+15)/16));
   p->old = layer_new(width*height);

   undo_init(p);
   undo_reset(p);

   p->combined = project_to_image32(p,settings);

   return p;
}

Project *project_from_image8(Settings *settings, const Image8 *img)
{
   if(img==NULL)
      return NULL;

   Project *p = project_new(img->width,img->height,settings);

   settings->palette_colors = img->color_count;
   for(int i = 0;i<256;i++)
      settings->palette[i] = img->palette[i];
   for(int i = 0;i<img->width*img->height;i++)
      p->layers[0]->data[i] = img->data[i];

   project_update_full(p,settings);

   return p;
}

Image32 *project_to_image32(const Project *project, const Settings *settings)
{
   if(project==NULL)
      return NULL;

   Image32 *img = image32_new(project->width,project->height);
   if(img==NULL)
      return NULL;

   for(int i = 0;i<project->num_layers;i++)
   {
      if(i==0)
      {
         for(int j = 0;j<project->width*project->height;j++)
         {
            img->data[j] = settings->palette[project->layers[i]->data[j]];
         }
      }
      else
      {
         for(int j = 0;j<project->width*project->height;j++)
         {
            if(project->layers[i]->data[j])
               img->data[j] = settings->palette[project->layers[i]->data[j]];
         }
      }
   }

   return img;
}

Image8 *project_to_image8(const Project *project, const Settings *settings)
{
   if(project==NULL)
      return NULL;

   Image8 *img = image8_new(project->width,project->height);
   if(img==NULL)
      return NULL;

   memcpy(img->palette,settings->palette,sizeof(settings->palette));
   for(int i = 0;i<project->num_layers;i++)
   {
      if(i==0)
      {
         for(int j = 0;j<project->width*project->height;j++)
         {
            img->data[j] = project->layers[i]->data[j];
         }
      }
      else
      {
         for(int j = 0;j<project->width*project->height;j++)
         {
            if(project->layers[i]->data[j])
               img->data[j] = project->layers[i]->data[j];
         }
      }
   }

   return img;
}

void project_update(Project *project, int x, int y, const Settings *settings)
{
   project->combined->data[y*project->width+x] = settings->palette[project->layers[0]->data[y*project->width+x]];

   for(int i = 1;i<project->num_layers;i++)
   {
      if(project->layers[i]->data[y*project->width+x])
         project->combined->data[y*project->width+x] = settings->palette[project->layers[i]->data[y*project->width+x]];
   }
}

void project_update_full(Project *project, const Settings *settings)
{
   for(int i = 0;i<project->num_layers;i++)
   {
      if(i==0)
      {
         for(int j = 0;j<project->width*project->height;j++)
         {
            project->combined->data[j] = settings->palette[project->layers[i]->data[j]];
         }
      }
      else
      {
         for(int j = 0;j<project->width*project->height;j++)
         {
            if(project->layers[i]->data[j])
               project->combined->data[j] = settings->palette[project->layers[i]->data[j]];
         }
      }
   }
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
