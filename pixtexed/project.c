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
   p->layers = calloc(p->num_layers,sizeof(*p->layers));
   p->layers[0] = layer_new(width*height);
   p->layers[1] = layer_new(width*height);
   p->undo_map = HLH_bitmap_create(((width+15)/16)*((height+15)/16));
   p->bitmap = HLH_bitmap_create(width*height);
   p->old = layer_new(width*height);

   undo_init(p);
   undo_reset(p);

   layer_update_settings(p->layers[0],settings);
   layer_update_settings(p->layers[1],settings);

   p->combined = image32_new(p->width,p->height);
   p->combined8 = image8_new(p->width,p->height);
   project_update_full(p,settings);

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

   return image32_dup(project->combined);
}

Image8 *project_to_image8(const Project *project, const Settings *settings)
{
   if(project==NULL)
      return NULL;

   return image8_dup(project->combined8);
}

void project_update(Project *project, int x, int y, const Settings *settings)
{
   int first = 1;
   for(int i = 0;i<project->num_layers;i++)
   {
      if(project->layers[i]->hidden)
         continue;

      if(first)
      {
         first = 0;
         project->combined->data[y*project->width+x] = settings->palette[project->layers[i]->data[y*project->width+x]];
         project->combined8->data[y*project->width+x] = project->layers[i]->data[y*project->width+x];
      }
      else
      {
         if(project->layers[i]->type==LAYER_BLEND)
         {
            uint8_t blend = project->layers[i]->lut[project->combined8->data[y*project->width+x]][project->layers[i]->data[y*project->width+x]];
            project->combined->data[y*project->width+x] = settings->palette[blend];
            project->combined8->data[y*project->width+x] = blend;
         }
      }
      /*if(first)
      {
         first = 0;
         project->combined->data[y*project->width+x] = settings->palette[project->layers[i]->data[y*project->width+x]];
      }
      else
      {
         if(project->layers[i]->data[y*project->width+x])
            project->combined->data[y*project->width+x] = settings->palette[project->layers[i]->data[y*project->width+x]];
      }*/
   }
}

void project_update_full(Project *project, const Settings *settings)
{
   int first = 1;
   for(int i = 0;i<project->num_layers;i++)
   {
      if(project->layers[i]->hidden)
         continue;

      if(first)
      {
         first = 0;
         for(int j = 0;j<project->width*project->height;j++)
         {
            project->combined->data[j] = settings->palette[project->layers[i]->data[j]];
            project->combined8->data[j] = project->layers[i]->data[j];
         }
      }
      else
      {
         for(int j = 0;j<project->width*project->height;j++)
         {
            uint8_t blend = project->layers[i]->lut[project->combined8->data[j]][project->layers[i]->data[j]];
            project->combined->data[j] = settings->palette[blend];
            project->combined8->data[j] = blend;
         }
      }

      /*if(first)
      {
         first = 0;
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
      }*/
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

void project_layer_add(Project *project, int pos)
{
   if(project==NULL)
      return;

   project->num_layers++;
   project->layers = realloc(project->layers,sizeof(*project->layers)*project->num_layers);
   for(int i = project->num_layers-1;i>pos;i--)
      project->layers[i] = project->layers[i-1];
   project->layers[pos] = layer_new(project->width*project->height);
}

void project_layer_delete(Project *project, int pos)
{
   if(project==NULL)
      return;
   if(project->num_layers<=2)
      return;

   if(pos>=project->num_layers-1)
      return;

   layer_free(project->layers[pos]);
   for(int i = pos;i<project->num_layers-1;i++)
      project->layers[i] = project->layers[i+1];
   project->num_layers--;
}
//-------------------------------------
