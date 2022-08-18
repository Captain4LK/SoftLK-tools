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
static int htab_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
//-------------------------------------

//Function implementations

HLH_gui_htab *HLH_gui_htab_create(HLH_gui_element *parent, uint32_t flags)
{
   HLH_gui_htab *htab = (HLH_gui_htab *) HLH_gui_element_create(sizeof(*htab),parent,flags,htab_msg);
   htab->tabs = 0;
   htab->tab_current = 0;

   return htab;
}

void HLH_gui_htab_set(HLH_gui_htab *h, int tab, const char *str)
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

static int htab_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_htab *t = e;
   int width = (e->bounds.r-e->bounds.l)/t->tabs;

   if(msg==HLH_GUI_MSG_DESTROY)
   {
      for(int i = 0;i<t->tabs;i++)
         if(t->labels[i]!=NULL)
            free(t->labels[i]);
      free(t->labels);
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      //TODO
      return -1;
   }
   else if(msg==HLH_GUI_MSG_CLICK)
   {
      int tab = (e->window->mouse_x-e->bounds.l)/width;
      if(tab<0||tab>=t->tabs)
         return 0;

      e->parent->children[t->tab_current+1]->flags|=HLH_GUI_HIDDEN;
      t->tab_current = tab;
      e->parent->children[t->tab_current+1]->flags^=HLH_GUI_HIDDEN;

      HLH_gui_element_msg(e->parent,HLH_GUI_MSG_LAYOUT,0,NULL);
      HLH_gui_element_repaint(e->parent,NULL);
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return (HLH_GUI_GLYPH_HEIGHT+5)*HLH_gui_get_scale();
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
            HLH_gui_draw_block(painter,HLH_gui_rect_make(e->bounds.l+i*width,e->bounds.l+(i+1)*width,e->bounds.t,e->bounds.b),0x646464);
         HLH_gui_draw_string(dp,HLH_gui_rect_make(e->bounds.l+i*width,e->bounds.l+(i+1)*width,e->bounds.t,e->bounds.b),t->labels[i],strlen(t->labels[i]),0x000000,HLH_GUI_LABEL_CENTER);
      }
   }

   return 0;
}
//-------------------------------------
