/*
HLH_gui - gui framework

Written in 2023,2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
static void element_compute_required(HLH_gui_element *e);
static void element_set_bounds(HLH_gui_element *e, HLH_gui_rect availible);
static void element_redraw(HLH_gui_element *e);
static Uint32 sdl_callback(Uint32 interval, void *param);
//-------------------------------------

//Function implementations

HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint64_t flags, HLH_gui_msg_handler msg_handler)
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

      if(!(flags & HLH_GUI_NO_PARENT))
      {
         e->parent = parent;
         parent->child_count++;
         parent->children = realloc(parent->children, sizeof(*parent->children) * parent->child_count);
         parent->children[parent->child_count - 1] = e;
      }
      if(parent->flags & HLH_GUI_OVERLAY)
         e->flags |= HLH_GUI_OVERLAY;
   }

   return e;
}

int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(e==NULL)
      return 0;
   if(e->flags & HLH_GUI_DESTROY&&msg!=HLH_GUI_MSG_DESTROY)
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
      case HLH_GUI_MSG_GET_CHILD_SPACE:
         return 0;
      default:
         break;
      }
   }

   if(msg==HLH_GUI_MSG_DRAW&&e->flags & HLH_GUI_INVISIBLE)
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

int HLH_gui_element_msg_all(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(e==NULL)
      return 0;
   if(e->flags & HLH_GUI_DESTROY&&msg!=HLH_GUI_MSG_DESTROY)
      return 0;
   if(HLH_gui_element_ignored(e))
      return 0;
   if(msg==HLH_GUI_MSG_DRAW&&e->flags & HLH_GUI_INVISIBLE)
      return 0;
   for(int i = 0; i<e->child_count; i++)
      HLH_gui_element_msg_all(e->children[i], msg, di, dp);

   HLH_gui_element_msg(e, msg, di, dp);

   return 0;
}

void HLH_gui_element_redraw(HLH_gui_element *e)
{
   //TODO(Captain4LK): Separate lists for overlay, normal --> get rid of redraw_now
   if(e->flags & HLH_GUI_OVERLAY||e->flags&HLH_GUI_NO_PARENT)
   {
      HLH_gui_element_redraw_now(e);
      return;
   }

   e->needs_redraw = 1;
   HLH_array_push(e->window->redraw,e);
}

void HLH_gui_element_redraw_now(HLH_gui_element *e)
{
   if(e->flags & HLH_GUI_OVERLAY)
   {
      if(SDL_SetRenderTarget(e->window->renderer, e->window->overlay)<0)
         fprintf(stderr, "SDL_SetRenderTarget(): %s\n", SDL_GetError());
   }
   else
   {
      if(SDL_SetRenderTarget(e->window->renderer, e->window->target)<0)
         fprintf(stderr, "SDL_SetRenderTarget(): %s\n", SDL_GetError());
   }

   element_redraw(e);

   if(SDL_SetRenderTarget(e->window->renderer, NULL)<0)
      fprintf(stderr, "SDL_SetRenderTarget(): %s\n", SDL_GetError());
   if(SDL_RenderClear(e->window->renderer)<0)
      fprintf(stderr, "SDL_RenderClear(): %s\n", SDL_GetError());
   if(SDL_RenderCopy(e->window->renderer, e->window->target, NULL, NULL)<0)
      fprintf(stderr, "SDL_RenderCopy(): %s\n", SDL_GetError());
   if(SDL_RenderCopy(e->window->renderer, e->window->overlay, NULL, NULL)<0)
      fprintf(stderr, "SDL_RenderCopy(): %s\n", SDL_GetError());
   SDL_RenderPresent(e->window->renderer);
}

void HLH_gui_element_redraw_msg(HLH_gui_element *e)
{
   element_redraw(e);
}

void HLH_gui_element_layout(HLH_gui_element *e, HLH_gui_rect space)
{
   element_compute_required(e);
   element_set_bounds(e, space);
}

HLH_gui_point HLH_gui_element_size(HLH_gui_element *e, HLH_gui_point children)
{
   int width = HLH_gui_element_msg(e, HLH_GUI_MSG_GET_WIDTH, 0, (void *)&children);
   int height = HLH_gui_element_msg(e, HLH_GUI_MSG_GET_HEIGHT, 0, (void *)&children);

   return HLH_gui_point_make(width, height);
}

HLH_gui_element *HLH_gui_element_by_point(HLH_gui_element *e, HLH_gui_point pt)
{
   for(int i = 0; i<e->child_count; i++)
   {
      HLH_gui_element *c = e->children[i];

      if(HLH_gui_element_ignored(c))
         continue;
      if(HLH_gui_rect_inside(c->bounds, pt))
      {
         HLH_gui_element *leaf = HLH_gui_element_by_point(c, pt);

         if(leaf!=NULL)
            return leaf;

         return c;
      }
   }

   return NULL;
}

void HLH_gui_element_invisible(HLH_gui_element *e, int invisible)
{
   if(invisible)
      e->flags |= HLH_GUI_INVISIBLE;
   else
      e->flags &= ~HLH_GUI_INVISIBLE;

   for(int i = 0; i<e->child_count; i++)
      HLH_gui_element_invisible(e->children[i], invisible);
}

void HLH_gui_element_ignore(HLH_gui_element *e, int ignore)
{
   if(e==NULL)
      return;

   if(ignore)
      e->flags |= HLH_GUI_IGNORE;
   else
      e->flags &= ~HLH_GUI_IGNORE;
}

int HLH_gui_element_ignored(HLH_gui_element *e)
{
   if(e==NULL)
      return 0;

   if(e->flags&HLH_GUI_IGNORE)
      return 1;

   return HLH_gui_element_ignored(e->parent);
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

static Uint32 sdl_callback(Uint32 interval, void *param)
{
   HLH_gui_element *e = param;
   SDL_Event event;
   event.type = HLH_gui_timer_event;
   event.user.windowID = SDL_GetWindowID(e->window->window);
   event.user.data1 = param;
   SDL_PushEvent(&event);

   return e->timer_interval;
}

void HLH_gui_element_timer(HLH_gui_element *e, int interval)
{
   if(e->timer!=0)
   {
      SDL_RemoveTimer(e->timer);
      e->timer = 0;
   }
   e->timer_interval = interval;
   e->timer = SDL_AddTimer(interval, sdl_callback, e);
}

static void element_compute_required(HLH_gui_element *e)
{
   e->child_size_required = HLH_gui_point_make(0, 0);
   int local_x = 0;
   for(int i = 0;i<e->child_count;i++)
   {
      HLH_gui_element *child = e->children[i];
      element_compute_required(child);

      uint64_t layout = child->flags & HLH_GUI_LAYOUT;
      if(layout==HLH_GUI_LAYOUT_VERTICAL)
      {
         e->child_size_required.x = HLH_max(local_x+child->size_required.x,e->child_size_required.x);
         local_x = 0;
         e->child_size_required.y+=child->size_required.y;
      }
      else if(layout==HLH_GUI_LAYOUT_HORIZONTAL)
      {
         local_x += child->size_required.x;
         e->child_size_required.y = HLH_max(e->child_size_required.y, child->size_required.y);
      }
   }

   e->child_size_required.x = HLH_max(local_x,e->child_size_required.x);
   e->size_required = HLH_gui_point_add(HLH_gui_element_size(e, e->child_size_required), HLH_gui_point_add(e->pad_in, e->pad_out));
}

static void element_set_bounds(HLH_gui_element *e, HLH_gui_rect availible)
{
   availible.minx+=e->pad_out.x;
   availible.miny+=e->pad_out.y;
   availible.maxx-=e->pad_out.x;
   availible.maxy-=e->pad_out.y;

   e->bounds = availible;
   if(!(e->flags&HLH_GUI_FILL_X)&&
      availible.maxx-availible.minx>e->size_required.x)
   {
      if(!(e->flags&HLH_GUI_NO_CENTER_X))
         e->bounds.minx+= (e->bounds.maxx-e->bounds.minx-e->size_required.x+e->pad_out.x)/2;
      e->bounds.maxx = e->bounds.minx+e->size_required.x-e->pad_out.x;
   }
   if(!(e->flags&HLH_GUI_FILL_Y)&&
      availible.maxy-availible.miny>e->size_required.y)
   {
      if(!(e->flags&HLH_GUI_NO_CENTER_Y))
         e->bounds.miny+= (e->bounds.maxy-e->bounds.miny-e->size_required.y+e->pad_out.y)/2;
      e->bounds.maxy = e->bounds.miny+e->size_required.y-e->pad_out.y;
   }
   
   if(e->child_count<=0)
      return;

   //TODO(Captain4LK): this won't work properly when mixing fill_x and layout_vertical
   HLH_gui_point share = HLH_gui_point_make(0, 0);
   for(int i = 0; i<e->child_count; i++)
   {
      HLH_gui_element *child = e->children[i];

      uint64_t layout = child->flags & HLH_GUI_LAYOUT;
      if(layout==HLH_GUI_LAYOUT_VERTICAL&&(child->flags&HLH_GUI_FILL_Y))
         share.y++;
      else if(layout==HLH_GUI_LAYOUT_HORIZONTAL&&(child->flags&HLH_GUI_FILL_X))
         share.x++;
   }
   share.x = HLH_max(share.x,1);
   share.y = HLH_max(share.y,1);

   HLH_gui_rect child_space = e->bounds;
   HLH_gui_element_msg(e,HLH_GUI_MSG_GET_CHILD_SPACE,0,&child_space);
   HLH_gui_point space_fill = HLH_gui_point_make(child_space.maxx -child_space.minx - e->child_size_required.x, child_space.maxy-child_space.miny - e->child_size_required.y);
   HLH_gui_rect child_space_org = child_space;
   for(int i = 0; i<e->child_count; i++)
   {
      HLH_gui_element *c = e->children[i];

      if(HLH_gui_element_ignored(c))
         continue;

      uint64_t layout = c->flags & HLH_GUI_LAYOUT;
      if(layout==HLH_GUI_LAYOUT_VERTICAL)
      {
         if(c->flags&HLH_GUI_FILL_X)
            c->size_required.x += space_fill.x;
         if(c->flags&HLH_GUI_FILL_Y)
            c->size_required.y += space_fill.y / HLH_non_zero(share.y);

         element_set_bounds(c, HLH_gui_rect_make(child_space.minx,child_space.miny,child_space.maxx,child_space.miny+c->size_required.y));
         child_space.miny+=c->size_required.y;
         child_space.minx = child_space_org.minx;
         child_space.maxx = child_space_org.maxx;
      }
      else if(layout==HLH_GUI_LAYOUT_HORIZONTAL)
      {
         if(c->flags&HLH_GUI_FILL_X)
            c->size_required.x += space_fill.x / HLH_non_zero(share.x);
         if(c->flags&HLH_GUI_FILL_Y)
            c->size_required.y += space_fill.y;

         element_set_bounds(c, HLH_gui_rect_make(child_space.minx,child_space.miny,child_space.minx+c->size_required.x,child_space.maxy));
         child_space.minx+=c->size_required.x;
      }
   }
}

static void element_redraw(HLH_gui_element *e)
{
   if(e->flags & HLH_GUI_INVISIBLE||HLH_gui_element_ignored(e))
      return;

   HLH_gui_element_msg(e, HLH_GUI_MSG_DRAW, 0, NULL);
   e->needs_redraw = 0;

   for(int i = 0; i<e->child_count; i++)
      element_redraw(e->children[i]);
}
//-------------------------------------
