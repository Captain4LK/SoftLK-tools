/*
HLH_gui - gui framework

Written in 2023,2024,2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

//External includes
//-------------------------------------

//Internal includes
#include "HLH_gui.h"
#include "HLH.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static uint64_t next_id = 0;
//-------------------------------------

//Function prototypes
static void element_redraw_intern(HLH_gui_element *e);
//-------------------------------------

//Function implementations

HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, HLH_gui_flags flags, HLH_gui_msg_handler msg_handler)
{
   HLH_gui_element *e = calloc(1, bytes);
   e->flags = flags;
   e->msg_base = msg_handler;
   e->timer = 0;
   e->type = HLH_GUI_UNKNOWN;
   e->id = next_id++;

   if(parent!=NULL)
   {
      e->window = parent->window;

      if(!flags.no_parent)
      {
         e->parent = parent;
         parent->child_count++;
         parent->children = realloc(parent->children, sizeof(*parent->children) * parent->child_count);
         parent->children[parent->child_count - 1] = e;
      }
      if(parent->flags.overlay)
         e->flags.overlay = true;
   }

   return e;
}

int64_t HLH_gui_element_msg_direct(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp)
{
   if(e == NULL)
      return 0;
   if(e->flags.destroy && msg != HLH_GUI_MSG_DESTROY)
      return 0;

   if(msg == HLH_GUI_MSG_DRAW && e->flags.invisible)
      return 0;
   if(e->window->blocking != NULL && (msg < HLH_GUI_MSG_NO_BLOCK_START || msg > HLH_GUI_MSG_NO_BLOCK_END))
      return 0;

   if(e->msg_usr!=NULL)
   {
      int res = e->msg_usr(e, msg, di, dp);
      if(res)
         return res;
   }

   if(e->msg_base!=NULL)
      return e->msg_base(e, msg, di, dp);
   return 0;
}

int64_t HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp)
{
   if(e==NULL)
      return 0;
   if(e->flags.destroy && msg != HLH_GUI_MSG_DESTROY)
      return 0;

   //TODO(Captain4LK): which msgs can we safely ignore while ignored?
   if(HLH_gui_element_ignored(e))
   {
      switch(msg)
      {
      case HLH_GUI_MSG_MOUSE:
      case HLH_GUI_MSG_MOUSE_LEAVE:
      case HLH_GUI_MSG_DRAW:
      case HLH_GUI_MSG_GET_WIDTH:
      case HLH_GUI_MSG_GET_HEIGHT:
      case HLH_GUI_MSG_GET_CHILD_PAD:
         return 0;
      default:
         break;
      }
   }

   if(msg==HLH_GUI_MSG_DRAW && e->flags.invisible)
      return 0;
   if(e->window->blocking!=NULL&&(msg<HLH_GUI_MSG_NO_BLOCK_START||msg>HLH_GUI_MSG_NO_BLOCK_END))
      return 0;

   if(e->msg_usr!=NULL)
   {
      int res = e->msg_usr(e, msg, di, dp);
      if(res)
         return res;
   }

   if(e->msg_base!=NULL)
      return e->msg_base(e, msg, di, dp);
   return 0;
}

int64_t HLH_gui_element_msg_all(HLH_gui_element *e, HLH_gui_msg msg, int64_t di, void *dp)
{
   if(e==NULL)
      return 0;
   if(e->flags.destroy && msg!=HLH_GUI_MSG_DESTROY)
      return 0;
   if(HLH_gui_element_ignored(e))
      return 0;
   if(msg==HLH_GUI_MSG_DRAW && e->flags.invisible)
      return 0;
   for(int i = 0; i<e->child_count; i++)
      HLH_gui_element_msg_all(e->children[i], msg, di, dp);

   HLH_gui_element_msg(e, msg, di, dp);

   return 0;
}

void HLH_gui_element_redraw(HLH_gui_element *e)
{
   //TODO(Captain4LK): Separate lists for overlay, normal --> get rid of redraw_now
   if(e->flags.overlay || e->flags.no_parent)
   {
      HLH_gui_element_redraw_now(e);
      return;
   }

   e->window->redraw = true;
}

void HLH_gui_element_redraw_now(HLH_gui_element *e)
{
   if(e->flags.overlay)
   {
      if(!SDL_SetRenderTarget(e->window->sdl_renderer, e->window->sdl_overlay))
         fprintf(stderr, "SDL_SetRenderTarget() failed: %s\n", SDL_GetError());
   }
   else
   {
      if(!SDL_SetRenderTarget(e->window->sdl_renderer, e->window->sdl_target))
         fprintf(stderr, "SDL_SetRenderTarget(): %s\n", SDL_GetError());
   }

   element_redraw_intern(e);

   if(!SDL_SetRenderTarget(e->window->sdl_renderer, NULL))
      fprintf(stderr, "SDL_SetRenderTarget(): %s\n", SDL_GetError());
   if(!SDL_RenderClear(e->window->sdl_renderer))
      fprintf(stderr, "SDL_RenderClear(): %s\n", SDL_GetError());
   if(!SDL_RenderTexture(e->window->sdl_renderer, e->window->sdl_target, NULL, NULL))
      fprintf(stderr, "SDL_RenderCopy(): %s\n", SDL_GetError());
   if(!SDL_RenderTexture(e->window->sdl_renderer, e->window->sdl_overlay, NULL, NULL))
      fprintf(stderr, "SDL_RenderCopy(): %s\n", SDL_GetError());
   SDL_RenderPresent(e->window->sdl_renderer);
}

void HLH_gui_element_redraw_msg(HLH_gui_element *e)
{
   e->window->clip.min[0] = -1;
   e->window->clip.min[1] = -1;
   e->window->clip.max[0] = -1;
   e->window->clip.max[1] = -1;
   e->window->translation[0] = 0;
   e->window->translation[1] = 0;
   HLH_gui_draw_disable_clip(e);

   if(!e->flags.ignore)
   {
      element_redraw_intern(e);
   }
}

HLH_gui_element *HLH_gui_element_by_point(HLH_gui_element *e, HLH_gui_point pt)
{
   int32_t old_trans[2] = {e->window->translation[0], e->window->translation[1]};
   e->window->translation[0] += e->translate[0];
   e->window->translation[1] += e->translate[1];

   for(int i = 0; i<e->child_count; i++)
   {
      HLH_gui_element *c = e->children[i];

      if(HLH_gui_element_ignored(c))
         continue;

      HLH_gui_rect b;
      b.min[0] = c->bounds.min[0] + e->window->translation[0];
      b.min[1] = c->bounds.min[1] + e->window->translation[1];
      b.max[0] = c->bounds.max[0] + e->window->translation[0];
      b.max[1] = c->bounds.max[1] + e->window->translation[1];

      if(HLH_gui_rect_inside(c->bounds, pt))
      {
         HLH_gui_element *leaf = HLH_gui_element_by_point(c, pt);

         if(leaf!=NULL)
            return leaf;

         return c;
      }
   }

   e->window->translation[0] = old_trans[0];
   e->window->translation[1] = old_trans[1];

   return NULL;
}

void HLH_gui_element_invisible(HLH_gui_element *e, int invisible)
{
   if(invisible)
      e->flags.invisible = true;
   else
      e->flags.invisible = false;

   for(int i = 0; i<e->child_count; i++)
      HLH_gui_element_invisible(e->children[i], invisible);
}

int HLH_gui_element_ignored(HLH_gui_element *e)
{
   if(e==NULL)
      return 0;

   if(e->flags.ignore)
      return 1;

   HLH_gui_element *current = e->parent;
   while(current != NULL)
   {
      if(current->flags.ignore)
      {
         return 1;
      }
      current = current->parent;
   }

   return 0;
}

void HLH_gui_element_destroy(HLH_gui_element *e)
{
   for(int i = 0; i<e->child_count; i++)
      HLH_gui_element_destroy(e->children[i]);

   HLH_gui_element_msg(e, HLH_GUI_MSG_DESTROY, 0, NULL);
   if(e->timer!=0)
      SDL_RemoveTimer(e->timer);
   if(e->children!=NULL)
      free(e->children);
   free(e);
}

void HLH_gui_element_timer(HLH_gui_element *e, int interval)
{
   if(e->timer!=0)
   {
      SDL_RemoveTimer(e->timer);
      e->timer = 0;
   }
   e->timer_interval = interval;
   e->timer = SDL_AddTimer(interval, core_timer_callback, e);
}

static void element_redraw_intern(HLH_gui_element *e)
{
   if(e->flags.invisible ||HLH_gui_element_ignored(e))
      return;

   HLH_gui_element_msg(e, HLH_GUI_MSG_DRAW, 0, NULL);

   HLH_gui_rect old_clip = e->window->clip;
   if(e->flags.clip)
   {
      HLH_gui_draw_set_clip_rect(e, e->bounds);
      e->window->clip = e->bounds;
   }

   int32_t old_trans[2] = {e->window->translation[0], e->window->translation[1]};
   e->window->translation[0] += e->translate[0];
   e->window->translation[1] += e->translate[1];

   for(int i = 0; i<e->child_count; i++)
      element_redraw_intern(e->children[i]);

   if(e->flags.clip)
   {
      if(old_clip.min[0] == -1 && old_clip.min[1] == -1 &&
         old_clip.max[0] == -1 && old_clip.max[1] == -1)
      {
         HLH_gui_draw_disable_clip(e);
      }
      else
      {
         HLH_gui_draw_set_clip_rect(e, old_clip);
         e->window->clip = old_clip;
      }
   }

   e->window->translation[0] = old_trans[0];
   e->window->translation[1] = old_trans[1];
}
//-------------------------------------
