/*
HLH_gui - gui framework

Written in 2022 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
//-------------------------------------

//Internal includes
#include "HLH_gui.h"
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

HLH_gui_element *HLH_gui_element_create(size_t bytes, HLH_gui_element *parent, uint32_t flags, HLH_gui_msg_handler msg_handler)
{
   HLH_gui_element *e = calloc(1,bytes);
   e->flags = flags;
   e->msg_class = msg_handler;

   if(parent!=NULL)
   {
      e->window = parent->window;
      e->parent = parent;
      parent->child_count++;
      parent->children = realloc(parent->children,sizeof(*parent->children)*parent->child_count);
      parent->children[parent->child_count-1] = e;
   }

   return e;
}

void HLH_gui_element_move(HLH_gui_element *e, HLH_gui_rect bounds, int always_layout)
{
   HLH_gui_rect old_clip = e->clip;
   e->clip = HLH_gui_rect_intersect(e->parent->clip,bounds);

   if(!HLH_gui_rect_equal(e->bounds,bounds)||!(HLH_gui_rect_equal(e->clip,old_clip))||always_layout)
   {
      e->bounds = bounds;
      HLH_gui_element_msg(e,HLH_GUI_MSG_LAYOUT,0,NULL);
      HLH_gui_element_repaint(e,NULL);
   }
}

void HLH_gui_element_repaint(HLH_gui_element *e, HLH_gui_rect *region)
{
   if(e->flags&HLH_GUI_HIDDEN)
      return;

   if(region==NULL)
      region = &e->bounds;

   HLH_gui_rect r = HLH_gui_rect_intersect(*region,e->clip);

   if(HLH_gui_rect_valid(r))
   {
      if(HLH_gui_rect_valid(e->window->update_region))
         e->window->update_region = HLH_gui_rect_bounding(e->window->update_region,r);
      else
         e->window->update_region = r;
   }
}

int HLH_gui_element_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   if(e->flags&HLH_GUI_DESTROY&&msg!=HLH_GUI_MSG_DESTROY)
      return 0;
   if(e->flags&HLH_GUI_HIDDEN)
      return 0;

   if(e->msg_usr!=NULL)
   {
      int res = e->msg_usr(e,msg,di,dp);
      if(res)
         return res;
   }

   if(e->msg_class!=NULL)
      return e->msg_class(e,msg,di,dp);
   return 0;
}

HLH_gui_element *HLH_gui_element_find_by_point(HLH_gui_element *e, int x, int y)
{
   if(e->flags&HLH_GUI_HIDDEN)
      return NULL;
   for(int i = 0;i<e->child_count;i++)
      if(HLH_gui_rect_inside(e->children[i]->clip,x,y)&&!(e->children[i]->flags&HLH_GUI_HIDDEN))
         return HLH_gui_element_find_by_point(e->children[i],x,y);
   return e;
}

void HLH_gui_element_destroy(HLH_gui_element *e)
{
   if(e->flags&HLH_GUI_DESTROY)
      return;

   e->flags|=HLH_GUI_DESTROY;

   HLH_gui_element *ancestor = e->parent;
   while(ancestor!=NULL)
   {
      ancestor->flags|=HLH_GUI_DESTROY_DESCENDENT;
      ancestor = ancestor->parent;
   }

   for(int i = 0;i<e->child_count;i++)
      HLH_gui_element_destroy(e->children[i]);
}
//-------------------------------------
