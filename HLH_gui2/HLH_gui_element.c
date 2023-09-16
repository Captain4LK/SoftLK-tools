/*
HLH_gui - gui framework

Written in 2023 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
//-------------------------------------

//Internal includes
#include "HLH_gui.h"
//-------------------------------------

//#defines
#define hlh_gui_max(a,b) ((a)>(b)?(a):(b))
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static void element_compute_required(HLH_gui_element *e);
static HLH_gui_point element_size_siblings(HLH_gui_element *e);
//-------------------------------------

//Function implementations

HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint32_t flags, HLH_gui_msg_handler msg_handler)
{
   HLH_gui_element *e = calloc(1,bytes);
   e->flags = flags;
   e->msg_base = msg_handler;

   if(parent!=NULL)
   {
      e->window = parent->window;
      e->parent = parent;
      parent->child_count++;
      parent->children = realloc(parent->children,sizeof(*parent->children)*parent->child_count);
      parent->children[parent->child_count-1] = e;

      if(parent->child_count>1)
         parent->children[parent->child_count-2]->next = e;
   }

   return e;
}

int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(e->flags&HLH_GUI_DESTROY&&msg!=HLH_GUI_MSG_DESTROY)
      return 0;
   if(e->flags&HLH_GUI_IGNORE)
      return 0;
   if(msg==HLH_GUI_MSG_DRAW&&e->flags&HLH_GUI_INVISIBLE)
      return 0;

   if(e->msg_usr!=NULL)
   {
      int res = e->msg_usr(e,msg,di,dp);
      if(res)
         return res;
   }

   if(e->msg_base!=NULL)
      return e->msg_base(e,msg,di,dp);
   return 0;
}

void HLH_gui_element_redraw(HLH_gui_element *e)
{
   if(e->flags&HLH_GUI_INVISIBLE||e->flags&HLH_GUI_IGNORE)
      return;

   HLH_gui_element_msg(e,HLH_GUI_MSG_DRAW,0,NULL);

   for(int i = 0;i<e->child_count;i++)
      HLH_gui_element_redraw(e->children[i]);
}

void HLH_gui_element_pack(HLH_gui_element *e, HLH_gui_rect space)
{
   element_compute_required(e);
}

static void element_compute_required(HLH_gui_element *e)
{
   HLH_gui_point size_max = HLH_gui_point_make(0,0);

   for(int i = 0;i<e->child_count;i++)
   {
      HLH_gui_element *child = e->children[i];

      element_compute_required(child);

      if(child->size_required.x>size_max.x)
         size_max.x = child->size_required.x;
      if(child->size_required.y>size_max.y)
         size_max.y = child->size_required.y;
   }

   for(int i = 0;i<e->child_count;i++)
   {
      HLH_gui_element *child = e->children[i];

      if(child->flags&HLH_GUI_MAX_X)
         child->size_required.x = size_max.x;
      if(child->flags&HLH_GUI_MAX_Y)
         child->size_required.y = size_max.y;
   }

   e->child_size_required = element_size_siblings(e);
}

static HLH_gui_point element_size_siblings(HLH_gui_element *e)
{
   HLH_gui_point size = HLH_gui_point_make(0,0);
   if(e==NULL)
      return size;

   size = element_size_siblings(e->next);

   if(e->flags&HLH_GUI_PACK_NORTH||e->flags&HLH_GUI_PACK_SOUTH)
   {
      size.x = hlh_gui_max(size.x,e->size_required.x);
      size.y+=e->size_required.y;
   }
   else if(e->flags&HLH_GUI_PACK_EAST||e->flags&HLH_GUI_PACK_WEST)
   {
   }

   return size;
}

#undef hlh_gui_max
//-------------------------------------
