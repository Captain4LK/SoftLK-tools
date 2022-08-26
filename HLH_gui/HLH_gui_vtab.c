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
static int vtab_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_vtab *HLH_gui_vtab_create(HLH_gui_element *parent, uint32_t flags)
{
   HLH_gui_vtab *vtab = (HLH_gui_vtab *) HLH_gui_element_create(sizeof(*vtab),parent,flags,vtab_msg);
   vtab->tabs = 0;
   vtab->tab_current = 0;

   return vtab;
}

void HLH_gui_vtab_add(HLH_gui_vtab *h, int tab, const char *str)
{
   int max = (tab+1)>h->tabs?tab+1:h->tabs;
   if(max>h->tabs)
   {
      h->tabs = tab+1;
      h->labels = realloc(h->labels,sizeof(*h->labels)*h->tabs);
      h->labels[tab] = NULL;
   }
   if(tab!=0)
      h->e.parent->children[tab+1]->flags|=HLH_GUI_HIDDEN;

   HLH_gui_string_copy(&h->labels[tab],NULL,str,-1);
}

void HLH_gui_vtab_set(HLH_gui_vtab *t, int tab)
{
   if(tab<0||tab>=t->tabs||tab==t->tab_current)
      return;

   int old = t->tab_current;
   t->e.parent->children[t->tab_current+1]->flags|=HLH_GUI_HIDDEN;
   t->tab_current = tab;
   t->e.parent->children[t->tab_current+1]->flags&=~HLH_GUI_HIDDEN;

   HLH_gui_element_msg(t->e.parent,HLH_GUI_MSG_LAYOUT,0,NULL);
   HLH_gui_element_repaint(t->e.parent,NULL);
   HLH_gui_element_msg(&t->e,HLH_GUI_MSG_TAB_CHANGED,old,NULL);
}

static int vtab_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_vtab *t = (HLH_gui_vtab *) e;
   int height = (HLH_GUI_GLYPH_HEIGHT+4)*HLH_gui_get_scale();

   if(msg==HLH_GUI_MSG_DESTROY)
   {
      for(int i = 0;i<t->tabs;i++)
         if(t->labels[i]!=NULL)
            free(t->labels[i]);
      free(t->labels);
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      //return max width of labels
      int max = 0;
      for(int i = 0;i<t->tabs;i++)
      {
         int width = HLH_GUI_GLYPH_WIDTH*strlen(t->labels[i])*HLH_gui_get_scale();
         if(width>max)
            max = width;
      }
      return max+20;
   }
   else if(msg==HLH_GUI_MSG_CLICK)
   {
      int tab = (e->window->mouse_y-e->bounds.t)/height;
      HLH_gui_vtab_set(t,tab);
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      //TODO
      return -1;
   }
   else if(msg==HLH_GUI_MSG_PAINT)
   {
      //Draw background
      HLH_gui_painter *painter = dp;
      HLH_gui_draw_block(painter,e->bounds,0x323232);

      //Draw labels
      for(int i = 0;i<t->tabs;i++)
      {
         if(i==t->tab_current)
            HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l,e->bounds.r,e->bounds.t+height*i,e->bounds.t+(i+1)*height),0x646464);
         HLH_gui_draw_string(dp,HLH_gui_rect_make(e->bounds.l,e->bounds.r,e->bounds.t+height*i,e->bounds.t+(i+1)*height),t->labels[i],strlen(t->labels[i]),0x000000,HLH_GUI_LABEL_CENTER);
      }
   }

   return 0;
}
//-------------------------------------
