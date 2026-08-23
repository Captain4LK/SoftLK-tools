/*
HLH_gui - a small immediate-mode-ish gui library

See HLH_gui_theme.h.

Written in 2026 by June / SerbianKnifeFight, released to the public domain (CC0), same terms as the
rest of this project - see COPYING.
*/

#include "HLH_gui_theme.h"

HLH_gui_theme HLH_gui_theme_current =
{
   .bg = 0xff5a5a5a,
   .border = 0xff000000,
   .bevel_dark = 0xff323232,
   .bevel_light = 0xffc8c8c8,
   .text = 0xff000000,
};

void HLH_gui_theme_set_default(void)
{
   HLH_gui_theme_current.bg = 0xff5a5a5a;
   HLH_gui_theme_current.border = 0xff000000;
   HLH_gui_theme_current.bevel_dark = 0xff323232;
   HLH_gui_theme_current.bevel_light = 0xffc8c8c8;
   HLH_gui_theme_current.text = 0xff000000;
}
