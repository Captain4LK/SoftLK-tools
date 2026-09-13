/*
HLH_gui - gui framework

Written in 2023, 2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef _HLH_GUI_INTERNAL_H_

#define _HLH_GUI_INTERNAL_H_

typedef struct
{
   SDL_DialogFileFilter *filters;
   size_t filters_size;

   int32_t ident;
   HLH_gui_window *window;
}HLH_gui_dialog_internal_ctx;

SDL_Texture *HLH_gui_texture_load(HLH_gui_window *win, const char *path, int *width, int *height);
SDL_Texture *HLH_gui_texture_from_data(HLH_gui_window *win, uint32_t *pix, int width, int height);

char *HLH_gui_strdup(const char *str);

uint32_t core_timer_callback(void *userdata, SDL_TimerID timer_id, uint32_t interval);
void core_open_file_callback(void *userdata, const char * const *filelist, int filter);
void core_save_file_callback(void *userdata, const char * const *filelist, int filter);
void core_open_folder_callback(void *userdata, const char * const *folderlist, int filter);

#endif
