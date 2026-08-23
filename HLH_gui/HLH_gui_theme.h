/*
HLH_gui - a small immediate-mode-ish gui library

Theme support - pulls the widgets' hardcoded colors into one struct.

Written in 2026 by June / SerbianKnifeFight, released to the public domain (CC0), same terms as the
rest of this project - see COPYING.
*/

#ifndef _HLH_GUI_THEME_H_

#define _HLH_GUI_THEME_H_

#include <stdint.h>

typedef struct
{
   uint32_t bg;
   uint32_t border;
   uint32_t bevel_dark;
   uint32_t bevel_light;
   uint32_t text;
}HLH_gui_theme;

extern HLH_gui_theme HLH_gui_theme_current;

void HLH_gui_theme_set_default(void);

#endif
