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

HLH_gui_rect HLH_gui_rect_make(int l, int r, int t, int b)
{
   return (HLH_gui_rect){.l = l, .r = r, .t = t, .b = b};
}

HLH_gui_rect HLH_gui_rect_intersect(HLH_gui_rect a, HLH_gui_rect b)
{
   if(a.l<b.l) a.l = b.l;
   if(a.t<b.t) a.t = b.t;
   if(a.r>b.r) a.r = b.r;
   if(a.b>b.b) a.b = b.b;

   return a;
}

HLH_gui_rect HLH_gui_rect_bounding(HLH_gui_rect a, HLH_gui_rect b)
{
   if(a.l>b.l) a.l = b.l;
   if(a.t>b.t) a.t = b.t;
   if(a.r<b.r) a.r = b.r;
   if(a.b<b.b) a.b = b.b;

   return a;
}

int HLH_gui_rect_valid(HLH_gui_rect r)
{
   return r.r>r.l&&r.b>r.t;
}

int HLH_gui_rect_equal(HLH_gui_rect a, HLH_gui_rect b)
{
   return (a.l==b.l)&&(a.r==b.r)&&(a.t==b.t)&&(a.b==b.b);
}

int HLH_gui_rect_inside(HLH_gui_rect a, int x, int y)
{
   return a.l<=x&&a.r>x&&a.t<=y&&a.b>y;
}
//-------------------------------------
