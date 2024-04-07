/*
HLH_gui - gui framework

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
static int entry_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp);
static void entry_draw(HLH_gui_entry *e);
//-------------------------------------

//Function implementations

HLH_gui_entry *HLH_gui_entry_create(HLH_gui_element *parent, uint64_t flags, int max_len)
{
   HLH_gui_entry *entry = (HLH_gui_entry *) HLH_gui_element_create(sizeof(*entry), parent, flags, entry_msg);

   entry->len = 0;
   entry->max_len = max_len;
   entry->entry = calloc(1,entry->max_len+1);
   entry->state = 0;

   return entry;
}

void HLH_gui_entry_set(HLH_gui_entry *entry, char *text)
{
   if(entry==NULL)
      return;
   if(text==NULL)
      return;
   if(strcmp(entry->entry,text)==0)
      return;

   strncpy(entry->entry,text,entry->max_len);
   entry->entry[entry->max_len] = '\0';
   entry->len = strlen(entry->entry);
   HLH_gui_element_redraw(&entry->e);
}

static int entry_msg(HLH_gui_element *e, HLH_gui_msg msg, int di, void *dp)
{
   HLH_gui_entry *entry = (HLH_gui_entry *)e;

   if(msg==HLH_GUI_MSG_DESTROY)
   {
      if(entry->entry!=NULL)
         free(entry->entry);
      entry->entry = NULL;
   }
   else if(msg==HLH_GUI_MSG_GET_WIDTH)
   {
      return entry->max_len * HLH_GUI_GLYPH_WIDTH * HLH_gui_get_scale() + 12 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_GET_HEIGHT)
   {
      return HLH_GUI_GLYPH_HEIGHT * HLH_gui_get_scale() + 10 * HLH_gui_get_scale();
   }
   else if(msg==HLH_GUI_MSG_DRAW)
   {
      entry_draw(entry);
   }
   else if(msg==HLH_GUI_MSG_HIT)
   {
      HLH_gui_mouse *m = dp;

      if(m->button & HLH_GUI_MOUSE_OUT)
      {
         entry->state = 0;
      }
      else if(m->button & (HLH_GUI_MOUSE_LEFT | HLH_GUI_MOUSE_RIGHT | HLH_GUI_MOUSE_MIDDLE))
      {
         entry->state = 1;
      }
      else if(entry->state)
      {
         HLH_gui_textinput_start(&entry->e);
         entry->state = 0;
      }
   }
   else if(msg==HLH_GUI_MSG_TEXTINPUT)
   {
      HLH_gui_textinput *input = dp;
      int changed = 0;

      if(input->type==0)
      {
         if(entry->len<entry->max_len)
         {
            entry->entry[entry->len] = input->ch;
            entry->entry[entry->len+1] = '\0';
            entry->len++;
            changed = 1;
         }
      }
      else if(input->type==1)
      {
         if(input->keycode==SDLK_RETURN)
         {
            HLH_gui_textinput_stop(entry->e.window);
            changed = 1;
         }
         else if(input->keycode==SDLK_BACKSPACE)
         {
            if(entry->len>0)
            {
               entry->len--;
               changed = 1;
               entry->entry[entry->len] = '\0';
            }
         }
      }

      if(changed)
         HLH_gui_element_redraw(&entry->e);
   }

   return 0;
}

static void entry_draw(HLH_gui_entry *e)
{
   int scale = HLH_gui_get_scale();
   HLH_gui_rect bounds = e->e.bounds;

   //Infill
   HLH_gui_draw_rectangle_fill(&e->e, bounds, 0xff5a5a5a);


   //Border
   HLH_gui_draw_rectangle_fill(&e->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.miny + 2 * scale, bounds.minx + 2 * scale, bounds.maxy - 1 * scale), 0xffc8c8c8);
   HLH_gui_draw_rectangle_fill(&e->e, HLH_gui_rect_make(bounds.minx + 1 * scale, bounds.maxy - 2 * scale, bounds.maxx - 2 * scale, bounds.maxy - 1 * scale), 0xffc8c8c8);

   HLH_gui_draw_rectangle_fill(&e->e, HLH_gui_rect_make(bounds.maxx - 2 * scale, bounds.miny + 2 * scale, bounds.maxx - 1 * scale, bounds.maxy - 2 * scale), 0xff323232);
   HLH_gui_draw_rectangle_fill(&e->e, HLH_gui_rect_make(bounds.minx + 2 * scale, bounds.miny + 1 * scale, bounds.maxx - 1 * scale, bounds.miny + 2 * scale), 0xff323232);

   HLH_gui_draw_string(&e->e, bounds, e->entry, e->len, 0xff000000, 1);
}
//-------------------------------------
