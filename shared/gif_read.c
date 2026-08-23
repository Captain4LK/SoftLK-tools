/*
SLK_img2pixel - a tool for converting images to pixelart

Multi-frame GIF reader - see gif_read.h.

Only TU with a full stb_image build (HLH_gui_core.c uses STB_IMAGE_STATIC,
so its copy is file-local - no symbol clash).

Written in 2026 by June / SerbianKnifeFight, released to the public domain (CC0), same terms as the
rest of this project - see COPYING.
*/

//External includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO //we only ever decode from an in-memory buffer here
#include "../external/stb_image.h"
//-------------------------------------

//Internal includes
#include "gif_read.h"
//-------------------------------------

int SLK_gif_read_all_frames(const char *path, Image32 ***out_frames, int *out_frame_count, int **out_delays_cs)
{
   if(path==NULL||out_frames==NULL||out_frame_count==NULL)
      return 0;

   FILE *f = fopen(path,"rb");
   if(f==NULL)
      return 0;

   fseek(f,0,SEEK_END);
   long file_size = ftell(f);
   fseek(f,0,SEEK_SET);
   if(file_size<=0)
   {
      fclose(f);
      return 0;
   }

   uint8_t *file_data = malloc((size_t)file_size);
   if(file_data==NULL)
   {
      fclose(f);
      return 0;
   }

   size_t read = fread(file_data,1,(size_t)file_size,f);
   fclose(f);
   if(read!=(size_t)file_size)
   {
      free(file_data);
      return 0;
   }

   int width,height,layers,comp;
   int *delays_ms = NULL;
   uint8_t *decoded = stbi_load_gif_from_memory(file_data,(int)file_size,&delays_ms,&width,&height,&layers,&comp,4);
   free(file_data);

   if(decoded==NULL||width<=0||height<=0||layers<=0)
   {
      if(decoded!=NULL)
         stbi_image_free(decoded);
      if(delays_ms!=NULL)
         free(delays_ms);
      return 0;
   }

   Image32 **frames = malloc(sizeof(*frames)*(size_t)layers);
   for(int i = 0;i<layers;i++)
   {
      Image32 *img = malloc(sizeof(*img)+sizeof(*img->data)*(size_t)width*(size_t)height);
      img->width = width;
      img->height = height;
      memcpy(img->data,decoded+(size_t)i*(size_t)width*(size_t)height*4,sizeof(*img->data)*(size_t)width*(size_t)height);
      frames[i] = img;
   }
   stbi_image_free(decoded);

   *out_frames = frames;
   *out_frame_count = layers;

   if(out_delays_cs!=NULL)
   {
      int *delays_cs = malloc(sizeof(*delays_cs)*(size_t)layers);
      for(int i = 0;i<layers;i++)
      {
         //stb reports delays in milliseconds; GIF's native unit is
         //centiseconds (hundredths of a second)
         int cs = delays_ms!=NULL?delays_ms[i]/10:10;
         delays_cs[i] = cs>0?cs:1;
      }
      *out_delays_cs = delays_cs;
   }

   if(delays_ms!=NULL)
      free(delays_ms);

   return 1;
}
